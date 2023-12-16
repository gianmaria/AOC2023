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

bool operator<(const Point& a, const Point& b)
{
    if (a.r == b.r)
        return a.c < b.c;
    else
        return a.r < b.r;
}

bool operator==(const Point& a, const Point& b)
{
    return a.r == b.r and a.c == b.c;
}


Image parse_input(std::ifstream& stream)
{
    Image image;

    u16 galaxy_counter {0};
    for (str line;
         std::getline(stream, line);
         )
    {
        image.push_back({});
        ranges::for_each(line, [&image, &galaxy_counter](char ch)
        {
            if (ch == '.')
            {
                image.back().push_back(0);
            }
            else
            {
                image.back().push_back(++galaxy_counter);
            }
        });
    }

    return image;
}

void expand_universe(Image& image)
{
    for (auto it = image.begin();
         it != image.end();
         ++it)
    {
        if (std::all_of(it->begin(), it->end(), [](auto elem) { return elem == 0; }))
        {
            it = image.insert(it + 1, vec<u16>(it->size(), 0));
            it += 1; // skip inserted row
        }
    }

    for (u64 c = 0;
         c < image.at(0).size();
         ++c)
    {
        auto all_dots = std::all_of(image.begin(), image.end(),
                                    [c](const auto& row)
        {
            return row[c] == 0;
        });

        if (all_dots)
        {
            // Insert a new column with dots at the current position
            for (auto& row : image)
            {
                row.insert(row.begin() + c, 0);
            }
            ++c;
        }
    }
}

u32 solve(const Image& image)
{
    vec<Point> galaxies;

    // collect all the galaxies
    for (const auto& [r, row] : std::views::enumerate(image))
    {
        for (const auto& [c, value] : std::views::enumerate(row))
        {
            if (value != 0)
            {
                galaxies.emplace_back(r, c);
            }
        }
    }

    map<pair<u32, u32>, u32> res;

    for (const Point& galaxy_a : galaxies)
    {
        for (const Point& galaxy_b : galaxies)
        {
            if (galaxy_b == galaxy_a)
                continue;

            auto from = image[galaxy_a.r][galaxy_a.c];
            auto to = image[galaxy_b.r][galaxy_b.c];

            auto from_to = std::make_pair(from, to);
            auto to_from = std::make_pair(to, from);

            if (
                (res.find(from_to) != res.end())
                or
                (res.find(to_from) != res.end())
                )
            {
                continue;
            }

            u32 dist = std::abs(galaxy_a.r - galaxy_b.r) + std::abs(galaxy_a.c - galaxy_b.c);

            //cout << std::format("from galaxy {} ({},{}) to {} ({},{}) length {}",
            //                    from, galaxy_a.r + 1, galaxy_a.c + 1,
            //                    to, galaxy_b.r + 1, galaxy_b.c + 1,
            //                    dist)
            //    << endl;

            res.insert({from_to, dist});
        }
        //cout << endl;
    }

    return std::accumulate(res.begin(), res.end(), 0U,
                           [](u32 acc, const auto& pair) {
        return acc + pair.second;
    });
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Image image = parse_input(ifs);

    expand_universe(image);

    auto res = solve(image);

    cout << "day 11 part 1 (" << file_path << ") " << res << endl;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    for (str line;
         std::getline(ifs, line);
         )
    {

    }

    auto res = -1;
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
