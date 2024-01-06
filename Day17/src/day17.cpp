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


auto INF = std::numeric_limits<float>::infinity();

struct Vertex
{
    i64 x {-1};
    i64 y {-1};
    float dist {INF};
    Vertex* prev {nullptr};
};

bool operator==(const Vertex& a, const Vertex& b)
{
    return a.x == b.x and a.y == b.y;
}

//bool operator<(const Vertex& a, const Vertex& b)
//{
//    return a.dist < b.dist;
//}

auto UNDEFINED = Vertex {-1, -1};

template<typename T>
vec<Vertex> dijkstra(const Matrix<T>& graph,
                     const Vertex& source, const Vertex& target)
{
    struct Comparator
    {
        bool operator()(const Vertex* a, const Vertex* b) const
        {
            return a->dist < b->dist;
        }
    };

    auto neighbors = [](const Vertex* u,
                        const Matrix<T>& graph,
                        vec<Vertex>& vertices) -> vec<Vertex*>
    {
        vec<Vertex*> neighbors;
        const auto rows = graph.size();
        const auto cols = graph.at(0).size();

        if (u->x - 1 >= 0)
        {
            auto it = std::find(vertices.begin(),
                                vertices.end(),
                                Vertex(u->x - 1, u->y));
            neighbors.push_back(&(*it));
        }

        if (u->x + 1 < cols)
        {
            auto it = std::find(vertices.begin(),
                                vertices.end(),
                                Vertex(u->x + 1, u->y));
            neighbors.push_back(&(*it));
        }

        if (u->y - 1 >= 0)
        {
            auto it = std::find(vertices.begin(),
                                vertices.end(),
                                Vertex(u->x, u->y - 1));
            neighbors.push_back(&(*it));
        }

        if (u->y + 1 < rows)
        {
            auto it = std::find(vertices.begin(),
                                vertices.end(),
                                Vertex(u->x, u->y + 1));
            neighbors.push_back(&(*it));
        }

        return neighbors;
    };

    vec<Vertex> vertices;
    vertices.reserve((graph.size() * graph.at(0).size()) + 1);

    std::multiset<Vertex*, Comparator> Q;

    for (auto [y, row] : views::enumerate(graph))
    {
        for (auto [x, col] : views::enumerate(row))
        {
            vertices.emplace_back(x, y, INF, nullptr);
            Q.insert(&vertices.back());
        }
    }

    {
        auto it = std::find(vertices.begin(), vertices.end(), source);
        auto ex = Q.extract(&(*it));
        ex.value()->dist = 0.0f;
        Q.insert(std::move(ex));
    }

    while (not Q.empty())
    {
        // vertex in Q with min u.dist
        Vertex* u = *Q.begin();
        Q.erase(Q.begin());

        if (*u == target)
            break;

        for (Vertex* v : neighbors(u, graph, vertices))
        {
            if (Q.find(v) == Q.end())
            {
                // if v is not in Q, continue
                continue;
            }

            float alt = u->dist + graph.at(v->y).at(v->x);

            if (alt < v->dist)
            {
                auto it = std::find(vertices.begin(), vertices.end(), *v);
                auto ex = Q.extract(&(*it));
                ex.value()->dist = alt;
                ex.value()->prev = u;
                Q.insert(std::move(ex));
            }
        }

    }

    int s = 0;
    return vertices;
}

auto shortestPath(const vec<Vertex>& prev,
                  const Vertex& source,
                  const Vertex& target) -> std::stack<const Vertex*>
{
    std::stack<const Vertex*> S;

    const Vertex* u = &(*std::find(prev.begin(), prev.end(), target));
    const Vertex* src = &(*std::find(prev.begin(), prev.end(), source));
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

    auto vertices = dijkstra(heatmap, {0,0}, Vertex(cols - 1, rows - 1));
    auto shortest_path = shortestPath(vertices, {0,0}, Vertex(cols - 1, rows - 1));

    auto route = Matrix<char>(rows, vec<char>(cols, '.'));

    while (not shortest_path.empty())
    {
        auto& v = shortest_path.top();
        route.at(v->y).at(v->x) = '#';
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

    u64 acc = 0;
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
