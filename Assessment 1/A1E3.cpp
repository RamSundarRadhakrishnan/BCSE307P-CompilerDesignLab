#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <algorithm>
using namespace std;
int main(){
    int n, m, f;
    cout << "Enter the number of states(n), the number of alphabets(m) and the number of final states(f)" << endl;
    cin >> n >> m >> f;
    vector<string> Q(n), E(m), F(f);
    for(int i = 0; i < n; i++){
        cout << "Enter state " << i+1 << ": ";
        cin >> Q[i];
    }
    for(int i = 0; i < m; i++){
        cout << "Enter symbol " << i+1 << ": ";
        cin >> E[i];
    }
    for(int i = 0; i < f; i++){
        cout << "Enter final state " << i+1 << ": ";
        cin >> F[i];
    }
    map<string,string> D;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < m; j++){
            string key = Q[i] + E[j];
            cout << "Enter the transition for " << key << " : ";
            cin >> D[key];
        }
    }
    cout<<"\nNFA Transition Table: "<<endl;
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            string temp = Q[i]+E[j];
            string transition = D.at(temp);
            if(transition!="-") cout<<"  "<<Q[i]<<" --"<<E[j]<<"--> "<<transition<<endl;
        }
    }
    string Valid;
    cout<<"Enter the string to validate using NFA: "<<endl;
    cin>>Valid;
    map<string, vector<string>> NFA;
    for(int i=0; i<n; i++){
        for(int j=0; j<m; j++){
            string temp = Q[i]+E[j];
            string transition = D.at(temp);
            string token;
            vector<string> rule;
            stringstream converter(transition);
            while(getline(converter, token, ',')){
                rule.push_back(token);
            }
            NFA[temp] = rule;
        }
    }
    string NFAInitial = Q[0];
    for(int i=0; i<Valid.length(); i++){
        string character = string() + Valid[i];
        auto validator = find(begin(E), end(E), character);
        if(validator==end(E)){
            cout<<"The word is not valid!"<<endl;
            break;
        }
        string iterable = NFAInitial+character;
        vector<string> transitionSet = NFA.at(iterable);
        if(transitionSet[0]=="-") continue;
        else{
            cout<<"("<<NFAInitial<<","<<character<<")"<<"-->";
            for(int l=0; l<transitionSet.size(); l++){
                if(transitionSet[l]!=NFAInitial){
                    NFAInitial = transitionSet[l];
                    break;
                }
            }
        }
    }
    map<string,int> id;
    vector<string> dq;
    queue<string> q;
    id[ Q[0] ] = 0;
    dq.push_back( Q[0] );
    q.push( Q[0] );
    while(!q.empty()){
        string S = q.front(); 
        q.pop();
        vector<string> parts;
        {
            string tmp;
            stringstream ss(S);
            while(getline(ss, tmp, ',')){
                if(tmp != "-") parts.push_back(tmp);
            }
        }
        for(const string &a : E){
            set<string> U;
            for(const string &p : parts){
                string dest = D[p + a];
                string tmp2;
                stringstream ss2(dest);
                while(getline(ss2, tmp2, ',')){
                    if(tmp2 != "-") U.insert(tmp2);
                }
            }
            string T;
            for(auto it = U.begin(); it != U.end(); ++it){
                if(it != U.begin()) T += ",";
                T += *it;
            }
            if(T.empty()) T = "-";
            if(!id.count(T)){
                id[T] = dq.size();
                dq.push_back(T);
                q.push(T);
            }
        }
    }
    cout << "\nDFA States (Q'):\n";
    for(const auto &s : dq){
        cout << "  " << s << "\n";
    }
    cout << "\nDFA Transitions:\n";
    for(const auto &S : dq){
        vector<string> parts;
        {
            string tmp;
            stringstream ss(S);
            while(getline(ss, tmp, ',')){
                if(tmp != "-") parts.push_back(tmp);
            }
        }
        for(const auto &a : E){
            set<string> U;
            for(const auto &p : parts){
                string dest = D[p + a];
                string tmp2;
                stringstream ss2(dest);
                while(getline(ss2, tmp2, ',')){
                    if(tmp2 != "-") U.insert(tmp2);
                }
            }
            string T;
            for(auto it = U.begin(); it != U.end(); ++it){
                if(it != U.begin()) T += ",";
                T += *it;
            }
            if(T.empty()) T = "-";
            cout << "  " << S << " --" << a << "--> " << T << "\n";
        }
    }
    return 0;
}
