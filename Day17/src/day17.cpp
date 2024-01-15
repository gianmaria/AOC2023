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


enum class Direction: u32
{
    up, down, left, right, none
};

const char* to_str(Direction dir)
{
    switch (dir)
    {
        case Direction::none: return "none";
        case Direction::left: return "left";
        case Direction::right: return "right";
        case Direction::up: return "up";
        case Direction::down: return "down";
        default: return "???";
    }
}

struct Vertex
{
    i32 r{ -1 };
    i32 c{ -1 };
};

bool operator==(const Vertex& a, const Vertex& b)
{
    return (a.c == b.c) and (a.r == b.r);
}

struct State
{
    Vertex pos{};
    Direction dir{ Direction::none };
    u32 same_dir_count{ 0 };
};

Vertex operator+(const Vertex& v, const Direction& dir)
{
    // TODO:
    switch (dir)
    {
        case Direction::up:
        return { v.r - 1, v.c };

        case Direction::down:
        return { v.r + 1, v.c };

        case Direction::left:
        return { v.r, v.c - 1 };

        case Direction::right:
        return { v.r, v.c + 1 };

        case Direction::none:
        return v;

        default:
        {
            throw "Impossible direction!";
        }
    }

}

struct Seen_Hash
{
    u64 operator()(const State& state) const
    {
        u64 seed = 0;
        std::hash<i32> hi32;
        std::hash<u32> hu32;

        // Combine the hash values of individual members
        seed ^= hi32(state.pos.r) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hi32(state.pos.c) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        //seed ^= hu32(state.heat_loss) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hu32(static_cast<u32>(state.dir)) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hu32(state.same_dir_count) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

        return seed;
    }
};

bool operator==(const State& a, const State& b)
{
    bool res =
        a.pos == b.pos
        //and a.heat_loss == b.heat_loss
        and a.dir == b.dir
        and a.same_dir_count == b.same_dir_count
        ;

    return res;
}

struct Q_Comparator
{
    bool operator()(const State& a, const State& b) const
    {
        return a.heat_loss > b.heat_loss;
    }
};

template<typename T>
auto dijkstra(const Matrix<T>& graph, Vertex target)
{
    auto calc_directions = [&graph](State state)
    {
        const auto rows = graph.size();
        const auto cols = graph.at(0).size();

        vec<Direction> directions;

        // up
        if (state.dir != Direction::down and
            state.pos.r - 1 >= 0)
        {
            directions.emplace_back(Direction::up);
        }

        // down
        if (state.dir != Direction::up and
            state.pos.r + 1 < rows)
        {
            directions.emplace_back(Direction::down);
        }

        // left
        if (state.dir != Direction::right and
            state.pos.c - 1 >= 0)
        {
            directions.emplace_back(Direction::left);
        }

        // right
        if (state.dir != Direction::left and
            state.pos.c + 1 < cols)
        {
            directions.emplace_back(Direction::right);
        }

        return directions;
    };

    const u32 limit_same_dir = 3;
    std::priority_queue<State, vec<State>, Q_Comparator> Q;
    std::unordered_map<State, bool, Seen_Hash> seen;

    Q.push({ {0,0}, 0, Direction::none, 0 });

    while (not Q.empty())
    {
        auto state = Q.top();
        Q.pop();

        println("checking state ({},{}) {} - cost: {}",
              state.pos.r, state.pos.c, 
              state.same_dir_count, state.heat_loss);

        for (auto new_dir : calc_directions(state))
        {
            auto new_pos = state.pos + new_dir;
            auto new_cost = state.heat_loss + graph[new_pos.r][new_pos.c];

            if (new_pos == target)
            {
                return new_cost;
            }

            auto new_state = State(new_pos, new_cost, new_dir, 1);

            if (new_state.dir == state.dir)
            {
                new_state.same_dir_count = state.same_dir_count + 1;
            }
            
            /*println("  neighbour ({},{}) {} {} - {}",
                    new_state.pos.r, new_state.pos.c,
                    to_str(new_state.dir), new_state.same_dir_count,
                    new_state.heat_loss);*/
            
            if (state.same_dir_count < limit_same_dir)
            {
                if (not seen.contains(new_state))
                {
                    seen.insert({new_state, true});
                    Q.push(new_state);
                }
                else
                {
                    // TODO: mi serve il costo associato allo stato
                    // già presente nella mappa!
                    auto ss = seen[new_state];
                    
                }
            }

        }
        
    }

    return 0u;
}

u64 part1()
{
    auto file_path = "res\\test.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    Matrix<u16> heatmap;
    for (auto& line : split_string(input, "\n"))
    {
        heatmap.emplace_back(vec<u16>{});
        for (char ch : line)
        {
            heatmap.back().push_back(static_cast<u16>(ch - '0'));
        }
    }

    const auto rows = heatmap.size();
    const auto cols = heatmap[0].size();

    auto target = Vertex(rows - 1, cols - 1);

    auto heat_loss = dijkstra(heatmap, target);

    u64 res = heat_loss;
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
        println("day 17 part 1: {}", part1());
        //println("day 17 part 2: {}", part2());

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
