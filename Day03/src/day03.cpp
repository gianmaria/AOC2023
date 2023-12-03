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
#include <regex>
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

template<typename T>
using vec = vector<T>;

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
str ltrim(str_cref s) {
    auto it = std::find_if(s.begin(), s.end(), [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return str(it, s.end());
}

// Trim from the end (right)
str rtrim(str_cref s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return str(s.begin(), it.base());
}

// Trim from both ends
str trim(str_cref s) {
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


bool is_symbol(char ch)
{
    bool res =
        (not std::isdigit(ch)) and
        (ch != '.');

    return res;
}

using Grid = vec<vec<char>>;

bool is_in_bounds(Grid& grid, i64 r, i64 c)
{
    auto rows = grid.size();
    auto cols = grid.at(0).size();

    bool res =
        (r >= 0 and r < rows)
        and
        (c >= 0 and c < cols);

    return res;
}

bool is_digit_adjacent_to_symbol(Grid& grid, i64 r, i64 c)
{
    bool res = false;

    // up (r-1, c)
    if (is_in_bounds(grid, r - 1, c))
    {
        res |= is_symbol(grid.at(r - 1).at(c));
    }

    // down (r+1, c)
    if (is_in_bounds(grid, r + 1, c))
    {
        res |= is_symbol(grid.at(r + 1).at(c));
    }

    // left (r, c-1)
    if (is_in_bounds(grid, r, c - 1))
    {
        res |= is_symbol(grid.at(r).at(c - 1));
    }

    // right (r, c+1)
    if (is_in_bounds(grid, r, c + 1))
    {
        res |= is_symbol(grid.at(r).at(c + 1));
    }

    // up left (r-1, c-1)
    if (is_in_bounds(grid, r - 1, c - 1))
    {
        res |= is_symbol(grid.at(r - 1).at(c - 1));
    }

    // up right (r-1, c+1)
    if (is_in_bounds(grid, r - 1, c + 1))
    {
        res |= is_symbol(grid.at(r - 1).at(c + 1));
    }

    // down left (r+1, c-1)
    if (is_in_bounds(grid, r + 1, c - 1))
    {
        res |= is_symbol(grid.at(r + 1).at(c - 1));
    }

    // down right (r+1, c+1)
    if (is_in_bounds(grid, r + 1, c + 1))
    {
        res |= is_symbol(grid.at(r + 1).at(c + 1));
    }

    return res;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    auto grid = vec<vec<char>> {};

    for (str line;
         std::getline(ifs, line);
         )
    {
        auto row = vec<char> {};

        for (char c : line)
        {
            row.push_back(c);
        }
        grid.push_back(std::move(row));
    }


    i64 acc = 0;
    auto rows = grid.size();
    auto cols = grid.at(0).size();

    for (i64 r = 0;
         r < rows;
         ++r)
    {
        // row: "467..114.."
        for (i64 c = 0;
             c < cols;
             ++c)
        {
            auto& col = grid.at(r);
            char ch = col.at(c);

            if (std::isdigit(ch))
            {
                str num;
                while (c < cols and
                       std::isdigit(col.at(c)))
                {
                    num.push_back(col.at(c));
                    ++c;
                }

                bool adj = false;
                // for each digit in number
                for (u64 i = c - num.size();
                     i < c;
                     ++i)
                {
                    bool res = is_digit_adjacent_to_symbol(grid, r, i);
                    adj |= res;
                }

                if (adj)
                {
                    //cout << std::format("({},{}) {} OK", r+1,c+1,num) << endl;
                    acc += std::stoi(num);
                }
                else
                {
                    //cout << std::format("({},{}) {} sad", r+1,c+1,num) << endl;
                }
            }

        }
    }

    i32 res = acc;
    cout << "part 1 (" << file_path << ") " << res << endl;
}

bool is_gear(char ch)
{
    return ch == '*';
}

struct Point
{
    i64 r = 0;
    i64 c = 0;
};

bool operator<(const Point& lhs, const Point& rhs)
{
    if (lhs.r == rhs.r)
    {
        return lhs.c < rhs.c;
    }
    else
    {
        return lhs.r < rhs.r;
    }
}

struct Num
{
    str num;
    Point begin;
    Point end;
};

struct Num_Comp
{
    bool operator()(const Num& lhs, const Num& rhs) const
    {
        bool res = lhs.begin < rhs.begin and
            lhs.end < rhs.end;
        return res;
    }
};

void red_nums_in_row(Grid& grid, vec<Num>& nums, i64 r)
{
    if (r < 0)
        return;
    if (r > grid.size())
        return;

    auto& col = grid.at(r);
    auto cols = col.size();

    for (i64 c = 0;
         c < cols;
         ++c)
    {
        auto ch = col.at(c);

        if (std::isdigit(ch))
        {
            Num num;
            num.begin.r = r;
            num.begin.c = c;

            while (c < cols and
                   std::isdigit(col.at(c)))
            {
                num.num.push_back(col.at(c));
                ++c;
            }

            num.end.r = r;
            num.end.c = c - 1;

            nums.push_back(std::move(num));
        }
    }

    int s = 0;
}

i64 calculate_gear_ratio(Grid& grid, vec<Num>& nums, i64 gear_r, i64 gear_c)
{
    set<Num, Num_Comp> num_set;

    // up (r-1, c)
    if (is_in_bounds(grid, gear_r - 1, gear_c))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r - 1)
            {
                if (gear_c >= num.begin.c and
                    gear_c <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // down (r+1, c)
    if (is_in_bounds(grid, gear_r + 1, gear_c))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r + 1)
            {
                if (gear_c >= num.begin.c and
                    gear_c <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // left (r, c-1)
    if (is_in_bounds(grid, gear_r, gear_c - 1))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r)
            {
                if (gear_c - 1 >= num.begin.c and
                    gear_c - 1 <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // right (r, c+1)
    if (is_in_bounds(grid, gear_r, gear_c + 1))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r)
            {
                if (gear_c + 1 >= num.begin.c and
                    gear_c + 1 <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // up left (r-1, c-1)
    if (is_in_bounds(grid, gear_r - 1, gear_c - 1))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r - 1)
            {
                if (gear_c - 1 >= num.begin.c and
                    gear_c - 1 <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // up right (r-1, c+1)
    if (is_in_bounds(grid, gear_r - 1, gear_c + 1))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r - 1)
            {
                if (gear_c + 1 >= num.begin.c and
                    gear_c + 1 <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // down left (r+1, c-1)
    if (is_in_bounds(grid, gear_r + 1, gear_c - 1))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r + 1)
            {
                if (gear_c - 1 >= num.begin.c and
                    gear_c - 1 <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    // down right (r+1, c+1)
    if (is_in_bounds(grid, gear_r + 1, gear_c + 1))
    {
        for (const auto& num : nums)
        {
            if (num.begin.r == gear_r + 1)
            {
                if (gear_c + 1 >= num.begin.c and
                    gear_c + 1 <= num.end.c)
                {
                    num_set.insert(num);
                }
            }
        }
    }

    if (num_set.size() == 2)
    {
        i64 acc = 1;
        for (auto& num : num_set)
        {
            acc *= std::stoi(num.num);
        }

        return acc;
    }

    return 0;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    auto grid = Grid {};

    for (str line;
         std::getline(ifs, line);
         )
    {
        auto row = vec<char> {};

        for (char c : line)
        {
            row.push_back(c);
        }
        grid.push_back(std::move(row));
    }


    i64 acc = 0;
    auto rows = grid.size();
    auto cols = grid.at(0).size();

    for (i64 r = 0;
         r < rows;
         ++r)
    {
        // row: "467..114.."
        for (i64 c = 0;
             c < cols;
             ++c)
        {
            auto& col = grid.at(r);
            char ch = col.at(c);

            if (is_gear(ch))
            {
                vec<Num> nums;
                red_nums_in_row(grid, nums, r - 1);
                red_nums_in_row(grid, nums, r);
                red_nums_in_row(grid, nums, r + 1);

                i64 ratio = calculate_gear_ratio(grid, nums, r, c);
                acc += ratio;
            }

        }
    }

    i64 res = acc;
    cout << "part 2 (" << file_path << ") " << res << endl;
}

int main()
{

    try
    {
        part1();
        part2();
    }
    catch (str_cref e)
    {
        cout << "[ERROR] " << e << endl;
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
