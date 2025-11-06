#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

vector<string> productions_lhs;
vector<int> productions_rhs_size;
vector<map<string, string>> action_table;
vector<map<string, int>> goto_table;

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void read_parser_input() {
    string line;
    
    productions_lhs.push_back(""); 
    productions_rhs_size.push_back(0);

    getline(cin, line); 
    while (getline(cin, line) && line != "#ACTION") {
        auto parts = split(line, ',');
        productions_lhs.push_back(parts[1]);
        if (parts.size() < 3 || parts[2] == "epsilon") {
            productions_rhs_size.push_back(0);
        } else {
            stringstream ss(parts[2]);
            string word;
            int count = 0;
            while (ss >> word) count++;
            productions_rhs_size.push_back(count);
        }
    }

    getline(cin, line); 
    auto terminals = split(line, ',');
    terminals.erase(terminals.begin());

    while (getline(cin, line) && line != "#GOTO") {
        auto parts = split(line, ',');
        int state = stoi(parts[0]);
        if (action_table.size() <= state) action_table.resize(state + 1);
        for (int i = 1; i < parts.size(); ++i) {
            if (!parts[i].empty()) {
                action_table[state][terminals[i - 1]] = parts[i];
            }
        }
    }
    
    getline(cin, line);
    auto non_terminals = split(line, ',');
    non_terminals.erase(non_terminals.begin());

    while (getline(cin, line) && !line.empty() && line[0] != '#') {
         auto parts = split(line, ',');
         int state = stoi(parts[0]);
         if (goto_table.size() <= state) goto_table.resize(state + 1);
         for (int i = 1; i < parts.size(); ++i) {
            if (!parts[i].empty()) {
                goto_table[state][non_terminals[i - 1]] = stoi(parts[i]);
            }
        }
    }
}

void print_stack(stack<pair<string, int>>& s) {
    stack<pair<string, int>> temp = s;
    string out = "";
    while(!temp.empty()){
        out = to_string(temp.top().second) + " " + temp.top().first + " " + out;
        temp.pop();
    }
    cout << out;
}


int main() {
    cout << "Paste the parse table (PRODUCTIONS, ACTION, GOTO) and press Enter twice:\n";
    read_parser_input();

    string input_line;
    cout << "\nEnter the input string to parse: ";
    getline(cin, input_line);

    vector<string> tokens;
    stringstream ss(input_line);
    string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    tokens.push_back("$");

    stack<pair<string, int>> p_stack;
    p_stack.push({"", 0});
    int ip = 0;
    bool accepted = false;

    cout <<"\nParsing Trace"<<endl;
    printf("%-30s | %-25s | %s\n", "Stack", "Input", "Action");
    cout<<string(70, '-') << "\n";

    while (ip < tokens.size()) {
        int current_state = p_stack.top().second;
        string current_symbol = tokens[ip];

        stringstream stack_ss;
        stack<pair<string, int>> temp_stack = p_stack;
        while (!temp_stack.empty()) {
            stack_ss<<temp_stack.top().first<< " "<<temp_stack.top().second<<" ";
            temp_stack.pop();
        }
        string stack_str = stack_ss.str();
        reverse(stack_str.begin(), stack_str.end());
        printf("%-30s | ", stack_str.c_str());

        stringstream input_ss;
        for (int i = ip; i < tokens.size(); ++i) input_ss << tokens[i] << " ";
        printf("%-25s | ", input_ss.str().c_str());

        if (action_table.size() <= current_state || action_table[current_state].find(current_symbol) == action_table[current_state].end()) {
            cout << "Error: No action defined\n";
            break;
        }
        string action = action_table[current_state][current_symbol];
        cout<<action <<"\n";

        if (action[0] == 's') { // SHIFT
            int next_state = stoi(action.substr(1));
            p_stack.push({current_symbol, next_state});
            ip++;
        } else if (action[0] == 'r') { // REDUCE
            int prod_idx = stoi(action.substr(1));
            int pop_count = productions_rhs_size[prod_idx];
            for (int i = 0; i < pop_count; ++i) {
                p_stack.pop();
            }
            int state_after_pop = p_stack.top().second;
            string lhs = productions_lhs[prod_idx];
            int next_state = goto_table[state_after_pop][lhs];
            p_stack.push({lhs, next_state});
        } else if (action == "acc") { // ACCEPT
            accepted = true;
            break;
        } else {
             cout << "Error: Invalid action in table\n";
             break;
        }
    }

    if (accepted) {
        cout << "Result: Parsing successful! String is accepted.\n";
    } else {
        cout << "Result: Parsing failed! String is rejected.\n";
    }

    return 0;
}