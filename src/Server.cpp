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
    if (pattern.empty()) return true;
    if (input_line.empty()) return pattern.empty();

    if (pattern[0] == '(') {
        size_t closing_paren = 1;
        size_t nested = 0;
        for (size_t i = 1; i < pattern.size(); ++i) {
            if (pattern[i] == '(') ++nested;
            if (pattern[i] == ')') {
                if (nested == 0) {
                    closing_paren = i;
                    break;
                }
                --nested;
            }
        }
        if (closing_paren < pattern.size()) {
            for (size_t i = 0; i <= input_line.size(); ++i) {
                vector<string> temp_captures = captures;
                if (match_pattern(input_line.substr(0, i), pattern.substr(1, closing_paren - 1), false, temp_captures) &&
                    match_pattern(input_line.substr(i), pattern.substr(closing_paren + 1), anchored, temp_captures)) {
                    temp_captures.push_back(input_line.substr(0, i));
                    captures = temp_captures;
                    return true;
                }
            }
        }
        return false;
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

    if (pattern[0] == '$' && pattern.length() == 1) {
        return input_line.empty();
    }

    if (pattern[0] == '.' || pattern[0] == input_line[0]) {
        return match_pattern(input_line.substr(1), pattern.substr(1), anchored, captures);
    }

    if (pattern.substr(0, 2) == "\\d") {
        if (!input_line.empty() && isdigit(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2), anchored, captures);
        }
    } else if (pattern.substr(0, 2) == "\\w") {
        if (!input_line.empty() && isalnum(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2), anchored, captures);
        }
    }

    if (!anchored && !input_line.empty()) {
        return match_pattern(input_line.substr(1), pattern, false, captures);
    }

    return false;
}

bool match_patterns(const string& input_line, const string& pattern) {
    vector<string> captures;
    return match_pattern(input_line, pattern, false, captures);
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