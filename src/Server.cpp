#include <iostream>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

bool match_group(const string& input_line, const string& pattern) {
    return input_line.find_first_of(pattern) != string::npos;
}

bool match_pattern(const string& input_line, const string& pattern, bool anchored, vector<string>& captures);

bool match_alternation(const string& input_line, const string& pattern, bool anchored, vector<string>& captures) {
    size_t pipe = pattern.find('|');
    if (pipe == string::npos) {
        return match_pattern(input_line, pattern, anchored, captures);
    }
    string first = pattern.substr(0, pipe);
    string second = pattern.substr(pipe + 1);

    vector<string> first_captures = captures;
    if (match_pattern(input_line, first, anchored, first_captures)) {
        captures = first_captures;
        return true;
    }

    vector<string> second_captures = captures;
    if (match_pattern(input_line, second, anchored, second_captures)) {
        captures = second_captures;
        return true;
    }

    return false;
}

bool match_pattern(const string& input_line, const string& pattern, bool anchored, vector<string>& captures) {
    size_t inp_len = input_line.size();
    size_t patt_len = pattern.size();

    if (patt_len == 0) return true;
    if (inp_len == 0) return false;

    if (pattern[0] == '(') {
        size_t closing_paren = pattern.find(')');
        if (closing_paren != string::npos) {
            string captured;
            bool match = match_pattern(input_line, pattern.substr(1, closing_paren - 1), anchored, captures);
            if (match) {
                captured = input_line.substr(0, closing_paren - 1);
                captures.push_back(captured);
                return match_pattern(input_line.substr(captured.length()), pattern.substr(closing_paren + 1), anchored, captures);
            }
            return false;
        }
    }

    if (pattern[0] == '\\' && isdigit(pattern[1])) {
        int backreference = pattern[1] - '0';
        if (backreference > 0 && backreference <= captures.size()) {
            string captured = captures[backreference - 1];
            if (input_line.substr(0, captured.length()) == captured) {
                return match_pattern(input_line.substr(captured.length()), pattern.substr(2), anchored, captures);
            }
        }
        return false;
    }

    if (pattern.find('|') != string::npos) {
        return match_alternation(input_line, pattern, anchored, captures);
    }

    if (pattern[0] == '^') {
        return match_pattern(input_line, pattern.substr(1), true, captures);
    }

    for (size_t i = 0; i < patt_len; ++i) {
        if (pattern[i] == '+') {
            if (i == 0) return false;
            char preceding_char = pattern[i - 1];
            size_t match_count = 0;
            while (match_count < inp_len && input_line[match_count] == preceding_char) {
                match_count++;
            }
            if (match_count == 0) return false;
            return match_pattern(input_line.substr(match_count), pattern.substr(i + 1), anchored, captures);
        }
    }

    for (size_t j = 0; j < patt_len; ++j) {
        if (pattern[j] == '?') {
            if (j == 0) return false;
            char preceding_char = pattern[j - 1];
            if (inp_len > 0 && input_line[0] == preceding_char) {
                return match_pattern(input_line.substr(1), pattern.substr(j + 1), anchored, captures);
            } else {
                return match_pattern(input_line, pattern.substr(j + 1), anchored, captures);
            }
        }
    }

    if (pattern[patt_len - 1] == '$') {
        if (inp_len >= patt_len - 1 && input_line.substr(inp_len - (patt_len - 1)) == pattern.substr(0, patt_len - 1)) {
            return true;
        } else {
            return false;
        }
    }

    if (pattern[0] == '.') {
        if (inp_len > 0) {
            return match_pattern(input_line.substr(1), pattern.substr(1), anchored, captures);
        } else {
            return false;
        }
    }

    if (anchored && input_line.empty()) {
        return false;
    }

    if (pattern.substr(0, 2) == "\\d") {
        if (isdigit(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2), anchored, captures);
        }
        return false;
    } else if (pattern.substr(0, 2) == "\\w") {
        if (isalnum(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2), anchored, captures);
        }
        return false;
    } else if (pattern[0] == '[') {
        auto first = pattern.find(']');
        bool neg = pattern[1] == '^';
        if (neg) {
            if (!match_group(input_line, pattern.substr(2, first - 2))) {
                return match_pattern(input_line.substr(1), pattern.substr(first + 1), anchored, captures);
            }
            return false;
        }
        if (match_group(input_line, pattern.substr(1, first - 1))) {
            return match_pattern(input_line.substr(1), pattern.substr(first + 1), anchored, captures);
        } else {
            return false;
        }
    }

    if (pattern[0] == input_line[0]) {
        return match_pattern(input_line.substr(1), pattern.substr(1), anchored, captures);
    }

    if (!anchored && !input_line.empty()) {
        return match_pattern(input_line.substr(1), pattern, false, captures);
    }

    return false;
}

bool match_patterns(const string& input_line, const string& pattern) {
    vector<string> captures;
    if (pattern[0] == '^') {
        return match_pattern(input_line, pattern.substr(1), true, captures);
    }

    for (size_t i = 0; i < input_line.size(); ++i) {
        if (match_pattern(input_line.substr(i), pattern, false, captures)) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    cout << "Logs from your program will appear here" << endl;

    if (argc != 3) {
        cerr << "Expected two arguments" << endl;
        return 1;
    }

    string flag = argv[1];
    string pattern = argv[2];

    if (flag != "-E") {
        cerr << "Expected first argument to be '-E'" << endl;
        return 1;
    }

    string input_line;
    getline(cin, input_line);

    try {
        if (match_patterns(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const runtime_error& e) {
        cerr << e.what() << endl;
        return 1;
    }
}