#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <algorithm>
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

void parseString(const string& inputString, const vector<string>& terminals, const vector<string>& nonTerminals, const map<string, map<string, char>>& precedenceTable){
    vector<string> input = tokenize(inputString, terminals, nonTerminals);
    input.push_back("$");
    vector<string> stack;
    stack.push_back("$");

    cout<<"Stack\t\tInput\t\tOutput"<<endl;
    int inputPtr = 0;
    while (true) {
        string stackTop = stack.back();
        string currentInput = input[inputPtr];
        
        string stackStr;
        for(const auto& s : stack) stackStr += s;
        
        string inputSubStr;
        for(size_t i = inputPtr; i < input.size(); ++i) inputSubStr += input[i];

        string paddedStackStr = stackStr;
        if (paddedStackStr.length() < 24) {
            paddedStackStr += string(24 - paddedStackStr.length(), ' ');
        }
        
        string paddedInputStr = inputSubStr;
        if (paddedInputStr.length() < 24) {
            paddedInputStr += string(24 - paddedInputStr.length(), ' ');
        }
        
        cout << paddedStackStr << paddedInputStr;

        if (precedenceTable.count(stackTop) == 0 || precedenceTable.at(stackTop).count(currentInput) == 0) {
            cout << "ERROR (No relation found)\n";
            break;
        }
        
        char relation = precedenceTable.at(stackTop).at(currentInput);

        if (relation == '<' || relation == '=') {
            cout << "SHIFT (" << stackTop << " " << relation << " " << currentInput << ")\n";
            stack.push_back(currentInput);
            inputPtr++;
        } else if (relation == '>') {
            cout << "REDUCE (" << stackTop << " " << relation << " " << currentInput << ")\n";
            stack.pop_back();
        } else if (relation == 'A' || (stackTop == "$" && currentInput == "$")) {
            cout << "ACCEPT\n";
            break;
        } else {
            cout << "ERROR\n";
            break;
        }
    }

    if (stack.size() == 1 && stack.back() == "$" && inputPtr >= input.size() - 1) {
        cout << "String successfully parsed.\n";
    } else {
        cout << "Error: Invalid string.\n";
    }

}

int main(){
    vector<string> terminals, nonTerminals;
    map<string, map<string, char>> precedenceTable;
    int numTerminals;
    cout << "Enter the number of terminals: ";
    cin >> numTerminals;

    cout << "Enter the terminals:\n";
    for (int i = 0; i < numTerminals; ++i) {
        string temp;
        cin >> temp;
        terminals.push_back(temp);
    }

    cout << "\nEnter the precedence relations for the table below." << endl;
    cout << "Use '<', '>', '=', or '_' (blank/error).\n" << endl;

    for (const string& rowTerm : terminals) {
        for (const string& colTerm : terminals) {
            cout << "Enter relation for (" << rowTerm << ", " << colTerm << "): ";
            char relation;
            cin >> relation;
            precedenceTable[rowTerm][colTerm] = relation;
        }
    }

    string inputString;
    cout<<"Enter the string to parse: ";
    cin>>inputString;
    parseString(inputString, terminals, nonTerminals, precedenceTable);
}