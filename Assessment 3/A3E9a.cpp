#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>

using namespace std;

static vector<string> tokenize(const string& str, const vector<string>& all_symbols) {
    vector<string> tokens;
    int i = 0;
    while (i < str.length()) {
        if (isspace(str[i])) {
            i++;
            continue;
        }
        bool matched = false;
        for (const auto& sym : all_symbols) {
            if (str.substr(i, sym.length()) == sym) {
                tokens.push_back(sym);
                i += sym.length();
                matched = true;
                break;
            }
        }
        if (!matched) {
            tokens.push_back(string(1, str[i]));
            i++;
        }
    }
    return tokens;
}

static set<string> first_of_sequence(const vector<string>& seq, const map<string, set<string>>& first_sets, const set<string>& non_terminals) {
    set<string> result;
    bool all_derive_epsilon = true;
    for (const auto& sym : seq) {
        bool sym_has_epsilon = false;
        if (non_terminals.count(sym)) {
            const auto& sym_first = first_sets.at(sym);
            for (const auto& s : sym_first) {
                if (s != "epsilon") {
                    result.insert(s);
                } else {
                    sym_has_epsilon = true;
                }
            }
        } else {
            result.insert(sym);
        }
        if (!sym_has_epsilon) {
            all_derive_epsilon = false;
            break;
        }
    }
    if (all_derive_epsilon) {
        result.insert("epsilon");
    }
    return result;
}

static void compute_first_sets_recursive(const string& symbol, map<string, set<string>>& first_sets, const map<string, vector<int>>& prods_by_lhs, const vector<vector<string>>& all_rhs, const set<string>& non_terminals) {
    if (first_sets.count(symbol)) return;

    first_sets[symbol] = {};
    for (int p_idx : prods_by_lhs.at(symbol)) {
        const auto& rhs = all_rhs.at(p_idx);
        bool all_derive_epsilon = true;

        for (const auto& sym : rhs) {
            if (sym == "epsilon") break;
            if (!non_terminals.count(sym)) {
                first_sets[symbol].insert(sym);
                all_derive_epsilon = false;
                break;
            }
            compute_first_sets_recursive(sym, first_sets, prods_by_lhs, all_rhs, non_terminals);
            bool sym_has_epsilon = false;
            for (const auto& f : first_sets.at(sym)) {
                if (f != "epsilon") {
                    first_sets[symbol].insert(f);
                } else {
                    sym_has_epsilon = true;
                }
            }
            if (!sym_has_epsilon) {
                all_derive_epsilon = false;
                break;
            }
        }
        if (all_derive_epsilon) {
            first_sets[symbol].insert("epsilon");
        }
    }
}

static string trim(const string& s) {
    int a = s.find_first_not_of(" \t\n\r");
    if (string::npos == a) return "";
    int b = s.find_last_not_of(" \t\n\r");
    return s.substr(a, b - a + 1);
}

int main() {
    int nNT_count;
    cout << "Enter the number of non terminals:"<<endl;
    cin >> nNT_count;
    set<string> non_terminals;
    vector<string> non_terminals_ordered(nNT_count);
    for (int i = 0; i < nNT_count; ++i) {
        cout << "Enter the non terminal: ";
        cin >> non_terminals_ordered[i];
        non_terminals.insert(non_terminals_ordered[i]);
    }

    int T_count;
    cout << "Enter the number of terminals:"<<endl;
    cin >> T_count;
    set<string> terminals;
    for (int i = 0; i < T_count; ++i) {
        string t;
        cout << "Enter the terminal: ";
        cin >> t;
        terminals.insert(t);
    }

    int P_count;
    cout << "Enter the number of productions:"<<endl;
    cin >> P_count;
    string line;
    getline(cin, line);

    vector<string> all_lhs;
    vector<vector<string>> all_rhs;
    string start_symbol = non_terminals_ordered.front();

    string augmented_start_symbol = start_symbol + "'";
    while (non_terminals.count(augmented_start_symbol)) {
        augmented_start_symbol += "'";
    }
    all_lhs.push_back(augmented_start_symbol);
    all_rhs.push_back({start_symbol});

    vector<string> all_symbols_vec;
    all_symbols_vec.insert(all_symbols_vec.end(), non_terminals.begin(), non_terminals.end());
    all_symbols_vec.insert(all_symbols_vec.end(), terminals.begin(), terminals.end());
    sort(all_symbols_vec.begin(), all_symbols_vec.end(), [](const string& a, const string& b) {
        return a.length() > b.length();
    });

    for (int i = 0; i < P_count; ++i) {
        cout << "Enter the production: ";
        getline(cin, line);
        string lhs = trim(line.substr(0, line.find("->")));
        string rhs_str = trim(line.substr(line.find("->") + 2));
        stringstream ss(rhs_str);
        string part;
        while (getline(ss, part, '|')) {
            all_lhs.push_back(lhs);
            auto tokens = tokenize(trim(part), all_symbols_vec);
            if (tokens.empty() || tokens[0] == "epsilon") {
                all_rhs.push_back({});
            } else {
                all_rhs.push_back(tokens);
            }
        }
    }

    map<string, vector<int>> prods_by_lhs;
    for (int i = 0; i < all_lhs.size(); ++i) {
        prods_by_lhs[all_lhs[i]].push_back(i);
    }

    map<string, set<string>> first_sets;
    for (const auto& nt : non_terminals_ordered) {
        compute_first_sets_recursive(nt, first_sets, prods_by_lhs, all_rhs, non_terminals);
    }

    map<string, set<string>> follow_sets;
    for (const auto& nt : non_terminals) follow_sets[nt] = {};
    follow_sets[start_symbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < all_lhs.size(); ++i) {
            const string& A = all_lhs[i];
            const auto& rhs = all_rhs[i];
            for (int j = 0; j < rhs.size(); ++j) {
                const string& B = rhs[j];
                if (!non_terminals.count(B)) continue;

                vector<string> beta(rhs.begin() + j + 1, rhs.end());
                set<string> first_of_beta = first_of_sequence(beta, first_sets, non_terminals);
                
                int before_size = follow_sets[B].size();
                for (const auto& s : first_of_beta) {
                    if (s != "epsilon") follow_sets[B].insert(s);
                }
                if (follow_sets[B].size() != before_size) changed = true;

                if (first_of_beta.count("epsilon") || beta.empty()) {
                    before_size = follow_sets[B].size();
                    follow_sets[B].insert(follow_sets[A].begin(), follow_sets[A].end());
                    if (follow_sets[B].size() != before_size) changed = true;
                }
            }
        }
    }
    
    using LrItem = pair<int, int>;
    using LrItemSet = set<LrItem>;

    auto closure = [&](LrItemSet I) {
        vector<LrItem> worklist(I.begin(), I.end());
        int head = 0;
        while(head < worklist.size()){
            LrItem item = worklist[head++];
            if (item.second < all_rhs[item.first].size()) {
                const string& B = all_rhs[item.first][item.second];
                if (non_terminals.count(B)) {
                    for (int p_idx : prods_by_lhs[B]) {
                        if (I.insert({p_idx, 0}).second) {
                            worklist.push_back({p_idx, 0});
                        }
                    }
                }
            }
        }
        return I;
    };

    auto goTo = [&](const LrItemSet& I, const string& X) {
        LrItemSet J;
        for (const auto& item : I) {
            if (item.second < all_rhs[item.first].size() && all_rhs[item.first][item.second] == X) {
                J.insert({item.first, item.second + 1});
            }
        }
        return closure(J);
    };
    
    vector<LrItemSet> canonical_collection;
    map<LrItemSet, int> state_map;
    
    canonical_collection.push_back(closure({{0, 0}}));
    state_map[canonical_collection.back()] = 0;

    vector<string> all_grammar_symbols;
    all_grammar_symbols.insert(all_grammar_symbols.end(), terminals.begin(), terminals.end());
    all_grammar_symbols.insert(all_grammar_symbols.end(), non_terminals.begin(), non_terminals.end());

    vector<map<string, int>> transitions(1);
    for (int i = 0; i < canonical_collection.size(); ++i) {
        for (const auto& X : all_grammar_symbols) {
            LrItemSet next_state = goTo(canonical_collection[i], X);
            if (!next_state.empty()) {
                if (state_map.find(next_state) == state_map.end()) {
                    state_map[next_state] = canonical_collection.size();
                    canonical_collection.push_back(next_state);
                    transitions.emplace_back();
                }
                transitions[i][X] = state_map[next_state];
            }
        }
    }

    vector<map<string, string>> action_table(canonical_collection.size());
    vector<map<string, int>> goto_table(canonical_collection.size());
    
    for (int i = 0; i < canonical_collection.size(); ++i) {
        for (const auto& item : canonical_collection[i]) {
            if (item.second == all_rhs[item.first].size()) {
                if (item.first == 0) {
                    action_table[i]["$"] = "acc";
                } else {
                    for (const auto& term : follow_sets[all_lhs[item.first]]) {
                         action_table[i][term] = "r" + to_string(item.first);
                    }
                }
            }
        }
        if (transitions[i].empty()) continue;
        for(const auto& trans : transitions[i]){
            if (terminals.count(trans.first) || trans.first == "$") {
                action_table[i][trans.first] = "s" + to_string(trans.second);
            } else if (non_terminals.count(trans.first)) {
                goto_table[i][trans.first] = trans.second;
            }
        }
    }

    cout << "#PRODUCTIONS\n";
    for (int i = 1; i < all_lhs.size(); ++i) {
        cout << i << "," << all_lhs[i] << ",";
        if (all_rhs[i].empty()) {
            cout << "epsilon";
        } else {
            for (int k = 0; k < all_rhs[i].size(); ++k) {
                cout << (k > 0 ? " " : "") << all_rhs[i][k];
            }
        }
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
        for (const auto& t : sorted_terminals) {
            cout << "," << (action_table[i].count(t) ? action_table[i][t] : "");
        }
        cout << "\n";
    }

    sort(non_terminals_ordered.begin(), non_terminals_ordered.end());
    cout << "#GOTO\nstate";
    for (const auto& nt : non_terminals_ordered) cout << "," << nt;
    cout << "\n";
    for (int i = 0; i < goto_table.size(); ++i) {
        cout << i;
        for (const auto& nt : non_terminals_ordered) {
            cout << ",";
            if(goto_table[i].count(nt)) cout << goto_table[i][nt];
        }
        cout << "\n";
    }

    return 0;
}