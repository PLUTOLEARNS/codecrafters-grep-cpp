#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <stdexcept>
using namespace std;

vector<string> captured_groups;

bool is_word_char(char c) {
    return isalnum(c) || c == '_';
}

bool match_group(const string& input_line, const string& pattern) {
    return input_line.find_first_of(pattern) != string::npos;
}

bool match_pattern(const std::string& input_line, const std::string& pattern, bool anchored = false);

bool match_alternation(const string& input_line, const string& pattern, bool anchored = false) {
    size_t pipe = pattern.find('|');
    if (pipe == string::npos) {
        return match_pattern(input_line, pattern, anchored);
    }
    string first = pattern.substr(0, pipe);
    string second = pattern.substr(pipe + 1);
    return match_pattern(input_line, first, anchored) || match_pattern(input_line, second, anchored);
}

bool match_pattern(const std::string& input_line, const std::string& pattern, bool anchored) {
    size_t inp_len = input_line.size();
    size_t patt_len = pattern.size();
    
    if (patt_len == 0) return true;
    if (inp_len == 0) return false;
    
    if (pattern[0] == '(') {
        size_t close_p = pattern.find(')');
        if (close_p != string::npos) {
            string group = pattern.substr(1, close_p - 1);
            for (size_t i = 0; i <= inp_len; ++i) {
                size_t match_len = 0;
                if (match_alternation(input_line.substr(i), group, true)) {
                    while (match_len + i < inp_len && match_alternation(input_line.substr(i, match_len + 1), group, true)) {
                        match_len++;
                    }
                    captured_groups.push_back(input_line.substr(i, match_len));
                    if (match_pattern(input_line.substr(i + match_len), pattern.substr(close_p + 1), anchored)) {
                        return true;
                    }
                    captured_groups.pop_back();
                }
                if (anchored) break;
            }
            return false;
        }
    }

    if (pattern[0] == '\\' && pattern.size() > 1 && isdigit(pattern[1])) {
        int group_num = pattern[1] - '0';
        if (group_num > 0 && group_num <= static_cast<int>(captured_groups.size())) {
            string captured_group = captured_groups[group_num - 1];
            if (inp_len >= captured_group.size() && input_line.substr(0, captured_group.size()) == captured_group) {
                return match_pattern(input_line.substr(captured_group.size()), pattern.substr(2), anchored);
            }
        }
        return false;
    }

    if (pattern[0] == '.') {
        if (inp_len > 0) {
            return match_pattern(input_line.substr(1), pattern.substr(1), anchored);
        } else {
            return false;
        }
    }

    if (pattern[0] == '^') {
        return match_pattern(input_line, pattern.substr(1), true);
    }

    if (pattern[patt_len - 1] == '$') {
        if (inp_len >= patt_len - 1 && input_line.substr(inp_len - (patt_len - 1)) == pattern.substr(0, patt_len - 1)) {
            return true;
        } else {
            return false;
        }
    }

    if (pattern.size() >= 2) {
        if (pattern.substr(0, 2) == "\\d") {
            if (inp_len > 0 && isdigit(input_line[0])) {
                return match_pattern(input_line.substr(1), pattern.substr(2), anchored);
            }
            return false;
        } else if (pattern.substr(0, 2) == "\\w") {
            if (inp_len > 0 && is_word_char(input_line[0])) {
                return match_pattern(input_line.substr(1), pattern.substr(2), anchored);
            }
            return false;
        }
    }

    for (size_t j = 0; j < patt_len; ++j) {
        if (pattern[j] == '?') {
            if (j == 0) return false;
            char preceding_char = pattern[j - 1];
            if (inp_len > 0 && input_line[0] == preceding_char) {
                return match_pattern(input_line.substr(1), pattern.substr(j + 1), anchored);
            } else {
                return match_pattern(input_line, pattern.substr(j + 1), anchored);
            }
        }
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
            return match_pattern(input_line.substr(match_count), pattern.substr(i + 1), anchored);
        }
    }

    if (pattern[0] == '[') {
        auto first = pattern.find(']');
        bool neg = pattern.size() > 1 && pattern[1] == '^';
        if (first == string::npos || first <= 1) return false;

        if (neg) {
            if (inp_len > 0 && !match_group(input_line, pattern.substr(2, first - 2))) {
                return match_pattern(input_line.substr(1), pattern.substr(first + 1), anchored);
            }
            return false;
        }
        if (inp_len > 0 && match_group(input_line, pattern.substr(1, first - 1))) {
            return match_pattern(input_line.substr(1), pattern.substr(first + 1), anchored);
        } else {
            return false;
        }
    }

    if (inp_len > 0 && pattern[0] == input_line[0]) {
        return match_pattern(input_line.substr(1), pattern.substr(1), anchored);
    }

    return false;
}

bool match_patterns(const std::string& input_line, const std::string& pattern) {
    if (!pattern.empty() && pattern[0] == '^') {
        captured_groups.clear();
        return match_pattern(input_line, pattern);
    }
    for (size_t i = 0; i <= input_line.size(); ++i) {
        captured_groups.clear();
        if (match_pattern(input_line.substr(i), pattern)) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    std::cout << "Logs from your program will appear here" << std::endl;
    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }
    std::string flag = argv[1];
    std::string pattern = argv[2];
    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }
    std::string input_line;
    std::getline(std::cin, input_line);
    try {
        if (match_patterns(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }
}