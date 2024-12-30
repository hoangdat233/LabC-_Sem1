#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace ArgumentParser {

class ArgParser {
    private:
    std::string parser_name;
    size_t parser_size = 0;
    
    template<typename T>
    class Argument {
    private:
        bool is_positional_arg = false;
        bool is_multivalue_arg = false;
        bool has_default_value = false;
        size_t min_required_args = 1;
        T default_value;

        std::vector<std::string> argument_keys;
        std::vector<T> unique_values_storage;

        T* value_storage = nullptr;
        std::vector<T>* values_storage = nullptr;
        
    public:
        
        std::string argument_description;

        Argument(std::string long_key = "", std::string short_key = "") : argument_keys{long_key, short_key} {};

        Argument& StoreValue(T& value) {
            value_storage = &value;
            return *this;
        }

        Argument& StoreValues(std::vector<T>& values) {
            values_storage = &values;
            return *this;
        }

        Argument& MultiValue(size_t min_args = 1) {
            is_multivalue_arg = true;
            min_required_args = min_args;
            return *this;
        }

        Argument& Positional() {
            is_positional_arg = true;
            return *this;
        }

        bool IsPositional() const {
            return is_positional_arg;
        }

        void Default(T value) {
            has_default_value = true;
            default_value = value;
        }

        void SetValue(T value) {
            if (value_storage != nullptr) {
                *value_storage = value;
            } else if (values_storage != nullptr) {
                (*values_storage).push_back(value);
            } else {
                unique_values_storage.push_back(value);
            }
        }

        T GetValue(int index) const {
            if (value_storage != nullptr) {
                return *value_storage;
            } else if (values_storage != nullptr) {
                if ((*values_storage).size() - 1 >= index) {
                    return (*values_storage)[index];
                } else {
                    return default_value;
                }
            } else {
                if (unique_values_storage.size() - 1 >= index && !unique_values_storage.empty()) {
                    return unique_values_storage[index];
                } else {
                    return default_value;
                }
            }
        }

        bool Find(const std::string& key) const {
            for (auto element : argument_keys) {
                if (element == key) {
                    return true;
                }
            }
            return false;
        }

        bool FindAnyValue() const {
            if (value_storage != nullptr || values_storage != nullptr || !unique_values_storage.empty() || has_default_value) {
                if (is_multivalue_arg && (values_storage->size() < min_required_args && unique_values_storage.size() < min_required_args) && !has_default_value) {
                    return false;
                }
                return true;
            }
            return false;
        }

        std::string ArgumentDescription() const {
            std::string result;

            if (argument_keys.size() >= 2) {
                if (argument_keys[0].size() == 1) {
                    result += '-' + argument_keys[0] + ",  ";
                    result += "--" + argument_keys[1] + ",  ";
                } else if (argument_keys[1].size() == 1) {
                    result += '-' + argument_keys[1] + ",  ";
                    result += "--" + argument_keys[0] + ",  ";
                } else {
                    result += "     --" + argument_keys[0] + ",  ";
                }
            }

            result += argument_description + ' ';
            std::vector<std::string> info;
            if (is_multivalue_arg || is_positional_arg || min_required_args > 0) {
                if (is_multivalue_arg) {
                    info.push_back("repeated");
                }
                if (is_positional_arg) {
                    info.push_back("positional");
                }
                if (min_required_args >= 1) {
                    info.push_back("min args = " + std::to_string(min_required_args));
                }
            }
            if (!info.empty()) {
                result += " [";
                for (int i = 0; i < info.size() - 1; ++i) {
                    result += info[i] + ", ";
                }
                result += info[info.size() - 1] + "]";
            }
            result += '\n';
            return result;
        }
    };

    public:

    bool is_help_request = false;
    std::string short_help_key;
    std::string long_help_key;
    std::string help_description;

    std::vector<Argument<std::string>*> string_arguments;
    std::vector<Argument<int>*> int_arguments;
    std::vector<Argument<bool>*> flag_arguments;

    ArgParser(std::string name) : parser_name(name) {};
    ArgParser() : parser_name("Parser") {};

    bool Parse(int argc, char** argv);

    bool Parse(const std::vector<std::string>& input);

    Argument<std::string>& AddStringArgument(const std::string& long_key, const std::string& description = "");

    Argument<std::string>& AddStringArgument(char short_key, const std::string& long_key, const std::string& description = "");

    std::string GetStringValue(const std::string& key, int index = 0);

    Argument<bool>& AddFlag(char short_key, const std::string& long_key, const std::string& description = "");

    Argument<bool>& AddFlag(const std::string& long_key, const std::string& description = "");

    bool GetFlag(const std::string& key, int index = 0);

    Argument<int>& AddIntArgument(const std::string& long_key, const std::string& description = "");

    Argument<int>& AddIntArgument(char short_key, const std::string& long_key, const std::string& description = "");

    int GetIntValue(const std::string& key, int index = 0);

    void AddHelp(char short_key, const std::string& long_key, const std::string& description = "");

    bool Help() const;

    std::string HelpDescription();

    ~ArgParser() {
        for (auto arg : string_arguments) {
            delete arg;
        }

        for (auto arg : int_arguments) {
            delete arg;
        }

        for (auto arg : flag_arguments) {
            delete arg;
        }
    }
};
}
