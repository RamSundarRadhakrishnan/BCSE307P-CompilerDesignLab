#include <bits/stdc++.h>
using namespace std;

static void rstrip(string& s){ while(!s.empty() && (s.back()=='\n'||s.back()=='\r'||isspace((unsigned char)s.back()))) s.pop_back(); }
static void lstrip(string& s){ size_t i=0; while(i<s.size() && isspace((unsigned char)s[i])) i++; if(i) s.erase(0,i); }
static void trim(string& s){ rstrip(s); lstrip(s); }

static bool isnum(const string& s){
  if(s.empty()) return false;
  size_t i=0; if(s[0]=='+'||s[0]=='-') i=1;
  if(i>=s.size()) return false;
  for(; i<s.size(); i++) if(!isdigit((unsigned char)s[i])) return false;
  return true;
}

int main(){
  vector<string> lines;
  string line;
  while(getline(cin,line)) lines.push_back(line);

  vector<string> syms;
  unordered_set<string> seen;
  auto addsym = [&](const string& name){
    if(name.empty()) return;
    if(isnum(name)) return;
    if(name=="goto"||name=="if") return;
    if(seen.insert(name).second) syms.push_back(name);
  };

  auto parse_label = [&](const string& s, string& lbl)->bool{
    size_t p=s.find(':');
    if(p==string::npos) return false;
    string left=s.substr(0,p);
    string right=s.substr(p+1);
    if(right.find(':')!=string::npos) return false;
    string t=left; trim(t);
    if(t.empty()) return false;
    lbl=t;
    return true;
  };

  for(string s: lines){
    trim(s);
    if(s.empty()) continue;
    string lbl;
    if (parse_label(s,lbl)) { 
      // Label line: do NOT add to data symbols
    continue; 
    }
    if(s.rfind("goto ",0)==0){
      continue;
    }else if(s.rfind("if ",0)==0){
      char a[128],rop[32],b[128],L[128];
      if(sscanf(s.c_str(),"if %127s %31s %127s goto %127s",a,rop,b,L)==4){ addsym(a); addsym(b); continue; }
    }else{
      char lhs[128],rhs1[128],op[32],rhs2[128];
      if(sscanf(s.c_str(),"%127s = %127s %31s %127s",lhs,rhs1,op,rhs2)==4){
        if(string(op)=="<<"){ addsym(lhs); addsym(rhs1); continue; }
        addsym(lhs); addsym(rhs1); addsym(rhs2); continue;
      }
      char shlhs[128],sha[128],shb[128];
      if(sscanf(s.c_str(),"%127s = %127s << %127s",shlhs,sha,shb)==3){
        addsym(shlhs); addsym(sha); continue;
      }
      char movlhs[128],mova[128];
      if(sscanf(s.c_str(),"%127s = %127s",movlhs,mova)==2){
        addsym(movlhs); addsym(mova); continue;
      }
    }
  }

  cout<<"assume cs:code, ds:data\n";
  cout<<"data segment\n";
  for(auto& x: syms) cout<<x<<" dw ?\n";
  cout<<"data ends\n";
  cout<<"code segment\nstart:\nmov ax,data\nmov ds,ax\n";

  auto mov_ax = [&](const string& src){
    if(isnum(src)) cout<<"mov ax,"<<src<<"\n";
    else cout<<"mov ax,"<<src<<"\n";
  };
  auto load_bx = [&](const string& src){
    if(isnum(src)) cout<<"mov bx,"<<src<<"\n";
    else cout<<"mov bx,"<<src<<"\n";
  };
  auto binop = [&](const string& op,const string& a,const string& b){
    mov_ax(a);
    load_bx(b);
    if(op=="+") cout<<"add ax,bx\n";
    else if(op=="-") cout<<"sub ax,bx\n";
    else if(op=="*") cout<<"imul bx\n";
    else if(op=="/") { cout<<"cwd\n"; cout<<"idiv bx\n"; }
  };
  auto cmp_ab = [&](const string& a,const string& b){
    mov_ax(a);
    load_bx(b);
    cout<<"cmp ax,bx\n";
  };
  auto jcc = [&](const string& rop)->const char*{
    if(rop=="<") return "jl";
    if(rop=="<=") return "jle";
    if(rop==">") return "jg";
    if(rop==">=") return "jge";
    if(rop=="==") return "je";
    if(rop=="!=") return "jne";
    return "jne";
  };

  for(string s: lines){
    trim(s);
    if(s.empty()) continue;

    string lbl;
    if(parse_label(s,lbl)){ cout<<lbl<<":\n"; continue; }

    if(s.rfind("goto ",0)==0){
      char L[128];
      if(sscanf(s.c_str(),"goto %127s",L)==1){ cout<<"jmp "<<L<<"\n"; continue; }
    }

    if(s.rfind("if ",0)==0){
      char a[128],rop[32],b[128],L[128];
      if(sscanf(s.c_str(),"if %127s %31s %127s goto %127s",a,rop,b,L)==4){
        cmp_ab(a,b);
        cout<<jcc(rop)<<" "<<L<<"\n";
        continue;
      }
    }

    char lhs[128],rhs1[128],op[32],rhs2[128];
    if(sscanf(s.c_str(),"%127s = %127s %31s %127s",lhs,rhs1,op,rhs2)==4){
      string sop(op);
      if(sop=="<<"){
        mov_ax(rhs1);
        if(isnum(rhs2)) cout<<"mov cl,"<<rhs2<<"\n"; else cout<<"mov cl,0\n";
        cout<<"shl ax,cl\nmov "<<lhs<<",ax\n";
      }else{
        binop(sop,rhs1,rhs2);
        cout<<"mov "<<lhs<<",ax\n";
      }
      continue;
    }

    char shlhs[128],sha[128],shb[128];
    if(sscanf(s.c_str(),"%127s = %127s << %127s",shlhs,sha,shb)==3){
      mov_ax(sha);
      if(isnum(shb)) cout<<"mov cl,"<<shb<<"\n"; else cout<<"mov cl,0\n";
      cout<<"shl ax,cl\nmov "<<shlhs<<",ax\n";
      continue;
    }

    char movlhs[128],mova[128];
    if(sscanf(s.c_str(),"%127s = %127s",movlhs,mova)==2){
      mov_ax(mova);
      cout<<"mov "<<movlhs<<",ax\n";
      continue;
    }
  }

  cout<<"mov ah,4ch\nint 21h\ncode ends\nend start\n";
  return 0;
}