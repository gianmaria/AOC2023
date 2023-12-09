// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <queue>
#include <ranges>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <vector>

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
using std::map;
using std::ranges::find_if;

using str = std::string;
using str_cref = std::string const&;

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace fs = std::filesystem;
namespace ranges = std::ranges;

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
        res.push_back(std::move(trim(token)));
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

template<typename T>
vec<T> solve(vec<T>& nums, bool part_one)
{
    vec<T> next_nums;

    bool all_zeros = ranges::all_of(nums, [](const T& val) { return val == 0; });

    if (all_zeros)
    {
        next_nums = vec<T> {nums};
        
        if (part_one)
            next_nums.push_back(0);
        else
            next_nums.insert(next_nums.begin(), 0);

        return next_nums;
    }
    else
    {
        for (u64 i = 0;
             i < nums.size() - 1;
             ++i)
        {
            auto a = nums[i];
            auto b = nums[i + 1];
            next_nums.push_back(b - a);
        }

        next_nums = solve(next_nums, part_one);

        if (part_one)
        {
            auto a = nums.back();
            auto b = next_nums.back();
            auto c = a + b;
            nums.push_back(c);
        }
        else
        {
            auto a = nums.front();
            auto b = next_nums.front();
            auto c = a - b;
            nums.insert(nums.begin(), c);
        }

        return nums;
    }
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    const auto pattern = std::regex(R"((-?\d+))");

    i32 sum = 0;

    str line;
    for (;
         std::getline(ifs, line);
         )
    {
        auto token = line;
        vec<i32> nums;

        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            nums.push_back(std::stoi(match[1].str()));
            token = match.suffix();
        }

        nums = solve(nums, true);
        sum += nums.back();

        int s = 0;
    }

    auto res = sum;
    cout << "part 1 (" << file_path << ") " << res << endl;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    const auto pattern = std::regex(R"((-?\d+))");

    i32 sum = 0;

    str line;
    for (;
         std::getline(ifs, line);
         )
    {
        auto token = line;
        vec<i32> nums;

        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            nums.push_back(std::stoi(match[1].str()));
            token = match.suffix();
        }

        nums = solve(nums, false);
        sum += nums.front();

        int s = 0;
    }

    auto res = sum;
    cout << "part 2 (" << file_path << ") " << res << endl;
}

int main()
{
    try
    {
        part1();
        part2();
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

    return 0;
}
