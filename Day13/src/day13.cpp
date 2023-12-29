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

vector<str> split_string(str_cref line, char sep)
{
    vector<str> res {};
    std::istringstream iss {line};

    str token {};

    while (std::getline(iss, token, sep))
    {
        res.push_back(trim(token));
    }

    return res;
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


struct Symmetry_Info
{
    u64 pos = 0;
    u64 len = 0;
};

optional<Symmetry_Info> check_symmetry(const vec<str>& input)
{
    Symmetry_Info si {};

    u64 rows = input.size();

    for (u64 r = 0;
         r < rows - 1;
         ++r)
    {
        if (input.at(r) != input.at(r + 1))
            continue;

        si.pos = r;

        // count how "long" the symmetry is
        for (i64 a = r;
             a >= 0;
             --a)
        {
            u64 b = r + (r - a) + 1;
            println("checking {} {}", a, b);

            if (b < rows and
                input.at(a) == input.at(b))
            {
                ++si.len;
            }
        }

        return std::make_optional(si);
    }

    return std::nullopt;
}

vec<str> transpose(const vec<str>& input)
{
    auto t_input = vec<str>(input[0].length(), str(input.size(), 'A'));
    u64 rows = input.size();
    u64 cols = input.at(0).size();

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

void print_input(const vec<str>& input)
{
    for (const auto& row : input)
    {
        println("{}", row);
    }
    println("");
}

pair<bool, u64>
solve(const vec<str>& input)
{
    auto h_symmetry = check_symmetry(input);

    auto t_input = transpose(input);
    auto v_symmetry = check_symmetry(t_input);

    if (h_symmetry.has_value() and
        v_symmetry.has_value())
    {
        print_input(input);
        print_input(t_input);
        println("==========================================");

        if (h_symmetry.value().len > v_symmetry.value().len)
        {
            return std::make_pair(true, h_symmetry.value().pos);
        }
        else if (h_symmetry.value().len < v_symmetry.value().len)
        {
            return std::make_pair(false, v_symmetry.value().pos);
        }
        else
        {
            if (h_symmetry.value().pos > v_symmetry.value().pos)
            {
                return std::make_pair(true, h_symmetry.value().pos);
            }
            else if (h_symmetry.value().pos < v_symmetry.value().pos)
            {
                return std::make_pair(false, v_symmetry.value().pos);
            }
            else
            {
                print_input(input);
                print_input(t_input);
                println("--------------------------------");
            }
        }
    }
    else if (h_symmetry.has_value())
    {
        //println("Horizontal symmetry");
        return std::make_pair(true, h_symmetry.value().pos);
    }
    else if (v_symmetry.has_value())
    {
        //println("Vertical symmetry");
        return std::make_pair(false, v_symmetry.value().pos);
    }
    else
    {
        print_input(input);
        print_input(t_input);

        int s = 0;
    }
}

u64 part1()
{
    auto file_path = "res\\test.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    vec<str> puzzle;
    puzzle.reserve(20);
    u64 acc = 0;

    for (str line;
         std::getline(ifs, line);
         )
    {
        if (line != "")
        {
            puzzle.push_back(std::move(line));
        }
        else
        {
            auto [h_symmetry, pos] = solve(puzzle);
            pos += 1;
            if (h_symmetry)
            {
                acc += 100 * pos;
            }
            else
            {
                acc += pos;
            }

            puzzle.clear();
        }
    }

    // calculate last one
    auto [h_symmetry, pos] = solve(puzzle);
    pos += 1;
    if (h_symmetry)
    {
        acc += 100 * pos;
    }
    else
    {
        acc += pos;
    }


    u64 res = acc;
    return res;
}

u64 part2()
{
    auto file_path = "res\\test.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    for (str line;
         std::getline(ifs, line);
         )
    {

    }

    u64 res = 0;
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
