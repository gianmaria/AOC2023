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
    vector<str> res{};
    std::istringstream iss{ line };

    str token{};

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

bool is_in_bounds(Grid& grid, i32 r, i32 c)
{
    auto rows = static_cast<i32>(grid.size());
    auto cols = static_cast<i32>(grid.at(0).size());

    bool res =
        (r >= 0 and r < rows)
        and
        (c >= 0 and c < cols);

    return res;
}

bool is_digit_adjacent_to_symbol(Grid& grid, i32 r, i32 c)
{
    bool res = false;

    for (i32 rr = -1; rr <= 1; ++rr)
    {
        for (i32 cc = -1; cc <= 1; ++cc)
        {
            if (is_in_bounds(grid, r + rr, c + cc))
            {
                res |= is_symbol(grid.at(r +rr).at(c+cc));
            }
        }
    }
    
    return res;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    auto grid = vec<vec<char>>{};

    for (str line;
         std::getline(ifs, line);
         )
    {
        auto row = vec<char>{};

        for (char c : line)
        {
            row.push_back(c);
        }
        grid.push_back(std::move(row));
    }


    i32 acc = 0;
    auto rows = static_cast<i32>(grid.size());
    auto cols = static_cast<i32>(grid.at(0).size());

    for (i32 r = 0;
         r < rows;
         ++r)
    {
        // row: "467..114.."
        for (i32 c = 0;
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
                for (i32 i = c - num.size();
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


struct Num
{
    str num;
    i32 row{};
    i32 start_col{};
    i32 end_col{};
};

struct Num_Comp
{
    bool operator()(const Num& lhs, const Num& rhs) const
    {
        if (lhs.row == rhs.row)
        {
            return lhs.start_col < rhs.start_col;
        }
        else
        {
            return lhs.row < rhs.row;
        }
    }
};

void read_nums_in_row(Grid& grid, vec<Num>& nums, i32 r)
{
    if (r < 0)
        return;
    if (r > grid.size())
        return;

    auto& col = grid.at(r);
    auto cols = col.size();

    for (i32 c = 0;
         c < cols;
         ++c)
    {
        auto ch = col.at(c);

        if (std::isdigit(ch))
        {
            Num num;
            num.row = r;

            num.start_col = c;

            while (c < cols and
                   std::isdigit(col.at(c)))
            {
                num.num.push_back(col.at(c));
                ++c;
            }

            num.end_col = c - 1;

            nums.push_back(std::move(num));
        }
    }

    int s = 0;
}

bool is_between(i32 num, i32 min, i32 max)
{
    return (num >= min) and (num <= max);
}

bool is_suitable(const Num& num, i32 gear_r, i32 gear_c)
{
    return (num.row == gear_r) and
        is_between(gear_c, num.start_col, num.end_col);
}

i32 calculate_gear_ratio(Grid& grid, const vec<Num>& nums, i32 gear_r, i32 gear_c)
{
    set<Num, Num_Comp> num_set;

    for (i32 r = -1; r <= 1; ++r)
    {
        for (i32 c = -1; c <= 1; ++c)
        {
            if (not is_in_bounds(grid, gear_r + r, gear_c + c))
                continue;

            for (const auto& num : nums)
            {
                if (is_suitable(num, gear_r + r, gear_c + c))
                {
                    num_set.insert(num);
                }
            }
        }
    }

    if (num_set.size() == 2)
    {
        i32 acc = std::stoi(num_set.begin()->num);
        acc *= std::stoi(num_set.rbegin()->num);

        return acc;
    }

    return 0;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    auto grid = Grid{};

    for (str line;
         std::getline(ifs, line);
         )
    {
        auto row = vec<char>{};

        for (char c : line)
        {
            row.push_back(c);
        }
        grid.push_back(std::move(row));
    }


    i32 acc = 0;
    auto rows = static_cast<i32>(grid.size());
    auto cols = static_cast<i32>(grid.at(0).size());

    for (i32 r = 0;
         r < rows;
         ++r)
    {
        // row: "467..114.."
        for (i32 c = 0;
             c < cols;
             ++c)
        {
            auto& col = grid.at(r);
            char ch = col.at(c);

            if (is_gear(ch))
            {
                vec<Num> nums;
                read_nums_in_row(grid, nums, r - 1);
                read_nums_in_row(grid, nums, r);
                read_nums_in_row(grid, nums, r + 1);

                i32 ratio = calculate_gear_ratio(grid, nums, r, c);
                acc += ratio;
            }

        }
    }

    i32 res = acc;
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
