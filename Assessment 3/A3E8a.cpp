#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
using namespace std;

static vector<string> tokenize(const string& rhs, const vector<string>& terminals, const vector<string>& nonterms){
    vector<string> dict = nonterms;
    dict.insert(dict.end(), terminals.begin(), terminals.end());
    sort(dict.begin(), dict.end(), [](const string& a, const string& b){ return a.size()>b.size(); });
    vector<string> tok;
    string s = rhs;
    size_t i=0,n=s.size();
    while(i<n){
        if(isspace((unsigned char)s[i])){ ++i; continue; }
        bool matched=false;
        for(const auto& t: dict){
            size_t L=t.size();
            if(L>0 && i+L<=n && s.compare(i,L,t)==0){
                tok.push_back(t);
                i+=L;
                matched=true;
                break;
            }
        }
        if(!matched){
            string one(1,s[i]);
            tok.push_back(one);
            ++i;
        }
    }
    return tok;
}

void leading(const multimap<string, string>& grammar, const vector<string>& nonTerminals, const vector<string>& terminals, map<string, set<string>>& leadingSets) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& nt : nonTerminals) {
            auto range = grammar.equal_range(nt);
            for (auto it = range.first; it != range.second; ++it) {
                const string& rhs = it->second;
                vector<string> tokens = tokenize(rhs, terminals, nonTerminals);
                if (tokens.empty()) continue;
                const string& firstSymbol = tokens[0];
                if (find(nonTerminals.begin(), nonTerminals.end(), firstSymbol) != nonTerminals.end()) {
                    int oldSize = leadingSets[nt].size();
                    leadingSets[nt].insert(leadingSets[firstSymbol].begin(), leadingSets[firstSymbol].end());
                    if (leadingSets[nt].size() > oldSize) changed = true;
                }
                for (size_t i = 0; i < tokens.size(); ++i) {
                    if (find(terminals.begin(), terminals.end(), tokens[i]) != terminals.end()) {
                        if (i == 0 || find(nonTerminals.begin(), nonTerminals.end(), tokens[i-1]) != nonTerminals.end()) {
                           if (leadingSets[nt].insert(tokens[i]).second) {
                               changed = true;
                           }
                        }
                    }
                }
            }
        }
    }
}


void trailing(const multimap<string, string>& grammar, const vector<string>& nonTerminals, const vector<string>& terminals, map<string, set<string>>& trailingSets) {
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& nt : nonTerminals) {
            auto range = grammar.equal_range(nt);
            for (auto it = range.first; it != range.second; ++it) {
                const string& rhs = it->second;
                vector<string> tokens = tokenize(rhs, terminals, nonTerminals);
                if (tokens.empty()) continue;
                const string& lastSymbol = tokens.back();
                if (find(nonTerminals.begin(), nonTerminals.end(), lastSymbol) != nonTerminals.end()) {
                    int oldSize = trailingSets[nt].size();
                    trailingSets[nt].insert(trailingSets[lastSymbol].begin(), trailingSets[lastSymbol].end());
                    if (trailingSets[nt].size() > oldSize) changed = true;
                }
                for (int i = tokens.size() - 1; i >= 0; --i) {
                     if (find(terminals.begin(), terminals.end(), tokens[i]) != terminals.end()) {
                        if (i == tokens.size() - 1 || find(nonTerminals.begin(), nonTerminals.end(), tokens[i+1]) != nonTerminals.end()) {
                            if (trailingSets[nt].insert(tokens[i]).second) {
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main(){
    int numTerminals, numNonTerminals, numProductions;
    cout<<"Enter the number of terminals and nonterminals: "<<endl;
    cin>>numTerminals;
    cin>>numNonTerminals;
    vector<string> terminals, nonTerminals;
    for(int i=0; i<numTerminals; i++){
        cout<<"Enter the terminal: ";
        string temp;
        cin>>temp;
        terminals.push_back(temp);
    }
    terminals.push_back("$");
    for(int i=0; i<numNonTerminals; i++){
        cout<<"Enter the nonterminals: ";
        string temp;
        cin>>temp;
        nonTerminals.push_back(temp);
    }
    multimap<string, string> operatorGrammar;
    cout<<"Enter the number of productions:";
    cin>>numProductions;

    for(int i=0; i<numProductions; i++){
        cout<<"Enter the LHS of the production (NT): ";
        string temp1, temp2;
        cin>>temp1;
        cout<<"Enter the RHS of the production (NT|T)* ";
        cin>>temp2;
        operatorGrammar.emplace(temp1, temp2);
    }

    map<string, set<string>> leadingSets, trailingSets;
    leading(operatorGrammar, nonTerminals, terminals, leadingSets);
    trailing(operatorGrammar, nonTerminals, terminals, trailingSets);

    map<string, map<string, char>> precedenceTable;

    for(const auto& t1 : terminals) {
        for (const auto& t2 : terminals) {
            precedenceTable[t1][t2] = '_';
        }
    }

    for (const auto& prod : operatorGrammar) {
        const string& rhs = prod.second;
        vector<string> tokens = tokenize(rhs, terminals, nonTerminals);
        for (size_t i = 0; i < tokens.size() - 1; ++i) {
            const string& X = tokens[i];
            const string& Y = tokens[i+1];

            if (find(terminals.begin(), terminals.end(), X) != terminals.end() &&
                find(terminals.begin(), terminals.end(), Y) != terminals.end()) {
                precedenceTable[X][Y] = '=';
            }

            if (find(terminals.begin(), terminals.end(), X) != terminals.end() &&
                find(nonTerminals.begin(), nonTerminals.end(), Y) != nonTerminals.end()) {
                for (const auto& t : leadingSets.at(Y)) {
                    precedenceTable[X][t] = '<';
                }
                if (i + 2 < tokens.size()) {
                    const string& Z = tokens[i + 2];
                    if (find(terminals.begin(), terminals.end(), Z) != terminals.end()) {
                        precedenceTable[X][Z] = '=';
                    }
                }
            }

            if (find(nonTerminals.begin(), nonTerminals.end(), X) != nonTerminals.end() &&
                find(terminals.begin(), terminals.end(), Y) != terminals.end()) {
                for (const auto& t : trailingSets.at(X)) {
                    precedenceTable[t][Y] = '>';
                }
            }
        }
    }

    for (const auto& t : terminals) {
        if (t != "$") {
            precedenceTable["$"][t] = '<';
            precedenceTable[t]["$"] = '>';
        }
    }

    cout << "\nOperator Precedence Table:\n";
    cout << "  ";
    for (const auto& t : terminals) {
        cout << t << " ";
    }
    cout << endl;
    for (const auto& t1 : terminals) {
        cout << t1 << " ";
        for (const auto& t2 : terminals) {
            cout << precedenceTable.at(t1).at(t2) << " ";
        }
        cout << endl;
    }

}