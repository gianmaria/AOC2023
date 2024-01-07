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
    none, left, right, up, down
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

auto INF = std::numeric_limits<float>::infinity();
struct Vertex
{
    i64 r {-1};
    i64 c {-1};
    float dist {INF};
    Vertex* prev {nullptr};
    Direction dir {Direction::none};

};

bool operator==(const Vertex& a, const Vertex& b)
{
    return (a.c == b.c) and (a.r == b.r);
}




template<typename T>
void dijkstra(const Matrix<T>& graph,
              vec<Vertex>& vertices,
              Vertex* source, Vertex* target)
{

    auto get_neighbors_still_in_Q = [&graph](vec<Vertex*>& Q,
                                             const Vertex* u, Direction from) -> vec<Vertex*>
    {
        vec<Vertex*> neighbors;
        const auto rows = graph.size();
        const auto cols = graph.at(0).size();

        // left
        if (u->c - 1 >= 0
            and
            from != Direction::right)
        {
            auto target = Vertex(u->r, u->c - 1);
            auto it = ranges::find_if(Q, [&target](const Vertex* elem)
            {
                return target == *elem;
            });
            if (it != Q.end())
                neighbors.push_back(*it);
        }

        // right
        if (u->c + 1 < cols
            and
            from != Direction::left)
        {
            auto target = Vertex(u->r, u->c + 1);
            auto it = ranges::find_if(Q, [&target](const Vertex* elem)
            {
                return target == *elem;
            });
            if (it != Q.end())
                neighbors.push_back(*it);
        }

        // up
        if (u->r - 1 >= 0
            and
            from != Direction::down)
        {
            auto target = Vertex(u->r - 1, u->c);
            auto it = ranges::find_if(Q, [&target](const Vertex* elem)
            {
                return target == *elem;
            });
            if (it != Q.end())
                neighbors.push_back(*it);
        }

        // down
        if (u->r + 1 < rows
            and
            from != Direction::up)
        {
            auto target = Vertex(u->r + 1, u->c);
            auto it = ranges::find_if(Q, [&target](const Vertex* elem)
            {
                return target == *elem;
            });
            if (it != Q.end())
                neighbors.push_back(*it);
        }

        return neighbors;
    };

    auto min_dist_vertex = [](const Vertex* a, const Vertex* b)
    {
        return a->dist < b->dist;
    };

    vec<Vertex*> Q;
    Q.reserve(vertices.size());

    for (Vertex& vertex : vertices)
    {
        Q.push_back(&vertex);
    }

    source->dist = 0.0f;

    auto direction = Direction::none;
    auto prev_direction = Direction::none;
    u32 blocks = 0;
    while (Q.size() > 0)
    {
        // vertex in Q with min u.dist
        auto it = ranges::min_element(Q, min_dist_vertex);

        Vertex* u = *it;

        std::swap(*it, *(Q.end() - 1));
        Q.pop_back();

        if (*u == *target)
            break;
        
        //println("u is ({}, {}) dist: {}", u->r, u->c, u->dist);

        auto neighbors = get_neighbors_still_in_Q(Q, u, direction);
        for (Vertex* v : neighbors)
        {
            float alt = u->dist + graph.at(v->r).at(v->c);

            //print("  checking v ({}, {})", v->r, v->c);

            if (alt < v->dist)
            {
                prev_direction = direction;

                if (u->c < v->c)
                    direction = Direction::right;
                else if (u->c > v->c)
                    direction = Direction::left;
                else if (u->r < v->r)
                    direction = Direction::down;
                else if (u->r > v->r)
                    direction = Direction::up;
                else
                    int s = 0;

                //print(" updating dist to {}, direction {}", alt, to_str(direction));

                v->dist = alt;
                v->prev = u;
                v->dir = direction;
            }

            //println("");
        }

        int s = 0;
        /*if (blocks < 3)
        {
            v->dist = alt;
            v->prev.push_back(u);
            v->dir = direction;

            if (v->prev.size() > 1)
            {
                int s = 0;
            }
        }*/

    }

    int s = 0;
}

auto find_shortest_path(const vec<Vertex>& prev,
                        const Vertex* source,
                        const Vertex* target) -> std::stack<const Vertex*>
{
    std::stack<const Vertex*> S;

    auto it_src = ranges::find_if(prev, [&](const Vertex& elem)
    {
        return *source == elem;
    });
    const Vertex* src = &(*it_src);

    auto it_u = ranges::find_if(prev, [&](const Vertex& elem)
    {
        return *target == elem;
    });
    const Vertex* u = &(*it_u);

    if (u->prev != nullptr
        or
        *u == *src)
    {
        while (u != nullptr)
        {
            S.push(u);
            u = u->prev;
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
    const auto cols = heatmap.at(0).size();

    vec<Vertex> vertices;
    vertices.reserve(rows * cols);
    for (auto [r, row] : views::enumerate(heatmap))
    {
        for (auto [c, col] : views::enumerate(row))
        {
            vertices.emplace_back(r, c, INF, nullptr, Direction::none);
        }
    }

    Vertex* source = &vertices.front();
    Vertex* target = &vertices.back();
    println("{}", get_time());
    dijkstra(heatmap, vertices, source, target);
    println("{}", get_time());

    auto shortest_path = find_shortest_path(vertices, source, target);

    auto route = Matrix<char>(rows, vec<char>(cols, '.'));

    float heat_loss = 0.0f;
    while (not shortest_path.empty())
    {
        auto v = shortest_path.top();
        heat_loss = v->dist;
        //route.at(v->r).at(v->c) = '#';
        switch (v->dir)
        {
            case Direction::up:
            route.at(v->r).at(v->c) = '^';
            break;
            case Direction::down:
            route.at(v->r).at(v->c) = 'v';
            break;
            case Direction::left:
            route.at(v->r).at(v->c) = '<';
            break;
            case Direction::right:
            route.at(v->r).at(v->c) = '>';
            break;
            default:
            route.at(v->r).at(v->c) = '#';
        }
        shortest_path.pop();
    }

    for (const auto& row : route)
    {
        for (const char ch : row)
        {
            cout << ch;
        }
        cout << endl;
    }
    cout << endl;

    u64 acc = heat_loss;
    u64 res = acc;
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
