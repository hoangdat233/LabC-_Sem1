#include "ArgParser.h"
#include <utility>
#include <iostream>

using namespace ArgumentParser;

bool ArgParser::Parse(int argc, char **argv) {
    std::vector<std::string> arguments(argv, argv + argc);
    return Parse(arguments);
}

bool ArgParser::Parse(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        return false;
    }

    int start = 0;
    if (arguments[0][0] != '-') {
        start = 1;
    }

    std::vector<std::string> positional_arguments;
    for (size_t i = start; i < arguments.size(); ++i) {
        const std::string& arg = arguments[i];

        if (arg[0] == '-') {
            int slide = 1;
            if (arg[1] == '-') {
                slide++;  
            }

            std::string key = arg.substr(slide);
            if (key == "help" || key == "h") {
                is_help_request = true;
                return true;  
            }
            size_t equal_pos = arg.find('=');
            if (equal_pos != std::string::npos) {
                key = arg.substr(slide, equal_pos - slide);
                std::string value = arg.substr(equal_pos + 1);

                bool found = false;
                for (auto& string_arg : string_arguments) {
                    if (string_arg->Find(key)) {
                        string_arg->SetValue(value);
                        found = true;
                        break;
                    }
                }
                for (auto& int_arg : int_arguments) {
                    if (int_arg->Find(key)) {
                        int_arg->SetValue(std::stoi(value));
                        found = true;
                        break;
                    }
                }
                for (auto& flag_arg : flag_arguments) {
                    if (flag_arg->Find(key)) {
                        flag_arg->SetValue(true);
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return false; 
                }
            }
            else if (i + 1 < arguments.size() && arguments[i + 1][0] != '-') {
                key = arg.substr(slide);
                std::string value = arguments[++i];

                bool found = false;
                for (auto& string_arg : string_arguments) {
                    if (string_arg->Find(key)) {
                        string_arg->SetValue(value);
                        found = true;
                        break;
                    }
                }
                for (auto& int_arg : int_arguments) {
                    if (int_arg->Find(key)) {
                        int_arg->SetValue(std::stoi(value));
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    return false;  
                }
            }
            else {
                for (size_t j = slide; j < arg.size(); ++j) {
                    std::string flag_key(1, arg[j]);
                    for (auto& flag_arg : flag_arguments) {
                        if (flag_arg->Find(flag_key)) {
                            flag_arg->SetValue(true);
                            break;
                        }
                    }
                }
            }
        } else {
            positional_arguments.push_back(arg);
        }
    }

    for (auto& string_arg : string_arguments) {
        if (!string_arg->IsPositional()) {
            for (auto& element : positional_arguments) {
                string_arg->SetValue(element);
            }
        }
        if (!string_arg->FindAnyValue()) {
            return false;  
        }
    }

    for (auto& int_arg : int_arguments) {
        if (int_arg->IsPositional()) {
            for (auto& element : positional_arguments) {
                int_arg->SetValue(std::stoi(element));
            }
        }
        if (!int_arg->FindAnyValue()) {
            return false;  
        }
    }

    return true;
}

ArgParser::Argument<std::string>& ArgParser::AddStringArgument(char short_key, const std::string& long_key, const std::string& info) {
    std::string temp(1, short_key);
    auto new_arg = new Argument<std::string>(long_key, temp);
    new_arg->argument_description = info;
    string_arguments.push_back(new_arg);
    return *new_arg;
}


ArgParser::Argument<std::string>& ArgParser::AddStringArgument(const std::string& long_key, const std::string& info) {
    auto new_arg = new Argument<std::string>(long_key);
    new_arg->argument_description = info;
    string_arguments.push_back(new_arg);
    return *new_arg;
}



std::string ArgParser::GetStringValue(const std::string& key, int index) {
    for (auto& string_arg : string_arguments) {
        if (string_arg->Find(key)) {
            return string_arg->GetValue(index);
        }
    }
    return "";
}

ArgParser::Argument<int>& ArgParser::AddIntArgument(char short_key, const std::string& long_key, const std::string& info) {
    std::string temp(1, short_key);
    auto new_arg = new Argument<int>(long_key, temp);
    new_arg->argument_description = info;
    int_arguments.push_back(new_arg);
    return *new_arg;
}

ArgParser::Argument<int>& ArgParser::AddIntArgument(const std::string &long_key, const std::string& info) {
    auto new_arg = new Argument<int>(long_key);
    new_arg->argument_description = info;
    int_arguments.push_back(new_arg);
    return *new_arg;
}

int ArgParser::GetIntValue(const std::string& key, int index) {
    for (auto& int_arg : int_arguments) {
        if (int_arg->Find(key)) {
            return int_arg->GetValue(index);
        }
    }
    return 0;
}

std::string ArgParser::HelpDescription() {
    std::string result;
    result += parser_name + '\n';
    result += help_description + '\n';
    result += '\n';

    if (!string_arguments.empty()) {
        for (auto& string_arg : string_arguments) {
            result += string_arg->ArgumentDescription();
        }
    }

    if (!flag_arguments.empty()) {
        for (auto& flag_arg : flag_arguments) {
            result += flag_arg->ArgumentDescription();
        }
    }

    if (!int_arguments.empty()) {
        for (auto& int_arg : int_arguments) {
            result += int_arg->ArgumentDescription();
        }
    }

    result += '\n';
    if (!short_help_key.empty()) {
        result += '-' + short_help_key + ",  ";
    } else {
        result += "     ";
    }
    result += "--" + long_help_key + " Display this help and exit\n";
    return result;
}

ArgParser::Argument<bool>& ArgParser::AddFlag(const std::string& long_key, const std::string& info) {
    auto new_arg = new Argument<bool>(long_key);
    new_arg->argument_description = info;
    flag_arguments.push_back(new_arg);
    return *new_arg;
}

ArgParser::Argument<bool>& ArgParser::AddFlag(char short_key, const std::string& long_key, const std::string& info) {
    std::string temp(1, short_key);
    auto new_arg = new Argument<bool>(temp, long_key);
    new_arg->argument_description = info;
    flag_arguments.push_back(new_arg);
    return *new_arg;
}

bool ArgParser::GetFlag(const std::string& key, int index) {
    for (auto& flag_arg : flag_arguments) {
        if (flag_arg->Find(key)) {
            return flag_arg->GetValue(0);
        }
    }
    return false;
}

void ArgParser::AddHelp(char short_key, const std::string& long_key, const std::string& info) {
    short_help_key = std::string(1, short_key);
    long_help_key = long_key;
    help_description = info;
}

bool ArgParser::Help() const {
    return is_help_request;
}