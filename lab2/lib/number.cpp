#include "number.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include <stdexcept> 
#include <utility>

uint239_t Shift_to_Zero(uint239_t value) {
    for (int i = 0; i < 17; i++) {
        std::swap(value.data[i], value.data[34 - i]);
    }
    uint239_t result = uint239_t();
    uint64_t shiftAmount = 0;
    for (int i = 0; i < 35; i++) {
        uint64_t bit = (value.data[i] >> 7) & 1;
        shiftAmount = shiftAmount | (bit << i);
    }

    shiftAmount = shiftAmount % 245;
    for (uint32_t i = 0; i < 245; i++) {
        uint8_t bit = (value.data[i / 7] >> (i % 7)) & 1;
        uint64_t new_index = (i - shiftAmount) % 245;
        result.data[new_index / 7] = result.data[new_index / 7] | (bit << (new_index % 7));
    }
    return result;
}

std::string HalveString(std::string& number) {
    std::string result;
    int remainder = 0;

    for (char digit : number) {
        int current = remainder * 10 + (digit - '0');
        result.push_back(current / 2 + '0');
        remainder = current % 2;
    }
    while (result[0] == '0' && result.length() > 1) {
        result = result.substr(1);
    }
    return result;
}

std::string BigNumtoBinary(std::string number) {
    std::string binary;

    while (number != "0") {
        int lastDigit = number.back() - '0';
        binary.push_back((lastDigit % 2) + '0');
        number = HalveString(number);
    }

    int n = static_cast<int>(binary.length());
    for (int i = 0; i < n / 2; i++) {
        std::swap(binary[i], binary[n - i - 1]);
    }
    return binary;
}

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint32_t temp = shift;
    uint239_t value_239 = uint239_t();
    int block = 0;
    while (value != 0) {
        value_239.data[block] = value & 127;
        value = value >> 7;
        block++;
    }

    uint239_t result = uint239_t();
    shift = shift % 245;
    for (uint32_t i = 0; i < 245; i++) {
        uint8_t bit = (value_239.data[i / 7] >> (i % 7)) & 1;
        uint64_t newPos = (i + shift) % 245;
        result.data[newPos / 7] = result.data[newPos / 7] | (bit << (newPos % 7));
    }

    block = 0;
    while (temp != 0) {
        uint8_t bit = (temp & 1) << 7;
        result.data[block] = result.data[block] | bit;
        temp = temp >> 1;
        block++;
    }

    for (int i = 0; i < 17; i++) {
        std::swap(result.data[i], result.data[34 - i]);
    }
    return result;
}

uint239_t FromString(const char* str, uint32_t shift) {
    uint32_t temp = shift;
    uint239_t string_239 = uint239_t();

    std::string strBinary = str;
    strBinary = BigNumtoBinary(strBinary);
    for (int i = static_cast<int>(strBinary.length()) - 1, j = 0; i >= 0; i--, j++)
    {
        uint8_t bit = strBinary[i] - 48;
        string_239.data[j / 7] = string_239.data[j / 7] | (bit << (j % 7));
    }

    uint239_t result = uint239_t();
    shift = shift % 245;
    for (uint32_t i = 0; i < 245; i++) {
        uint8_t bit = (string_239.data[i / 7] >> (i % 7)) & 1;
        uint64_t newPos = (i + shift) % 245;
        result.data[newPos / 7] = result.data[newPos / 7] | (bit << (newPos % 7));
    }

    int block = 0;
    while (temp != 0) {
        uint8_t bit = (temp & 1) << 7;
        result.data[block] = result.data[block] | bit;
        temp = temp >> 1;
        block++;
    }

    for (int i = 0; i < 17; i++) {
        std::swap(result.data[i], result.data[34 - i]);
    }
    return result;
}

uint239_t operator+(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result = uint239_t();
    uint16_t carry = 0;
    uint239_t left = Shift_to_Zero(lhs);
    uint239_t right = Shift_to_Zero(rhs);
    for (int i = 0; i < 35; ++i) {
        uint16_t sum = static_cast<uint16_t>(left.data[i]) + static_cast<uint16_t>(right.data[i]) + carry;
        result.data[i] = sum & 0x7F; 
        carry = sum >> 7;     
    }
    if (carry > 0) {
        for (int i = 35; i < 36; ++i) {  
            result.data[i] = carry & 0x7F; 
            carry >>= 7; 
        }
    }
    result = Shift_to_Zero(result);
    return result;
}

uint239_t operator-(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t result;
    uint16_t borrow = 0;
    uint239_t left = Shift_to_Zero(lhs);
    uint239_t right = Shift_to_Zero(rhs);
    for (int i = 0; i < 35; ++i) {
        int16_t diff = static_cast<int16_t>(left.data[i]) - static_cast<int16_t>(right.data[i]) - borrow;
        if (diff < 0) {
            diff += 128; 
            borrow = 1;  
        } else {
            borrow = 0;  
        }
        result.data[i] = static_cast<uint8_t>(diff & 0x7F); 
    }
    result = Shift_to_Zero(result);

    return result;
}

uint239_t operator*(const uint239_t& lhs, const uint239_t& rhs) { 
    uint239_t left = Shift_to_Zero(lhs);
    uint239_t right = Shift_to_Zero(rhs);
    uint239_t result = uint239_t();
    
    for (int i = 0; i < 35; ++i) {
        uint8_t left_byte = left.data[i] & 127;
        uint16_t carry = 0;
        for (int j = 0; j < 35 && i + j < 35; ++j) {
            uint8_t right_byte = right.data[j] & 127;
            uint16_t product = left_byte * right_byte + carry + result.data[i + j];
            result.data[i + j] = product & 127;
            carry = product >> 7;
        }
        if (i + 35 < 35) {
            result.data[i + 35] += carry;
        }
    }
    for (int i = 0; i < 17; i++) {
        uint8_t temp = result.data[i];
        result.data[i] = result.data[34 - i];
        result.data[34 - i] = temp;
    }
    return result;
}

bool IsAtLeast(const std::string& first, const std::string& second) {
    if (first.length() != second.length()) {
        return first.length() > second.length();
    }
    return first >= second;
}

std::string ToString(uint239_t value){
    std::string result = "";
    for (int i = 0; i < 35; i++){
        for (int j = 0; j < 7; j++){
            char bit = ((value.data[i] >> j) & 1) + '0';
            result = bit + result;
        }
    }
    while (result[0] == '0' && result.length() > 1){
        result = result.substr(1);
    }
    return result;
}

std::string Sub_Binary(const std::string& a, const std::string& b) {
    std::string result = "";
    int carry = 0, i = static_cast<int>(a.length()) - 1, j = static_cast<int>(b.length()) - 1;
    
    while (i >= 0 || j >= 0 || carry) {
        int bitA = i >= 0 ? a[i--] - '0' : 0;
        int bitB = j >= 0 ? b[j--] - '0' : 0;
        int diff = bitA - bitB - carry;
        
        if (diff < 0) {
            diff += 2;
            carry = 1;
        } else {
            carry = 0;
        }
        result = std::to_string(diff) + result;
    }
    
    while (result[0] == '0' && result.length() > 1){
        result = result.substr(1);
    }
    return result;
}

uint239_t operator/(const uint239_t& lhs, const uint239_t& rhs) {

    std::string left = ToString(Shift_to_Zero(lhs));
    std::string right = ToString(Shift_to_Zero(rhs));

    if (right == "0") {
        throw std::runtime_error("Division by zero");
    }

    std::string quotient = "";
    std::string remainder = "";

    for (size_t i = 0; i < left.size(); ++i) {
        remainder += left[i]; 

        if (remainder[0] == '0' && remainder.length() > 1) {
            remainder = remainder.substr(1);
        }

        if (IsAtLeast(remainder, right)) {
            remainder = Sub_Binary(remainder, right);
            quotient += '1';
        } else {
            quotient += '0';
        }
    }
    uint239_t result = uint239_t();
    for (int i = static_cast<int>(quotient.length()) - 1, j = 0; i >= 0; i--, j++){
        uint8_t bit = quotient[i] - 48;
        result.data[j / 7] = result.data[j / 7] | (bit << (j % 7));
    }
    for (int i = 0; i < 17; i++) {
        std::swap(result.data[i], result.data[34 - i]);
    }
    return result;
}

bool operator==(const uint239_t& lhs, const uint239_t& rhs) {
    uint239_t left = Shift_to_Zero(lhs);
    uint239_t right = Shift_to_Zero(rhs);
    for (int i = 0; i < 35; i++) {
        if (left.data[i] != right.data[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const uint239_t& lhs, const uint239_t& rhs) {
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, const uint239_t& value) {
    for (int i = 34; i >= 0; --i) {
        stream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(value.data[i]);
    }
    return stream;
}

uint64_t GetShift(const uint239_t& value) {
    for (int i = 34; i >= 0; --i) {
        if (value.data[i] != 0) {
            return i * 8;
        }
    }
    return 0;
}
