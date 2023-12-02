// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <set>
#include <ranges>
#include <format>
#include <vector>
#include <iterator>
#include <map>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using std::cout;
using std::endl;
using std::string;
using std::string_view;
using std::vector;
using std::set;
using str = std::string;
using str_cref = std::string const&;
using std::ranges::find_if;

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace fs = std::filesystem;


str read_file(const char* file_path)
{
    auto ifs = std::ifstream(file_path);

    if (not ifs.is_open())
        throw std::runtime_error("[ERROR] Cannot open file "s + file_path);

    //std::stringstream buffer;
    //buffer << ifs.rdbuf();
    //return buffer.str();

    return str(std::istreambuf_iterator<char>{ifs}, {});
}

void part1()
{
    auto file_path = "res\\input_p1.txt";
    std::istringstream input;
    input.str(read_file(file_path));

    u32 acc = 0;

    for (str line; std::getline(input, line); )
    {
        auto it_first_digit = find_if(line, [](char ch) { return std::isdigit(ch); });
        if (it_first_digit == line.end())
            throw std::runtime_error(std::format("[ERROR] Cannot find first digit in string <{}>", line));

        auto it_last_digit = find_if(line.rbegin(), line.rend(), 
                                     [](char ch) { return std::isdigit(ch); }
        );
        if (it_last_digit == line.rend())
            throw std::runtime_error(std::format("[ERROR] Cannot find last digit in string <{}>", line));

        i32 first_digit = std::stoi(str(1, *it_first_digit));
        i32 last_digit  = std::stoi(str(1, *it_last_digit));
        
        i32 res = first_digit * 10 + last_digit;

        acc += res;
    }

    cout << "part 1 (" << file_path << ") " << acc << endl;
}

struct Digit
{
    i32 digit = -1;
    i32 index = -1;
};

struct Digit_Comp
{
    bool operator()(const Digit& lhs, const Digit& rhs) const
    {
        return lhs.index < rhs.index;
    }
};

void look_for_digit(str_cref line, set<Digit, Digit_Comp>& set)
{
    for (u64 i = 0;
         i < line.size();
         ++i)
    {
        if (std::isdigit(line[i]))
        {
            set.insert(
                {
                    .digit = std::stoi(str(1, line[i])),
                    .index = static_cast<i32>(i)
                }
            );
        }
    }
}

void look_for_letter_digit(str_cref line, set<Digit, Digit_Comp>& set)
{
    vector<str> letter_digits = {
        "one", "two", "three", 
        "four", "five", "six", 
        "seven", "eight", "nine"
    };

    std::map<str, i32> map_letter_digits = {
        {"one", 1},
        {"two", 2},
        {"three", 3},
        {"four", 4},
        {"five", 5},
        {"six", 6},
        {"seven", 7},
        {"eight", 8},
        {"nine", 9}
    };

    for (str_cref digit : letter_digits)
    {
        u64 index = 0;
        while ((index = line.find(digit, index)) != str::npos)
        {
            set.insert(
                {
                    .digit = map_letter_digits.at(digit),
                    .index = static_cast<i32>(index)
                }
            );

            ++index;
        }
    }
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    u32 acc = 0;

    set<Digit, Digit_Comp> set;

    for (str line; 
         std::getline(ifs, line); 
         )
    {
        look_for_digit(line, set);
        look_for_letter_digit(line, set);
        
        i32 first_digit = (*std::begin(set)).digit;
        i32 last_digit = (*std::prev(set.end())).digit;
        
        i32 res = first_digit * 10 + last_digit;

        acc += res;

        //cout << "[DBG] " << line << " => " << first_digit << " + " << last_digit << " = " << res << endl;
        set.clear();
    }

    cout << "part 2 (" << file_path << ") " << acc << endl;
}

int main()
{

    try
    {
        part1();
        part2();
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
