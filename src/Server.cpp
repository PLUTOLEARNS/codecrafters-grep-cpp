#include <iostream>
#include <string>
#include <cctype>
using namespace std;

bool match_group(const string& input_line, const string& pattern) {
    return input_line.find_first_of(pattern) != string::npos;
}

bool match_pattern(const string& input_line, const string& pattern, bool anchored = false);

bool match_alternation(const string& input_line, const string& pattern, bool anchored = false) {
    size_t pipe = pattern.find('|');
    if (pipe == string::npos) {
        return match_pattern(input_line, pattern, anchored);
    }
    string first = pattern.substr(0, pipe);
    string second = pattern.substr(pipe + 1);

    return match_pattern(input_line, first, anchored) || match_pattern(input_line, second, anchored);
}

bool match_pattern(const std::string& input_line, const std::string& pattern, bool anchored = false) {
    size_t i = 0,j = 0;
    size_t inp_len = input_line.size();
    size_t patt_len = pattern.size();

    if (patt_len == 0) return true;
    if (inp_len == 0) return false;

    if (pattern.find('|') != string::npos) {
        return match_alternation(input_line, pattern, anchored);
    }

    if (pattern[0] == '^') {
        return match_pattern(input_line, pattern.substr(1), true);
    }
    for (size_t i = 0; i < patt_len; ++i) {
        if (pattern[i] == '+') {
            if (i == 0) return false;

            char preceding_char = pattern[i - 1];  // Character before the '+'
            size_t match_count = 0;
            while (match_count < inp_len && input_line[match_count] == preceding_char) {
                match_count++;
            }
            if (match_count == 0) return false;
            return match_pattern(input_line.substr(match_count), pattern.substr(i + 1), anchored);
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

    if (pattern[patt_len - 1] == '$') {
        if (inp_len >= patt_len - 1 && input_line.substr(inp_len - (patt_len - 1)) == pattern.substr(0, patt_len - 1)) {
            return true;
        } else {
            return false;
        }
    }
    if (pattern[0] == '.') {
        if (inp_len > 0) {
            // Skip one character in input_line and continue matching
            return match_pattern(input_line.substr(1), pattern.substr(1), anchored);
        } else {
            return false;
        }
    }

    if (anchored && input_line.empty()) {
        return false;  // In case the input ends early when anchored
    }

    if (pattern.substr(0, 2) == "\\d") {
        if (isdigit(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2), anchored);
        }
        return false;
    } else if (pattern.substr(0, 2) == "\\w") {
        if (isalnum(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2), anchored);
        }
        return false;
    } else if (pattern[0] == '[') {
        auto first = pattern.find(']');
        bool neg = pattern[1] == '^';
        if (neg) {
            if (!match_group(input_line, pattern.substr(2, first - 2))) {
                return match_pattern(input_line.substr(1), pattern.substr(first + 1), anchored);
            }
            return false;
        }
        if (match_group(input_line, pattern.substr(1, first - 1))) {
            return match_pattern(input_line.substr(1), pattern.substr(first + 1), anchored);
        } else {
            return false;
        }
    }

    if (pattern[0] == input_line[0]) {
        return match_pattern(input_line.substr(1), pattern.substr(1), anchored);
    }

    return false;  // If the characters don't match
}

bool match_patterns(const std::string& input_line, const std::string& pattern) {
    if (pattern[0] == '^') {
        // Match at the beginning of the line (anchored)
        return match_pattern(input_line, pattern);
    }

    // Otherwise, search the pattern throughout the input string
    for (size_t i = 0; i < input_line.size(); ++i) {
        if (match_pattern(input_line.substr(i), pattern)) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
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

    // Get input from stdin
    std::string input_line;
    std::getline(std::cin, input_line);

    try {
        if (match_patterns(input_line, pattern)) {
            return 0;
        } else {
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
