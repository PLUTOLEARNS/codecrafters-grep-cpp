#include <iostream>
#include <string>
#include <cctype>
#include <vector>
using namespace std;

bool match_pattern(const string& input, const string& pattern, size_t in_pos, size_t pat_pos, vector<string>& captures);

bool match_group(const string& input, const string& pattern, size_t in_pos) {
    return input.substr(in_pos).find_first_of(pattern) == 0;
}

bool match_alternation(const string& input, const string& pattern, size_t in_pos, vector<string>& captures) {
    size_t pipe = pattern.find('|');
    if (pipe == string::npos) {
        return match_pattern(input, pattern, in_pos, 0, captures);
    }
    
    string first = pattern.substr(0, pipe);
    string second = pattern.substr(pipe + 1);

    vector<string> first_captures = captures;
    if (match_pattern(input, first, in_pos, 0, first_captures)) {
        captures = first_captures;
        return true;
    }

    vector<string> second_captures = captures;
    if (match_pattern(input, second, in_pos, 0, second_captures)) {
        captures = second_captures;
        return true;
    }

    return false;
}

bool match_pattern(const string& input, const string& pattern, size_t in_pos, size_t pat_pos, vector<string>& captures) {
    while (pat_pos < pattern.length()) {
        if (pattern[pat_pos] == '(') {
            size_t group_start = in_pos;
            size_t end_paren = pattern.find(')', pat_pos);
            if (end_paren == string::npos) return false;

            size_t next_pat_pos = end_paren + 1;
            while (in_pos <= input.length()) {
                vector<string> temp_captures = captures;
                if (match_pattern(input, pattern.substr(pat_pos + 1, end_paren - pat_pos - 1), in_pos, 0, temp_captures) &&
                    match_pattern(input, pattern, in_pos, next_pat_pos, temp_captures)) {
                    temp_captures.push_back(input.substr(group_start, in_pos - group_start));
                    captures = temp_captures;
                    return true;
                }
                in_pos++;
            }
            return false;
        } else if (pattern[pat_pos] == '\\' && isdigit(pattern[pat_pos + 1])) {
            int backreference = pattern[pat_pos + 1] - '0';
            if (backreference > 0 && backreference <= captures.size()) {
                string captured = captures[backreference - 1];
                if (input.substr(in_pos, captured.length()) == captured) {
                    in_pos += captured.length();
                    pat_pos += 2;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        } else if (pattern[pat_pos] == '|') {
            return match_alternation(input, pattern.substr(pat_pos), in_pos, captures);
        } else if (in_pos >= input.length()) {
            return false;
        } else if (pattern[pat_pos] == '.') {
            in_pos++;
            pat_pos++;
        } else if (pattern[pat_pos] == input[in_pos]) {
            in_pos++;
            pat_pos++;
        } else {
            return false;
        }
    }
    return in_pos == input.length();
}

bool match_patterns(const string& input, const string& pattern) {
    vector<string> captures;
    return match_pattern(input, pattern, 0, 0, captures);
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