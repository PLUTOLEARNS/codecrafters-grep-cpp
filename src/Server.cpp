#include <iostream>
#include <string>
#include <cctype>

bool is_digit(char c) {
    return std::isdigit(static_cast<unsigned char>(c));
}

bool is_word(char c) {
    return std::isalnum(static_cast<unsigned char>(c));
}

bool match_pattern(const std::string& input_line, const std::string& pattern) {
    size_t inp_pos = 0, patt_pos = 0;
    size_t inp_len = input_line.size();
    size_t patt_len = pattern.size();

    while (inp_pos < inp_len && patt_pos < patt_len){
        if (pattern[patt_pos] == '\\') {
            if (patt_pos + 1 < patt_len) {
                char next = pattern[patt_pos + 1];

                if (next == 'd') {
                    if (!is_digit(input_line[inp_pos])){
                        return false;
                    }
                    inp_pos++;
                }
		else if (next == 'w') {
                    if (!is_word(input_line[inp_pos])){
                        return false;
                    }
                    inp_pos++;
                }
		else{
                    throw std::runtime_error("Unknown escape sequence: " + std::string(1, next));
                }
                patt_pos += 2;  // Move past the escape sequence
            }
        } 
	else if (pattern[patt_pos] == '['){
            size_t cb_pos = pattern.find(']', patt_pos);
            if (cb_pos == std::string::npos){
                throw std::runtime_error("Unmatched [ in pattern");
            }
            bool is_negated = (pattern[patt_pos + 1] == '^');
            std::string char_group = is_negated
                                         ? pattern.substr(patt_pos + 2, cb_pos - patt_pos - 2)
                                         : pattern.substr(patt_pos + 1, cb_pos - patt_pos - 1);
            bool found = (char_group.find(input_line[inp_pos]) != std::string::npos);
            if (is_negated && found) {
                return false;
            } 
	    else if (!is_negated && !found){
                return false;
            }
            inp_pos++;
            patt_pos = cb_pos + 1;  // Move past the closing bracket
        } 
	else {
            if (input_line[inp_pos] != pattern[patt_pos]){
                return false;
            }
            inp_pos++;
            patt_pos++;
        }
    }
    return inp_pos == inp_len && patt_pos == patt_len;
}

int main(int argc, char* argv[]){
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3){
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string flag = argv[1];
    std::string pattern = argv[2];

    if (flag != "-E") {
        std::cerr << "Expected first argument to be '-E'" << std::endl;
        return 1;
    }

    // Read the input line
    std::string input_line;
    std::getline(std::cin, input_line);

    try {
        if (match_pattern(input_line, pattern)) {
            return 0;  // Pattern matches
        } else {
            return 1;  // Pattern doesn't match
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
