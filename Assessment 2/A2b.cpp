#include <iostream>
#include <fstream>
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
    ifstream file("inputCode.txt");
    string code;
    vector<string> linesOfCode;
    while(getline(file, code)){
        linesOfCode.push_back(code);
    }
    vector<string> identifiers;
    vector<string> arithmeticOperators;
    vector<string> relationalOperators;
    vector<string> numbers;
    vector<string> delimiters;
    vector<string> keywords;
    bool assignmentOperator;
    for(int i=0; i<linesOfCode.size(); i++){
        string inputLine = linesOfCode[i];
        stringstream stream(inputLine);
        vector<string> splitInputLine;
        string tok;
        while(getline(stream, tok, ' ')){
            splitInputLine.push_back(tok);
        }
        for(int i=0; i<splitInputLine.size(); i++){
            string current = splitInputLine[i];
            if(current=="int"||current=="float"||current=="long"||current=="bool"||current=="for"||current=="while"||current=="if"||current=="do"||current=="switch"||current=="true"||current=="false"||current=="return"||current=="main"){
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
                if(assignmentOperator) continue;
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
    }
    ofstream output("outputLexed.txt");
    if(keywords.size()!=0){
        cout<<"Keywords:"<<endl;
        output<<"Keywords:"<<endl;
        for(int i=0; i<keywords.size(); i++){
            cout<<keywords[i]<<" ";
            output<<keywords[i]<<" ";
        }
        cout<<endl;
        output<<endl;
    }
    if(assignmentOperator){
        cout<<"Assignment Operator:\n="<<endl;
        output<<"Assignment Operator:\n="<<endl;
    }
    if(arithmeticOperators.size()!=0){
        cout<<"Arithmetic Operators:"<<endl;
        output<<"Arithmetic Operators:"<<endl;
        for(int i=0; i<arithmeticOperators.size(); i++){
            cout<<arithmeticOperators[i]<<" ";
            output<<arithmeticOperators[i]<<" ";
        }
        cout<<endl;
        output<<endl;
    }
    if(relationalOperators.size()!=0){
        cout<<"Relational Operators:"<<endl;
        output<<"Relational Operators:"<<endl;
        for(int i=0; i<relationalOperators.size(); i++){
            cout<<relationalOperators[i]<<" ";
            output<<relationalOperators[i]<<" ";
        }
        cout<<endl;
        output<<endl;
    }
    if(identifiers.size()!=0){
        cout<<"Identifiers:"<<endl;
        output<<"Identifiers:"<<endl;
        for(int i=0; i<identifiers.size(); i++){
            cout<<identifiers[i]<<" ";
            output<<identifiers[i]<<" ";
        }
        cout<<endl;
        output<<endl;
        file.close();
        output.close();
    }
}