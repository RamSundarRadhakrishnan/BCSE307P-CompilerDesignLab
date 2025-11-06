#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <set>
#include <unordered_map>
using namespace std;

static bool inVec(const vector<string>& v, const string& s){
    return find(v.begin(), v.end(), s)!=v.end();
}

static vector<string> tokenizeProduction(const string& rhs, const vector<string>& terminals, const vector<string>& nonTerminals){
    vector<string> all = nonTerminals;
    all.insert(all.end(), terminals.begin(), terminals.end());
    all.push_back("epsilon");
    sort(all.begin(), all.end(), [](const string& a, const string& b){ return a.size()>b.size(); });
    vector<string> out;
    size_t i=0;
    while(i<rhs.size()){
        bool matched=false;
        for(const auto& tok: all){
            if(tok.size()<=rhs.size()-i && rhs.compare(i, tok.size(), tok)==0){
                out.push_back(tok);
                i += tok.size();
                matched=true;
                break;
            }
        }
        if(!matched){
            out.emplace_back(1, rhs[i]);
            i++;
        }
    }
    return out;
}

static bool isTerminalSym(const string& s, const vector<string>& terminals){
    return s!="epsilon" && inVec(terminals, s);
}

static bool isNonTerminalSym(const string& s, const vector<string>& nonTerminals){
    return inVec(nonTerminals, s);
}

static void FIRSTFunction(const multimap<string,string>& grammar, const vector<string>& nonTerminals, const vector<string>& terminals, const string& current, map<string,set<string>>& FIRST, set<string>& visiting){
    if(FIRST.count(current)) return;
    if(visiting.count(current)) return;
    visiting.insert(current);
    auto range = grammar.equal_range(current);
    for(auto it=range.first; it!=range.second; ++it){
        auto tokens = tokenizeProduction(it->second, terminals, nonTerminals);
        bool allEps=true;
        for(size_t i=0;i<tokens.size();++i){
            const string& sym = tokens[i];
            if(sym=="epsilon"){
                FIRST[current].insert("epsilon");
                allEps=false;
                break;
            }else if(isTerminalSym(sym, terminals)){
                FIRST[current].insert(sym);
                allEps=false;
                break;
            }else if(isNonTerminalSym(sym, nonTerminals)){
                if(!FIRST.count(sym)) FIRSTFunction(grammar, nonTerminals, terminals, sym, FIRST, visiting);
                for(const auto& x: FIRST[sym]){
                    if(x!="epsilon") FIRST[current].insert(x);
                }
                if(FIRST[sym].count("epsilon")) continue;
                allEps=false;
                break;
            }else{
                FIRST[current].insert(sym);
                allEps=false;
                break;
            }
        }
        if(allEps) FIRST[current].insert("epsilon");
    }
    visiting.erase(current);
}

static set<string> firstOfSequence(const vector<string>& seq, size_t startIdx, const map<string,set<string>>& FIRST, const vector<string>& terminals, const vector<string>& nonTerminals, bool& derivesEpsilon){
    set<string> res;
    derivesEpsilon=true;
    for(size_t i=startIdx;i<seq.size();++i){
        const string& s = seq[i];
        if(s=="epsilon") continue;
        if(isTerminalSym(s, terminals)){
            res.insert(s);
            derivesEpsilon=false;
            return res;
        }else if(isNonTerminalSym(s, nonTerminals)){
            for(const auto& x: FIRST.at(s)){
                if(x!="epsilon") res.insert(x);
            }
            if(FIRST.at(s).count("epsilon")) continue;
            derivesEpsilon=false;
            return res;
        }else{
            res.insert(s);
            derivesEpsilon=false;
            return res;
        }
    }
    return res;
}

static void FOLLOWFunction(const multimap<string,string>& grammar, const vector<string>& nonTerminals, const vector<string>& terminals, const map<string,set<string>>& FIRST, const string& start, map<string,set<string>>& FOLLOW){
    for(const auto& nt: nonTerminals) FOLLOW[nt];
    if(!start.empty()) FOLLOW[start].insert("$");
    bool changed=true;
    while(changed){
        changed=false;
        for(const auto& pr: grammar){
            const string& A = pr.first;
            auto rhsTok = tokenizeProduction(pr.second, terminals, nonTerminals);
            for(size_t i=0;i<rhsTok.size();++i){
                const string& B = rhsTok[i];
                if(!isNonTerminalSym(B, nonTerminals)) continue;
                bool derivesEps=false;
                auto afterFirst = firstOfSequence(rhsTok, i+1, FIRST, terminals, nonTerminals, derivesEps);
                size_t before = FOLLOW[B].size();
                for(const auto& x: afterFirst) if(x!="epsilon") FOLLOW[B].insert(x);
                if(derivesEps || i+1==rhsTok.size()){
                    FOLLOW[B].insert(FOLLOW[A].begin(), FOLLOW[A].end());
                }
                if(FOLLOW[B].size()!=before) changed=true;
            }
        }
    }
}



int main(){
    vector<string> terminals;
    vector<string> nonTerminals;
    vector<bool> grammarHasLeftFactoring;
    int numTerminals, numNonTerminals;
    cout<<"Enter number of terminals and non terminals: ";
    cin>>numTerminals;
    cin>>numNonTerminals;
    for(int i=0; i<numTerminals; i++){
        cout<<"Enter the terminal:";
        string input;
        cin>>input;
        terminals.push_back(input);
    }
     for(int i=0; i<numNonTerminals; i++){
        cout<<"Enter the non terminal:";
        string input;
        cin>>input;
        nonTerminals.push_back(input);
        grammarHasLeftFactoring.push_back(false);
    }
    
    
    multimap<string,string> grammar;
    vector<bool> grammarHasLeftRecursion;
    multimap<string,string> leftFactoredGrammar;
    multimap<string,string> leftRecursiveEliminatedGrammar;
    int numProductions;
    cout<<"Enter the number of productions:";
    cin>>numProductions;
    for(int i=0; i<numProductions; i++){
        string lhs, rhs;
        cout<<"Enter the LHS (NT):";
        cin>>lhs;
        cout<<"Enter the RHS (NT|T)*:";
        cin>>rhs;
        grammar.emplace(lhs,rhs);
        grammarHasLeftRecursion.push_back(false);
    }
    
    leftRecursiveEliminatedGrammar.clear();
    vector<string> originalNT = nonTerminals;
    for(const auto& A : originalNT){
        auto range = grammar.equal_range(A);
        if(range.first==range.second) continue;
        vector<string> alpha, beta;
        for(auto it=range.first; it!=range.second; ++it){
            const string& rhs = it->second;
            if(rhs.rfind(A, 0)==0) alpha.push_back(rhs.substr(A.size()));
            else beta.push_back(rhs);
        }
        if(alpha.empty()){
            for(const auto& b: beta) leftRecursiveEliminatedGrammar.emplace(A, b);
        }
        else{
            string Aprime = A + "'";
            nonTerminals.push_back(Aprime);
            numNonTerminals++;
            grammarHasLeftFactoring.push_back(false);
            for(const auto& b: beta) leftRecursiveEliminatedGrammar.emplace(A, b + Aprime);
            for(const auto& a: alpha) leftRecursiveEliminatedGrammar.emplace(Aprime, a + Aprime);
            leftRecursiveEliminatedGrammar.emplace(Aprime, "epsilon");
        }
    }

    for(auto it:leftRecursiveEliminatedGrammar){
        cout<<it.first<<"->"<<it.second<<endl;
    }

    
    for(int i=0; i<numNonTerminals; i++){
        auto range = leftRecursiveEliminatedGrammar.equal_range(nonTerminals[i]);
        vector<string> tempProd;
        for(auto iterator=range.first; iterator!=range.second; ++iterator){
            tempProd.push_back(iterator->second);
        }
        string commonPrefix;
        sort(tempProd.begin(), tempProd.end());
        for(int j=0; j<tempProd[0].size(); j++){
            for(int k=1; k<tempProd.size(); k++){
                if(tempProd[k].size()<=j || tempProd[k][j]!=tempProd[0][j]){
                    commonPrefix = tempProd[0].substr(0, j);
                }
            }
        }
        if(commonPrefix.size()==0) grammarHasLeftFactoring[i] = false;
        else grammarHasLeftFactoring[i] = true;
    }
    
    map<string,vector<string>> FIRST;
    map<string,vector<string>> FOLLOW;
    
    map<string,set<string>> firstSets;
    set<string> visiting;
    for(int i=0;i<numNonTerminals;i++){
        FIRSTFunction(leftRecursiveEliminatedGrammar, nonTerminals, terminals, nonTerminals[i], firstSets, visiting);
    }
    for(const auto& nt: nonTerminals){
        vector<string> v(firstSets[nt].begin(), firstSets[nt].end());
        FIRST[nt]=v;
    }

    map<string,set<string>> followSets;
    string startSymbol = nonTerminals.empty() ? string() : nonTerminals[0];
    FOLLOWFunction(leftRecursiveEliminatedGrammar, nonTerminals, terminals, firstSets, startSymbol, followSets);
    for(const auto& nt: nonTerminals){
        vector<string> v(followSets[nt].begin(), followSets[nt].end());
        FOLLOW[nt]=v;
    }

    for(const auto& nt: nonTerminals){
        cout<<"FIRST("<<nt<<") = {";
        for(size_t i=0;i<FIRST[nt].size();++i) cout<<FIRST[nt][i]<<(i+1<FIRST[nt].size()? ", ":"");
        cout<<"}\n";
    }
    for(const auto& nt: nonTerminals){
        cout<<"FOLLOW("<<nt<<") = {";
        for(size_t i=0;i<FOLLOW[nt].size();++i) cout<<FOLLOW[nt][i]<<(i+1<FOLLOW[nt].size()? ", ":"");
        cout<<"}\n";
    }

    vector<string> termsPlus = terminals;
    termsPlus.push_back("$");
    unordered_map<string, int> ntIdx, tIdx;

    for(int i=0; i<nonTerminals.size(); i++){
        ntIdx[nonTerminals[i]] = i;
    }

    for(int j=0; j<termsPlus.size(); j++){
        tIdx[termsPlus[j]] = j;
    }

    vector<vector<string>> parseTable(nonTerminals.size(), vector<string>(termsPlus.size(), ""));
    bool ll1_ok = true;
    auto put = [&](const string& A, const string& a, const string& rhs){
        auto itN = ntIdx.find(A);
        auto itT = tIdx.find(a);
        if(itN==ntIdx.end()||itT==tIdx.end()) return;
        int r = itN->second;
        int c = itT->second;
        if(parseTable[r][c].empty()||parseTable[r][c]==rhs) parseTable[r][c]=rhs;
        else{
            parseTable[r][c] = "CONFLICT";
            ll1_ok = false;
        }
    };

    for(const auto& pr:leftRecursiveEliminatedGrammar){
        const string& A = pr.first;
        const string& rhs = pr.second;
        auto tokens = tokenizeProduction(rhs, terminals, nonTerminals);
        bool derivesEps = false;
        auto firstAlpha = firstOfSequence(tokens, 0, firstSets, terminals, nonTerminals, derivesEps);
        for(const auto& a: firstAlpha){
            if(a!="epsilon") put(A, a, rhs);
        }
        bool isEps = (tokens.size()==1 && tokens[0]=="epsilon") || derivesEps;
        if(isEps){
            for(const auto& b : followSets.at(A)){
                put(A, b, "epsilon");
            }
        }
    }

    cout<<"NT";
    for(const auto& t: termsPlus){
        cout<<'\t'<<t;
    }
    cout<<'\n';
    for(size_t i=0;i<nonTerminals.size();++i){
        cout<<nonTerminals[i];
        for(size_t j=0;j<termsPlus.size();++j){
            string v = parseTable[i][j];
            if(v.empty()) v = "-";
            else if(v!="CONFLICT") v = nonTerminals[i] + "->" + v;
            cout<<'\t'<<v;
        }
        cout<<'\n';
    }
    cerr<<(ll1_ok ? "Grammar is LL(1)\n" : "Grammar is not LL(1)\n");
}