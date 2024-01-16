// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <array>
#include <assert.h>
#include <functional>
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
        char buffer[512]{};
        if (strftime(buffer, sizeof(buffer), "%H:%M:%S", &local_time) != 0)
        {
            return { buffer };
        }
    }
    return "";
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


struct State
{
    i32 x{ 0 };
    i32 y{ 0 };
    i32 dx{ 0 };
    i32 dy{ 0 };
    i32 distance{ 0 };
};

struct Seen_Hash
{
    u64 operator()(const State& state) const
    {
        u64 seed = 0;
        std::hash<i32> hi32;

        // Combine the hash values of individual members
        seed ^= hi32(state.x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hi32(state.y) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hi32(state.dx) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hi32(state.dy) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hi32(state.distance) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};

bool operator==(const State& a, const State& b)
{
    bool res =
        a.x == b.x
        and a.y == b.y
        and a.dx == b.dx
        and a.dy == b.dy
        and a.distance == b.distance
        ;

    return res;
}

i32 width = 0;
i32 height = 0;
i32 end_x = 0;
i32 end_y = 0;
Matrix<u16> grid;

using Cost = i32;
std::unordered_map<Cost, vec<State>> state_by_cost;
std::unordered_map<State, Cost, Seen_Hash> cost_by_state;

const u32 limit_same_dir = 3;

void move_and_add_state(i32 cost, i32 x, i32 y, i32 dx, i32 dy, i32 distance)
{
    x += dx;
    y += dy;

    if (x < 0 or y < 0)
        return;
    if (x >= width or y >= height)
        return;

    auto new_cost = cost + grid[y][x];

    if (x == end_x and y == end_y)
    {
        cout << new_cost << endl;
        exit(1);
    }

    auto state = State(x, y, dx, dy, distance);

    if (not cost_by_state.contains(state))
    {
        state_by_cost[new_cost].push_back(state);
        cost_by_state[state] = new_cost;
    }

}

u64 part1()
{
    //
    // taken from:
    // reddit.com/r/adventofcode/comments/18luw6q/2023_day_17_a_longform_tutorial_on_day_17/
    //

    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    for (auto& line : split_string(input, "\n"))
    {
        grid.emplace_back();
        for (char ch : line)
        {
            grid.back().push_back(static_cast<u16>(ch - '0'));
        }
    }

    height = grid.size();
    width = grid[0].size();
    end_x =  width - 1;
    end_y =  height - 1;

    move_and_add_state(0, 0, 0, 1, 0, 1);
    move_and_add_state(0, 0, 0, 0, 1, 1);

    while (true)
    {
        auto current_cost = *ranges::min_element(state_by_cost | views::keys);
        auto next_states = state_by_cost.extract(current_cost).mapped();
        
        for (const auto& state : next_states)
        {
            // Perform the left and right turns
            move_and_add_state(current_cost, state.x, state.y, state.dy, -state.dx, 1);
            move_and_add_state(current_cost, state.x, state.y, -state.dy, state.dx, 1);

            if (state.distance < limit_same_dir)
            {
                move_and_add_state(current_cost, state.x, state.y, state.dx, state.dy, state.distance + 1);
            }
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
        println("day 17 part 1: {}", part1());
        println("day 17 part 2: {}", part2());

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
