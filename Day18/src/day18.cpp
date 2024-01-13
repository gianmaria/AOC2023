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
#include <stack>
#include <string_view>
#include <vector>
#include <print>
#include <optional>
#include <chrono>
#include <ctime>
#include <charconv>


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

template<typename T>
void print_matrix(const Matrix<T>& m)
{
    for (const auto& row : m)
    {
        for (const auto& elem : row)
        {
            print("{}", elem);
        }
        println("");
    }
    println("");
}

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

str get_time()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;

    if (localtime_s(&local_time, &time) == 0)
    {
        char buffer[512] {};
        if (strftime(buffer, sizeof(buffer), "%H:%M:%S", &local_time) != 0)
        {
            return {buffer};
        }
    }
    return "";
}

template<typename T>
void save_matrix(const Matrix<T>& m, str_cref filename)
{
    std::ofstream ofs(filename, std::ios::trunc);

    if (not ofs.is_open())
    {
        throw std::format("cannot open file {} for writing", filename);
    }

    for (const auto& row : m)
    {
        for (char ch : row)
        {
            ofs << ch;
        }
        ofs << endl;
    }
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

struct Point
{
    i32 r {};
    i32 c {};
};

bool operator<(const Point& a, const Point& b)
{
    if (a.r == b.r)
        return a.c < b.c;
    else
        return a.r < b.r;
}

// If it's even, it's outside
// if it's odd, it's inside.
template<typename T>
bool is_inside(i32 r, i32 c,
               i32 rows, i32 cols,
               const Matrix<T>& map)
{
    if (map[r][c] == '#')
        return false;

    i32 hit = 0;
    for (i32 cc = c;
         cc < cols;
         ++cc)
    {
        if (map[r][cc] != '#')
            continue;

        bool up_wall = (r - 1 >= 0) and (map[r - 1][cc] == '#');
        bool down_wall = (r + 1 < rows) and (map[r + 1][cc] == '#');

        bool left_clean = (cc - 1 >= 0) and (map[r][cc - 1] != '#');
        bool right_clean = (cc + 1 < cols) and (map[r][cc + 1] != '#');

        if (up_wall and down_wall
            and
            left_clean and right_clean)
        {
            ++hit;
        }
        else
        {
            i32 begin_wall = cc;
            while (cc < cols and
                   map[r][cc] == '#')
            {
                ++cc;
            }
            i32 end_wall = cc-1;

            if (begin_wall == end_wall)
            {
                ++hit;
                continue;
            }

            bool up_wall_begin = (r - 1 >= 0) and (map[r - 1][begin_wall] == '#');
            bool down_wall_begin = (r + 1 < rows) and (map[r + 1][begin_wall] == '#');

            bool up_wall_end = (r - 1 >= 0) and (map[r - 1][end_wall] == '#');
            bool down_wall_end = (r + 1 < rows) and (map[r + 1][end_wall] == '#');

            if (down_wall_begin and down_wall_end)
            {
                hit += 2;
            }
            else if (up_wall_begin and up_wall_end)
            {
                hit += 2;
            }
            else if (down_wall_begin and up_wall_end)
            {
                hit += 1;
            }
            else if (up_wall_begin and down_wall_end)
            {
                hit += 1;
            }
            else
            {
                int s = 0;
            }
        }
    }

    bool odd = (hit % 2) == 1;

    return odd ? true : false;
}

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    vec<Point> points;
    points.emplace_back(0, 0);

    for (auto& line : split_string(input, "\n"))
    {
        auto token = split_string(line, " ");
        i32 len = 0;
        std::from_chars(token[1].data(), token[1].data() + token[1].size(), len);

        char dir = token[0][0];

        auto last_point = points.back();

        if (dir == 'R')
        {
            for (i32 i = 1;
                 i <= len;
                 ++i)
            {
                points.emplace_back(last_point.r, last_point.c + i);
            }
        }
        else if (dir == 'D')
        {
            for (i32 i = 1;
                 i <= len;
                 ++i)
            {
                points.emplace_back(last_point.r + i, last_point.c);
            }
        }
        else if (dir == 'L')
        {
            for (i32 i = 1;
                 i <= len;
                 ++i)
            {
                points.emplace_back(last_point.r, last_point.c - i);
            }
        }
        else if (dir == 'U')
        {
            for (i32 i = 1;
                 i <= len;
                 ++i)
            {
                points.emplace_back(last_point.r - i, last_point.c);
            }
        }
        else
        {
            throw std::format("Unknown direction from input <{}>", token[0]);
        }

    }

    // pop last reduntant point 
    points.pop_back();


    auto it = ranges::min_element(points, [](const Point& a, const Point& b)
    {
        return a.r < b.r;
    });
    i32 min_row = it->r;

    it = ranges::min_element(points, [](const Point& a, const Point& b)
    {
        return a.c < b.c;
    });
    i32 min_col = it->c;

    it = ranges::min_element(points, [](const Point& a, const Point& b)
    {
        return a.r > b.r;
    });
    i32 max_row = it->r;

    it = ranges::min_element(points, [](const Point& a, const Point& b)
    {
        return a.c > b.c;
    });
    i32 max_col = it->c;

    i32 offset_row = min_row < 0 ? -min_row : 0;
    i32 offset_col = min_col < 0 ? -min_col : 0;

    i32 rows = max_row + offset_row + 1;
    i32 cols = max_col + offset_col + 1;

    auto map = vec<vec<char>>(rows, vec<char>(cols, ' '));

    for (const auto& p : points)
    {
        map.at(p.r + offset_row).at(p.c + offset_col) = '#';
    }

    //print_matrix(map);

    u64 res = points.size();

    for (i32 r = 0;
         r < rows;
         ++r)
    {
        for (i32 c = 0;
             c < cols;
             ++c)
        {
            char ch = map[r][c];
            if (ch == '#')
                continue;

            if (is_inside(r, c, rows, cols, map))
            {
                ++res;
                map[r][c] = 'o';
            }
        }
    }

    save_matrix(map, "map.txt");

    return res;
}

u64 part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    /*for (auto& line : split_string(input, "\n"))
    {

    }*/

    u64 acc = 0;

    u64 res = acc;
    return res;
}

int main()
{
    try
    {
        println("day 18 part 1: {}", part1());
        println("day 18 part 2: {}", part2());

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
