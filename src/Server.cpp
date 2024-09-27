#include <iostream>
#include <string>
#include <cctype>
using namespace std;

bool match_group(const string& input_line, const string& pattern) {
    return input_line.find_first_of(pattern) != string::npos;
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    size_t inp_len = input_line.size();
    size_t patt_len = pattern.size();

    if (patt_len == 0) return true;
    if (inp_len == 0) return false;

    if (pattern.substr(0, 2) == "\\d") {
        if (isdigit(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2));
        }
        return match_pattern(input_line.substr(1), pattern);
    } else if (pattern.substr(0, 2) == "\\w") {
        if (isalnum(input_line[0])) {
            return match_pattern(input_line.substr(1), pattern.substr(2));
        }
        return match_pattern(input_line.substr(1), pattern);
    } else if (pattern[0] == '[') {
        auto first = pattern.find(']');
        bool neg = pattern[1] == '^';
        if (neg) {
            if (!match_group(input_line, pattern.substr(2, first - 2))) {
                return match_pattern(input_line.substr(1), pattern.substr(first + 1));
            }
            return false;
        }
        if (match_group(input_line, pattern.substr(1, first - 1))) {
            return match_pattern(input_line.substr(1), pattern.substr(first + 1));
        } else {
            return false;
        }
    }

    if (pattern[0] == input_line[0]) {
        return match_pattern(input_line.substr(1), pattern.substr(1));
    }

    return match_pattern(input_line.substr(1), pattern);
}

bool match_patterns(string input_line, string pattern) {
    do {
        if (match_pattern(input_line, pattern)) {
            return true;
        }
        input_line = input_line.substr(1);  // move 1 step further
    } while (input_line != "");
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