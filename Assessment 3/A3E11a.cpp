#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <utility>

using namespace std;

vector<string> all_lhs;
vector<vector<string>> all_rhs;
set<string> non_terminals;
set<string> terminals;
map<string, set<string>> first_sets;

static string trim(const string& s) {
    int a = s.find_first_not_of(" \t\n\r");
    if (string::npos == a) return "";
    int b = s.find_last_not_of(" \t\n\r");
    return s.substr(a, b - a + 1);
}

set<string> get_first_of_sequence(const vector<string>& seq) {
    set<string> result;
    if (seq.empty()) {
        result.insert("epsilon");
        return result;
    }

    for (const auto& sym : seq) {
        bool has_epsilon = false;
        if (!non_terminals.count(sym)) {
            result.insert(sym);
            return result;
        }
        if (first_sets.count(sym)) {
            for (const auto& f : first_sets.at(sym)) {
                if (f == "epsilon") {
                    has_epsilon = true;
                } else {
                    result.insert(f);
                }
            }
        }
        if (!has_epsilon) {
            return result;
        }
    }
    result.insert("epsilon");
    return result;
}

set<tuple<int, int, string>> closure(set<tuple<int, int, string>> I) {
    vector<tuple<int, int, string>> worklist(I.begin(), I.end());
    int head = 0;
    while(head < worklist.size()){
        tuple<int, int, string> item = worklist[head++];
        int prod_idx = get<0>(item);
        int dot_pos = get<1>(item);
        string lookahead = get<2>(item);

        if (dot_pos >= all_rhs[prod_idx].size()) continue;

        const string& B = all_rhs[prod_idx][dot_pos];
        if (!non_terminals.count(B)) continue;

        vector<string> beta;
        for (int i = dot_pos + 1; i < all_rhs[prod_idx].size(); ++i) {
            beta.push_back(all_rhs[prod_idx][i]);
        }
        beta.push_back(lookahead);

        set<string> lookaheads = get_first_of_sequence(beta);

        for (int p_idx = 0; p_idx < all_lhs.size(); ++p_idx) {
            if (all_lhs[p_idx] == B) {
                for (const auto& la : lookaheads) {
                    if (la != "epsilon") {
                        tuple<int, int, string> new_item = { (int)p_idx, 0, la };
                        if (I.insert(new_item).second) {
                            worklist.push_back(new_item);
                        }
                    }
                }
            }
        }
    }
    return I;
}

set<tuple<int, int, string>> goTo(const set<tuple<int, int, string>>& I, const string& X) {
    set<tuple<int, int, string>> J;
    for (const auto& item : I) {
        int prod_idx = get<0>(item);
        int dot_pos = get<1>(item);
        if (dot_pos < all_rhs[prod_idx].size() && all_rhs[prod_idx][dot_pos] == X) {
            J.insert({prod_idx, dot_pos + 1, get<2>(item)});
        }
    }
    return closure(J);
}

int main() {
    int nt_count, t_count, p_count;
    cout << "Enter the number of non terminals:"<<endl;
    cin >> nt_count;
    vector<string> nt_ordered(nt_count);
    for (int i = 0; i < nt_count; ++i) {
        cout << "Enter the non terminal: "; 
        cin >> nt_ordered[i]; 
        non_terminals.insert(nt_ordered[i]); 
    }

    cout << "Enter the number of terminals:"<<endl;
    cin >> t_count;
    for (int i = 0; i < t_count; ++i) {
        string t;
        cout << "Enter the terminal: "; 
        cin >> t; 
        terminals.insert(t); 
    }

    cout << "Enter the number of productions:"<<endl;
    cin >> p_count;
    string line;
    getline(cin, line); 

    vector<string> temp_lhs;
    vector<string> temp_rhs_str;
    for (int i = 0; i < p_count; ++i) {
        cout << "Enter the production: ";
        getline(cin, line);
        string lhs = trim(line.substr(0, line.find("->")));
        string rhs_str = trim(line.substr(line.find("->") + 2));
        temp_lhs.push_back(lhs);
        temp_rhs_str.push_back(rhs_str);
    }
    
    string start_symbol = nt_ordered[0];
    string augmented_start = start_symbol + "'";
    all_lhs.push_back(augmented_start);
    all_rhs.push_back({start_symbol});

    vector<string> all_symbols_vec;
    all_symbols_vec.insert(all_symbols_vec.end(), non_terminals.begin(), non_terminals.end());
    all_symbols_vec.insert(all_symbols_vec.end(), terminals.begin(), terminals.end());
    sort(all_symbols_vec.begin(), all_symbols_vec.end(), [](const string& a, const string& b) {
        return a.length() > b.length();
    });

    for (int i = 0; i < temp_lhs.size(); ++i) {
        stringstream ss(temp_rhs_str[i]);
        string part;
        while (getline(ss, part, '|')) {
            all_lhs.push_back(temp_lhs[i]);
            vector<string> tokens;
            stringstream pss(trim(part));
            string tok;
            while(pss >> tok) tokens.push_back(tok);
            all_rhs.push_back(tokens.empty() || tokens[0] == "epsilon" ? vector<string>{} : tokens);
        }
    }

    bool changed = true;
    while(changed){
        changed = false;
        for(const auto& nt : non_terminals){
            for(int i = 0; i < all_lhs.size(); ++i){
                if(all_lhs[i] == nt){
                    int before_size = first_sets[nt].size();
                    set<string> rhs_first = get_first_of_sequence(all_rhs[i]);
                    for(const auto& s : rhs_first){
                        first_sets[nt].insert(s);
                    }
                    if(first_sets[nt].size() != before_size) changed = true;
                }
            }
        }
    }

    vector<set<tuple<int, int, string>>> clr_collection;
    map<set<tuple<int, int, string>>, int> clr_state_map;
    clr_collection.push_back(closure({{0, 0, "$"}}));
    clr_state_map[clr_collection.back()] = 0;
    for (int i = 0; i < clr_collection.size(); ++i) {
        for (const auto& X : all_symbols_vec) {
            set<tuple<int, int, string>> next_state = goTo(clr_collection[i], X);
            if (!next_state.empty() && clr_state_map.find(next_state) == clr_state_map.end()) {
                clr_state_map[next_state] = clr_collection.size();
                clr_collection.push_back(next_state);
            }
        }
    }

    map<set<pair<int, int>>, int> core_to_lalr_state;
    vector<set<tuple<int, int, string>>> lalr_collection;
    vector<int> clr_to_lalr_map(clr_collection.size());

    for(int i = 0; i < clr_collection.size(); ++i){
        set<pair<int, int>> core;
        for(const auto& item : clr_collection[i]){
            core.insert({get<0>(item), get<1>(item)});
        }
        if(core_to_lalr_state.find(core) == core_to_lalr_state.end()){
            core_to_lalr_state[core] = lalr_collection.size();
            lalr_collection.push_back(clr_collection[i]);
            clr_to_lalr_map[i] = lalr_collection.size() - 1;
        } else {
            int lalr_idx = core_to_lalr_state[core];
            lalr_collection[lalr_idx].insert(clr_collection[i].begin(), clr_collection[i].end());
            clr_to_lalr_map[i] = lalr_idx;
        }
    }

    vector<map<string, string>> action_table(lalr_collection.size());
    vector<map<string, int>> goto_table(lalr_collection.size());

    for(int i=0; i < clr_collection.size(); ++i){
        int lalr_i = clr_to_lalr_map[i];
        for(const auto& X : all_symbols_vec){
            set<tuple<int, int, string>> next_clr_state = goTo(clr_collection[i], X);
            if(!next_clr_state.empty()){
                int clr_j = clr_state_map[next_clr_state];
                int lalr_j = clr_to_lalr_map[clr_j];
                if(terminals.count(X)) action_table[lalr_i][X] = "s" + to_string(lalr_j);
                else if(non_terminals.count(X)) goto_table[lalr_i][X] = lalr_j;
            }
        }
    }
    
    for(int i = 0; i < lalr_collection.size(); ++i){
        for(const auto& item : lalr_collection[i]){
            if(get<1>(item) == all_rhs[get<0>(item)].size()){
                if(get<0>(item) == 0) action_table[i]["$"] = "acc";
                else action_table[i][get<2>(item)] = "r" + to_string(get<0>(item));
            }
        }
    }

    cout << "#PRODUCTIONS\n";
    for (int i = 1; i < all_lhs.size(); ++i) {
        cout << i << "," << all_lhs[i] << ",";
        if (all_rhs[i].empty()) cout << "epsilon";
        else for (int k = 0; k < all_rhs[i].size(); ++k) cout << (k > 0 ? " " : "") << all_rhs[i][k];
        cout << "\n";
    }

    vector<string> sorted_terminals(terminals.begin(), terminals.end());
    sorted_terminals.push_back("$");
    sort(sorted_terminals.begin(), sorted_terminals.end());

    cout << "#ACTION\nstate";
    for (const auto& t : sorted_terminals) cout << "," << t;
    cout << "\n";
    for (int i = 0; i < action_table.size(); ++i) {
        cout << i;
        for (const auto& t : sorted_terminals) cout << "," << (action_table[i].count(t) ? action_table[i][t] : "");
        cout << "\n";
    }

    sort(nt_ordered.begin(), nt_ordered.end());
    cout << "#GOTO\nstate";
    for (const auto& nt : nt_ordered) cout << "," << nt;
    cout << "\n";
    for (int i = 0; i < goto_table.size(); ++i) {
        cout << i;
        for (const auto& nt : nt_ordered) {
            cout << ",";
            if(goto_table[i].count(nt)) cout << goto_table[i][nt];
        }
        cout << "\n";
    }

    return 0;
}