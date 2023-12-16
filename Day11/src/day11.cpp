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

using Image = vec<vec<u16>>;
auto Inf = std::numeric_limits<float>::infinity();

struct Point
{
    i64 r {};
    i64 c {};
};

u64 solve(const vec<vec<char>>& map, u64 offset)
{
    vec<bool> empty_rows;
    for (const auto& row : map)
    {
        bool empty = ranges::all_of(row.cbegin(), row.cend(), [](auto& ch) { return ch == '.'; });
        empty_rows.push_back(empty);
    }

    vec<bool> empty_cols;
    for (u64 c = 0;
         c < map.at(0).size();
         ++c)
    {
        bool empty = true;

        for (u64 r = 0;
             r < map.size();
             ++r)
        {
            if (map.at(r).at(c) == '#')
            {
                empty = false;
                break;
            }
        }
        
        empty_cols.push_back(empty);
    }

    vec<pair<u64, u64>> galaxies;

    // collect all the galaxies
    for (const auto& [r, row] : std::views::enumerate(map))
    {
        for (const auto& [c, ch] : std::views::enumerate(row))
        {
            if (ch == '#')
            {
                galaxies.emplace_back(r, c);
            }
        }
    }

    offset -= 1;
    u64 acc = 0;
    for (auto gal_a = galaxies.begin();
         gal_a != galaxies.end();
         ++gal_a)
    {
        for (auto gal_b = gal_a + 1;
             gal_b != galaxies.end();
             ++gal_b)
        {
            auto from = std::make_pair(std::min(gal_a->first, gal_b->first), std::min(gal_a->second, gal_b->second));
            auto to = std::make_pair(std::max(gal_a->first, gal_b->first), std::max(gal_a->second, gal_b->second));

            for (u64 r = from.first;
                 r < to.first;
                 ++r)
            {
                ++acc;

                if (empty_rows.at(r))
                    acc += offset;
            }

            for (u64 c = from.second;
                 c < to.second;
                 ++c)
            {
                ++acc;

                if (empty_cols.at(c))
                    acc += offset;
            }
        }
    }

    return acc;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    vec<vec<char>> map;

    for (str line;
         std::getline(ifs, line);
         )
    {
        map.push_back(vec<char>(line.begin(), line.end()));
    }

    auto res = solve(map, 2);
    cout << "day 11 part 1 (" << file_path << ") " << res << endl;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    vec<vec<char>> map;

    for (str line;
         std::getline(ifs, line);
         )
    {
        map.push_back(vec<char>(line.begin(), line.end()));
    }

    auto res = solve(map, 1000000);
    cout << "day 11 part 2 (" << file_path << ") " << res << endl;
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
