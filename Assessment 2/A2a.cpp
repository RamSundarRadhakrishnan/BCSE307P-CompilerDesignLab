#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;

bool isNumeric(const string &s){
    if(s.empty()) return false;
    for(char c:s){
        if(!isdigit(c)) return false;
    }
    return true;
}

int main(){
    cout<<"Enter the input line for token identification: ";
    string inputLine;
    getline(cin, inputLine);
    stringstream stream(inputLine);
    vector<string> splitInputLine;
    vector<string> identifiers;
    vector<string> keywords;
    vector<string> arithmeticOperators;
    bool assignmentOperator = false;
    vector<string> relationalOperators;
    vector<string> numbers;
    vector<string> delimiters;
    string tok;
    while(getline(stream, tok, ' ')){
        splitInputLine.push_back(tok);
    }
    for(int i=0; i<splitInputLine.size(); i++){
        string current = splitInputLine[i];
        if(current=="int"||current=="float"||current=="long"||current=="bool"||current=="for"||current=="while"||current=="if"||current=="do"||current=="switch"||current=="true"||current=="false"||current=="return"){
            auto it = find(keywords.begin(), keywords.end(), current);
            if(it==keywords.end()) keywords.push_back(current);
            continue;
        }
        if(isNumeric(current)){
            auto it = find(numbers.begin(), numbers.end(), current);
            if(it==numbers.end()) numbers.push_back(current);
            continue;
        }
        if(current=="="){
            assignmentOperator=true;
            continue;
        }
        if(current=="+"||current=="-"||current=="*"||current=="/"||current=="%"||current=="++"||current=="--"){
            auto it = find(arithmeticOperators.begin(), arithmeticOperators.end(), current);
            if(it==arithmeticOperators.end()) arithmeticOperators.push_back(current);
            continue;
        }
        if(current==">"||current=="<"||current==">="||current=="<="||current=="&&"||current=="||"){
            auto it = find(relationalOperators.begin(), relationalOperators.end(), current);
            if(it==relationalOperators.end()) relationalOperators.push_back(current);
            continue;
        }
        if(current=="{"||current=="}"||current=="("||current==")"||current==";"){
            auto it = find(delimiters.begin(), delimiters.end(), current);
            if(it==delimiters.end()) delimiters.push_back(current);
            continue;
        }
        if(current.length()!=0){
            auto it = find(identifiers.begin(), identifiers.end(), current);
            if(it==identifiers.end()) identifiers.push_back(current);
            continue;
        }
    }
    if(keywords.size()!=0){
        cout<<"Keywords:"<<endl;
        for(int i=0; i<keywords.size(); i++){
            cout<<keywords[i]<<" ";
        }
        cout<<endl;
    }
    if(assignmentOperator) cout<<"Assignment Operator:\n="<<endl;
    if(arithmeticOperators.size()!=0){
        cout<<"Arithmetic Operators:"<<endl;
        for(int i=0; i<arithmeticOperators.size(); i++){
            cout<<arithmeticOperators[i]<<" ";
        }
        cout<<endl;
    }
    if(relationalOperators.size()!=0){
        cout<<"Relational Operators:"<<endl;
        for(int i=0; i<relationalOperators.size(); i++){
            cout<<relationalOperators[i]<<" ";
        }
        cout<<endl;
    }
    if(identifiers.size()!=0){
        cout<<"Identifiers:"<<endl;
        for(int i=0; i<identifiers.size(); i++){
            cout<<identifiers[i]<<" ";
        }
        cout<<endl;
    }
}