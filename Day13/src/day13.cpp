// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>
#include <utility>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <unordered_map>
#include <numeric>
#include <queue>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <print>
#include <optional>

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
using std::array;
using std::set;
using std::map;
using std::pair;
using std::print;
using std::println;
using std::optional;

using str = std::string;
using strv = std::string_view;
using str_cref = std::string const&;

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace fs = std::filesystem;
namespace ranges = std::ranges;
namespace views = std::views;

template<typename T>
using vec = vector<T>;


// ==============================================
// ==============================================
//
//  _    _   _______   _____   _         _____  
// | |  | | |__   __| |_   _| | |       / ____| 
// | |  | |    | |      | |   | |      | (___   
// | |  | |    | |      | |   | |       \___ \  
// | |__| |    | |     _| |_  | |____   ____) | 
//  \____/     |_|    |_____| |______| |_____/  
//                                              
//                                               
// ==============================================
// ==============================================

template<typename T>
using Matrix = vec<vec<T>>;

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

// Trim from the beginning (left)
str ltrim(str_cref s)
{
    auto it = std::find_if(s.begin(), s.end(), [](char ch)
    {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return str(it, s.end());
}

// Trim from the end (right)
str rtrim(str_cref s)
{
    auto it = std::find_if(s.rbegin(), s.rend(), [](char ch)
    {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return str(s.begin(), it.base());
}

// Trim from both ends
str trim(str_cref s)
{
    return ltrim(rtrim(s));
}

vector<str> split_string(str_cref input, str_cref separator)
{
    vector<str> tokens;
    size_t start = 0, end;

    while ((end = input.find(separator, start)) != str::npos)
    {
        tokens.push_back(input.substr(start, end - start));
        start = end + separator.length();
    }

    tokens.push_back(input.substr(start));

    return tokens;
}

bool contains(str_cref string, str_cref text)
{
    return string.find(text) != str::npos;
}

template<typename T>
static inline bool is_between(T num, T min, T max)
{
    return (num >= min) and (num <= max);
}


// ==============================================
// ==============================================
//
//   _____    ____    _____    ______ 
//  / ____|  / __ \  |  __ \  |  ____|
// | |      | |  | | | |  | | | |__   
// | |      | |  | | | |  | | |  __|  
// | |____  | |__| | | |__| | | |____ 
//  \_____|  \____/  |_____/  |______|
//                                    
// ==============================================
// ==============================================



vec<str> transpose(const vec<str>& input)
{
    u64 rows = input.size();
    u64 cols = input.at(0).size();
    auto t_input = vec<str>(cols, str(rows, 'A'));

    for (u64 r = 0;
         r < rows;
         ++r)
    {
        for (u64 c = 0;
             c < cols;
             ++c)
        {
            t_input.at(c).at(r) = input.at(r).at(c);
        }
    }

    return t_input;
}

void print_puzzle(const vec<str>& input)
{
    for (const auto& row : input)
    {
        println("{}", row);
    }
    println("");
}

bool can_fold(const vec<str>& input, u64 r)
{
    u64 rows = input.size();

    i64 top = r;
    u64 bottom = r + 1;

    while (top >= 0 and bottom < rows)
    {
        if (input.at(top) != input.at(bottom))
        {
            return false;
        }
        --top;
        ++bottom;
    }

    return true;
}

u64 check_symmetry(const vec<str>& input)
{
    u64 rows = input.size();

    // NOTE: 
    // look for adjacent matching lines
    // then fold and check if all the overlapping lines matches
    for (u64 r = 0;
         r < rows - 1;
         ++r)
    {
        if (input.at(r) == input.at(r + 1))
        {
            if (can_fold(input, r))
            {
                return r + 1;
            }
        }
    }

    return 0;
}

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    u64 acc = 0;
    for (auto& block : split_string(input, "\n\n"))
    {
        auto puzzle = split_string(block, "\n");
        
        auto h_symmetry = check_symmetry(puzzle);

        auto t_puzzle = transpose(puzzle);
        auto v_symmetry = check_symmetry(t_puzzle);

        acc += (h_symmetry * 100) + v_symmetry;
    }

    u64 res = acc;
    return res;
}



u64 diff_count(str_cref a, str_cref b)
{
    u64 diff_count = 0;

    for (u64 i = 0;
         i < a.length();
         ++i)
    {
        if (a.at(i) != b.at(i))
        {
            ++diff_count;
        }
    }

    return diff_count;
}

bool can_fold_with_one_fixed_smudge(const vec<str>& input, u64 r)
{
    u64 rows = input.size();

    i64 top = r;
    u64 bottom = r + 1;

    bool fixed_one = false;
    while (top >= 0 and bottom < rows)
    {
        if (input.at(top) != input.at(bottom))
        {
            if (fixed_one)
                return false;
            else if (diff_count(input.at(top), input.at(bottom)) == 1)
                fixed_one = true;
            else
                return false;
        }
        --top;
        ++bottom;
    }

    return fixed_one;
}

u64 find_smudge(const vec<str>& input)
{
    u64 rows = input.size();

    for (u64 r = 0;
         r < rows;
         ++r)
    {
        if (can_fold_with_one_fixed_smudge(input, r))
        {
            return r + 1;
        }
    }

    return 0;
}

u64 part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    u64 acc = 0;
    for (auto& block : split_string(input, "\n\n"))
    {
        auto puzzle = split_string(block, "\n");
        
        auto h_symmetry = find_smudge(puzzle);

        auto t_puzzle = transpose(puzzle);
        auto v_symmetry = find_smudge(t_puzzle);

        acc += (h_symmetry * 100) + v_symmetry;
    }

    u64 res = acc;
    return res;
}

int main()
{
    try
    {
        println("day 13 part 1: {}", part1());
        println("day 13 part 2: {}", part2());

        return 0;
    }
    catch (const char* e)
    {
        cout << "[ERROR] " << e << endl;
    }
    catch (str_cref e)
    {
        cout << "[ERROR] " << e << endl;
    }
    catch (const std::exception& e)
    {
        cout << "[EXC] " << e.what() << endl;
    }

    return 1;
}
