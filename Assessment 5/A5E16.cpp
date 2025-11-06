#include <bits/stdc++.h>
using namespace std;

static void rstrip(string& s){ while(!s.empty() && (s.back()=='\n'||s.back()=='\r'||isspace((unsigned char)s.back())||s.back()==';')) s.pop_back(); }
static void lstrip(string& s){ size_t i=0; while(i<s.size() && isspace((unsigned char)s[i])) i++; if(i) s.erase(0,i); }
static void trim(string& s){ rstrip(s); lstrip(s); }
static bool isnum(const string& s){ if(s.empty()) return false; size_t i=0; if(s[0]=='+'||s[0]=='-') i=1; if(i>=s.size()) return false; for(; i<s.size(); ++i) if(!isdigit((unsigned char)s[i])) return false; return true; }
static long tolong(const string& s){ return strtol(s.c_str(),nullptr,10); }
static bool pow2(long x){ if(x<=0) return false; return (x & (x-1))==0; }
static long log2i(long x){ long k=0; while(x>1){ x>>=1; ++k; } return k; }

int main(){
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  string line;
  while(getline(cin,line)){
    trim(line);
    if(line.empty()) continue;
    if(line=="{" || line=="}") continue;

    if(line.find(':')!=string::npos){
      size_t p=line.find(':');
      string left=line.substr(0,p), right=line.substr(p+1);
      string t=left; trim(t);
      if(!t.empty() && right.find(':')==string::npos){ cout<<t<<":\n"; continue; }
    }

    if(line.rfind("goto ",0)==0){ cout<<line<<"\n"; continue; }
    if(line.rfind("if ",0)==0){ cout<<line<<"\n"; continue; }

    {
      char lhs[256], a[256], op[32], b[256];
      if(sscanf(line.c_str(),"%255s = %255s %31s %255s",lhs,a,op,b)==4){
        string L=lhs, A=a, OP=op, B=b;

        if(isnum(A) && isnum(B)){
          long x=tolong(A), y=tolong(B), r=0;
          if(OP=="+") r=x+y;
          else if(OP=="-") r=x-y;
          else if(OP=="*") r=x*y;
          else if(OP=="/") r=(y? x/y : 0);
          cout<<L<<" = "<<r<<"\n";
          continue;
        }

        if(OP=="*"){
          if(isnum(A) && pow2(tolong(A))){ cout<<L<<" = "<<B<<" << "<<log2i(tolong(A))<<"\n"; continue; }
          if(isnum(B) && pow2(tolong(B))){ cout<<L<<" = "<<A<<" << "<<log2i(tolong(B))<<"\n"; continue; }
        }

        if(OP=="+"){
          if(isnum(B) && tolong(B)==0){ cout<<L<<" = "<<A<<"\n"; continue; }
          if(isnum(A) && tolong(A)==0){ cout<<L<<" = "<<B<<"\n"; continue; }
        }
        if(OP=="-"){
          if(isnum(B) && tolong(B)==0){ cout<<L<<" = "<<A<<"\n"; continue; }
        }
        if(OP=="*"){
          if((isnum(A)&&tolong(A)==1)){ cout<<L<<" = "<<B<<"\n"; continue; }
          if((isnum(B)&&tolong(B)==1)){ cout<<L<<" = "<<A<<"\n"; continue; }
          if((isnum(A)&&tolong(A)==0)||(isnum(B)&&tolong(B)==0)){ cout<<L<<" = 0\n"; continue; }
        }

        cout<<L<<" = "<<A<<" "<<OP<<" "<<B<<"\n";
        continue;
      }
    }

    {
      char lhs[256], a[256], sh[8], k[256];
      if(sscanf(line.c_str(),"%255s = %255s %7s %255s",lhs,a,sh,k)==4 && string(sh)=="<<"){
        cout<<line<<"\n";
        continue;
      }
    }

    {
      char lhs[256], a[256];
      if(sscanf(line.c_str(),"%255s = %255s",lhs,a)==2){
        string L=lhs, A=a;
        if(L!=A) cout<<L<<" = "<<A<<"\n";
        continue;
      }
    }

    cout<<line<<"\n";
  }
  return 0;
}