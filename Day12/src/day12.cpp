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
using std::pair;

using str = std::string;
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


u64 compute(str_cref input, const vec<u64>& spring_groups)
{
    auto create_str = [](std::stringstream& ss,
                         const vec<u64>& indexes,
                         str_cref alphabet)
    {
        ss.str("");
        for (auto [index, _] : views::enumerate(indexes))
        {
            ss << alphabet[indexes[static_cast<u64>(index)]];
        }
        return ss.str();
    };

    std::stringstream ss;
    str alphabet = ".#";

    auto qmark_count = static_cast<u64>(ranges::count(input, '?'));
    auto max = static_cast<u64>(std::pow(alphabet.size(), qmark_count));
    
    vec<str> collection;
    auto indexes = vec<u64>(qmark_count, 0);
    collection.reserve(max);
    u64 counter = 0;
    // generate all the possible permutations of the alphabet
    while (counter < max)
    {
        for (u64 i = 0;
             i < alphabet.size();
             ++i, ++counter)
        {
            collection.push_back(create_str(ss, indexes, alphabet));
            indexes[0] += 1;
        }

        for (u64 i = 0;
             i < indexes.size() - 1;
             ++i)
        {
            if (indexes[i] >= alphabet.size())
            {
                indexes[i] = 0;
                indexes[i + 1] += 1;
            }
        }
    }

    /*
    * regex = \.* #{1} \.+ #{1} \.+ #{3} \.*
    */
    // cerate the regex based on spring_groups
    // for checking valid combination of operational and damaged springs 
    // out of every geenrated combination
    ss.str("");
    ss << "\\.*";
    for (u64 i = 0; i < spring_groups.size() - 1; ++i)
    {
        ss << "(#{" << spring_groups[i] << "})" << "\\.+";
    }
    ss << "(#{" << spring_groups.back() << "})" << "\\.*";

    const auto regex = std::regex(ss.str());

    vec<str> correct;
    for (const auto& elem : collection)
    {
        auto to_test = str(input.length(), 'A');
        u64 elem_index = 0;

        for (auto [to_test_index, ch] : views::enumerate(input))
        {
            if (ch != '?')
            {
                to_test.at(to_test_index) = ch;
            }
            else
            {
                to_test.at(to_test_index) = elem.at(elem_index++);
            }
        }

        if (std::regex_match(to_test, regex))
        {
            correct.push_back(std::move(to_test));
        }
    }

    return correct.size();
}

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    u64 acc = 0;
    u64 count = 0;
    for (str line;
         std::getline(ifs, line);
         )
    {
        auto parts = split_string(line, ' ');
        auto nums_str = split_string(parts[1], ',');

        vec<u64> nums;
        std::transform(nums_str.begin(),
                       nums_str.end(),
                       std::back_inserter(nums),
                       [](str_cref str) { return std::stoull(str); }
        );

        cout << std::format("{}) working with <{}>", ++count, parts[0]) << endl;
        acc += compute(parts[0], nums);
    }

    u64 res = acc;
    return res;
}

u64 part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);


    for (str line;
         std::getline(ifs, line);
         )
    {
        //map.emplace_back(vec<char>{line.begin(), line.end()});
    }

    u64 res = 0;
    return res;
}

int main()
{
    try
    {
        auto p1 = part1();
        cout << "day 12 part 1: " << p1 << endl;

        auto p2 = part2();
        cout << "day 12 part 2: " << p2 << endl;
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
