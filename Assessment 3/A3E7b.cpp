#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <stack>
using namespace std;

static vector<string> tokenize_symbols(const string& rhs) {
    vector<string> tok;
    size_t n = rhs.size();
    for (size_t i = 0; i < n; ) {
        unsigned char c = static_cast<unsigned char>(rhs[i]);
        if (isspace(c)) { ++i; continue; }
        if (isupper(c)) {
            size_t j = i + 1;
            while (j < n && rhs[j] == '\'') ++j;
            tok.emplace_back(rhs.substr(i, j - i));
            i = j;
        } else if (islower(c)) {
            size_t j = i + 1;
            while (j < n && islower(static_cast<unsigned char>(rhs[j]))) ++j;
            tok.emplace_back(rhs.substr(i, j - i));
            i = j;
        } else {
            tok.emplace_back(rhs.substr(i, 1));
            ++i;
        }
    }
    return tok;
}

void push_symbols_reversed(stack<string>& st, const string& rhs) {
    if (rhs.empty() || rhs == "-" || rhs == "epsilon" || rhs == "ε") return;
    auto tok = tokenize_symbols(rhs);
    for (auto it = tok.rbegin(); it != tok.rend(); ++it)
        if (*it != "ε" || *it != "epsilon") st.push(*it);
}

static bool isTerminal(const string& s, const vector<string>& terminals) {
    return find(terminals.begin(), terminals.end(), s) != terminals.end();
}

static bool isNonTerminal(const string& s, const vector<string>& nonTerminals) {
    return find(nonTerminals.begin(), nonTerminals.end(), s) != nonTerminals.end();
}

static int indexOf(const vector<string>& v, const string& s) {
    auto it = find(v.begin(), v.end(), s);
    return (it == v.end()) ? -1 : int(it - v.begin());
}

int main(){
    int numTerminals, numNonTerminals;
    vector<string> terminals, nonTerminals;
    cout<<"Enter the number of terminals and nonterminals: "<<endl;
    cin>>numTerminals;
    cin>>numNonTerminals;
    for(int i=0; i<numTerminals; i++){
        cout<<"Enter the terminal:";
        string temp;
        cin>>temp;
        terminals.push_back(temp);
    }
    terminals.push_back("$");
    for(int i=0; i<numNonTerminals; i++){
        cout<<"Enter the non terminal:";
        string temp;
        cin>>temp;
        nonTerminals.push_back(temp);
    }
    string startSymbol;
    cout<<"Enter the start non terminal: "<<endl;
    cin>>startSymbol;
    vector<vector<string>> parseTable(nonTerminals.size(), vector<string>(terminals.size(), ""));
    for(int i=0; i<nonTerminals.size(); i++){
        for(int j=0; j<terminals.size(); j++){
            cout<<"Enter the parse table entry for: "<<nonTerminals[i]<<" and "<<terminals[j]<<": ";
            string temp;
            cin>>temp;
            parseTable[i][j] = temp;
        }
    }
    string input;
    cout<<"Enter the string to parse:"<<endl;
    cin>>input;
    vector<string> inputTokens = tokenize_symbols(input);
    inputTokens.push_back("$"); 
    size_t input_ptr = 0;
    stack<string> st;
    st.push("$");
    st.push(startSymbol);
    while(true){
        string topOfStack = st.top();
        string topOfString = inputTokens[input_ptr];
        bool it = isTerminal(topOfStack, terminals);
        if(it){
            if(topOfStack!=topOfString){
                cout<<"Error while parsing - the terminals on top of stack and front of string do not match "<<topOfStack<<endl;
                return -1;
            }
            else if(topOfStack==topOfString){
                if(topOfStack=="$" && topOfString=="$"){
                    cout<<"Parsing successful!"<<endl;
                    return 1;
                }
                cout<<"Matched "<<topOfStack<<" and "<<topOfString<<endl;
                input_ptr++;
                st.pop();
            }
        }
        else{
            auto it2 = isNonTerminal(topOfStack, nonTerminals);
            if(!it2){
                cout<<"Error while parsing - foreign non terminal encountered "<<topOfStack<<endl;
                return -1;
            }
            else{
                /*int idx1 = it2 - nonTerminals.begin();
                auto it3 = isTerminal(topOfString, terminals);
                int idx2 = it3 - terminals.begin();*/
                int idx1 = indexOf(nonTerminals, topOfStack);
                int idx2 = indexOf(terminals, topOfString);
                string production = parseTable[idx1][idx2];
                if(production=="-"){
                    cout<<"Error while parsing - no rule in table cell "<<topOfStack<<" and "<<topOfString<<endl;
                }
                st.pop();
                push_symbols_reversed(st, production);
            }
        }
    }
}