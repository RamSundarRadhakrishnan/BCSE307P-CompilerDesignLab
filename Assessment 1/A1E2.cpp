#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

int main(){
    	int m,n,f;
	cout<<"Enter the number of states(n), the number of alphabets(m) and the number of final states(f)"<<endl;
	cin>>n;
	cin>>m;
	cin>>f;
	vector<string> Q;
	vector<string> E;
	vector<string> F;
	for(int i=0; i<n; i++){
		cout<<"Enter the state: ";
		string tempS;
		cin>>tempS;
		Q.push_back(tempS);
	}
	for(int i=0; i<m; i++){
		cout<<"Enter the symbol: ";
		string temp;
		cin>>temp;
		E.push_back(temp);
	}
	for(int i=0; i<f; i++){
		cout<<"Enter the final state: ";
		string temp;
		cin>>temp;
		F.push_back(temp);
	}
	map<string, string> D;
	for(int i=0; i<n; i++){
		for(int j=0; j<m; j++){
			string t2 = Q[i] + E[j];
			cout<<"Enter the transition for "<<t2<<" : ";
			string t1;
			cin>>t1;
			D.insert({t2,t1});
		}
	}
    vector<vector<string>> grammar;
    for(int i=0; i<n; i++){
        vector<string> rule;
        string state = Q[i];
        for(int j=0; j<m; j++){
            string character = E[i];
        }
    }
}