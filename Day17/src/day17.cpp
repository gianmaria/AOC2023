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


enum class Direction
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
    i64 r {-1};
    i64 c {-1};
};

auto INF = std::numeric_limits<float>::infinity();
auto UNDEF = Vertex();

bool operator==(const Vertex& a, const Vertex& b)
{
    return (a.c == b.c) and (a.r == b.r);
}


struct Vertex_Comparator
{
    bool operator()(const Vertex& a, const Vertex& b) const
    {
        if (a.r == b.r)
            return a.c < b.c;
        else
            return a.r < b.r;
    }
};

struct Q_Comparator
{
    bool operator()(const pair<Vertex, float>& a, const pair<Vertex, float>& b) const
    {
        return a.second > b.second;
    }
};

template<typename T>
auto neighbor(const Matrix<T>& graph, Vertex u, 
              Direction dir)
{
    vec<Vertex> res;
    const auto rows = graph.size();
    const auto cols = graph.at(0).size();

    // up
    if (u.r - 1 >= 0 and
        dir != Direction::down)
        res.emplace_back(u.r - 1, u.c);

    // down
    if (u.r + 1 < rows and
        dir != Direction::up)
        res.emplace_back(u.r + 1, u.c);

    // left
    if (u.c - 1 >= 0 and
        dir != Direction::right)
        res.emplace_back(u.r, u.c - 1);

    // right
    if (u.c + 1 < cols and
        dir != Direction::left)
        res.emplace_back(u.r, u.c + 1);

    return res;
}

template<typename T>
auto dijkstra(const Matrix<T>& graph,
              Vertex source, Vertex target)
{
    auto calc_dir = [](Vertex u, Vertex v)
    {
        auto direction = Direction::none;

        if (u.c < v.c)
            direction = Direction::right;
        else if (u.c > v.c)
            direction = Direction::left;
        else if (u.r < v.r)
            direction = Direction::down;
        else if (u.r > v.r)
            direction = Direction::up;
        else
            throw "where do we moved??";

        return direction;
    };

    map<Vertex, float, Vertex_Comparator> dist;
    map<Vertex, Vertex, Vertex_Comparator> prev;
    std::priority_queue<pair<Vertex, float>, vec<pair<Vertex, float>>, Q_Comparator> Q;
    
    std::multimap<Vertex, Direction, Vertex_Comparator> dir;

    for (i64 r = 0;
         r < graph.size();
         ++r)
    {
        for (i64 c = 0;
             c < graph[0].size();
             ++c)
        {
            dist[{r, c}] = INF;
            prev[{r, c}] = UNDEF;
        }
    }

    dist[source] = 0.0f;
    dir.emplace(source, Direction::none);

    Q.push({source, 0.0f});

    while (not Q.empty())
    {
        Vertex u = Q.top().first;
        Q.pop();

        if (u == target)
            break;

        auto range = dir.equal_range(u);

        for (Vertex v : neighbor(graph, u, range.first->second))
        {
            float alt = dist[u] + graph[v.r][v.c];

            if (alt < dist[v])
            {
                dist[v] = alt;
                prev[v] = u;
                dir.emplace(v, calc_dir(u, v));

                Q.push({v, alt});
            }
        }
    }


    return std::make_pair(dist, prev);
}

auto get_shortest_path(map<Vertex, Vertex, Vertex_Comparator>& prev,
                       Vertex source, Vertex target)
{
    std::stack<Vertex> S;
    Vertex u = target;

    if (prev[u] != UNDEF or
        u == source)
    {
        while (u != UNDEF)
        {
            S.push(u);
            u = prev[u];
        }
    }

    return S;
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

    auto source = Vertex(0, 0);
    auto target = Vertex(rows - 1, cols - 1);

    auto [dist, prev] = dijkstra(heatmap, source, target);
    auto shortest_path = get_shortest_path(prev, source, target);

    auto map = Matrix<char>(rows, vec<char>(cols, '.'));
    while (not shortest_path.empty())
    {
        auto v = shortest_path.top();
        shortest_path.pop();
        map[v.r][v.c] = '#';
    }

    print_matrix(map);

    u64 res = dist[target];
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
