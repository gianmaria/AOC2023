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

Point inverse(const Point& p)
{
    return {p.c, p.r};
}

float weight(const Image& image,
             const Point from, const Point to)
{
    auto dist_row = std::abs(from.r - to.r);
    auto dist_col = std::abs(from.c - to.c);
    if (dist_row <= 1 and dist_col <= 1)
    {
        //if (to == Point {2 - 1,10 - 1})
        //{
        //    int s = 0;
        //}
        auto weight = image.at(to.r).at(to.c);

        if (weight == 0)
            return 1;
        else
            return weight;
    }
    else
    {
        return Inf;
    }
}

bool valid(const Image& image, const Point& p)
{
    const auto& rows = image.size();
    const auto& cols = image.at(0).size();

    bool res =
        p.r >= 0 and p.r < rows
        and
        p.c >= 0 and p.c < cols;

    return res;
}

map<Point, float> dijkstra(const Image& image, const Point& start)
{
    map<Point, float> dist;
    map<Point, Point> prev;
    vec<Point> Q;
    std::set<Point> S;
    Point Undef {-1,-1};

    for (const auto& [r, row] : std::views::enumerate(image))
    {
        for (const auto& [c, value] : std::views::enumerate(row))
        {
            Point v {r,c};

            dist[v] = Inf;
            prev[v] = Undef;
            Q.push_back(v);
        }
    }

    dist[start] = 0.0f;

    while (not Q.empty())
    {
        auto it = std::min_element(Q.begin(), Q.end(),
                                   [&dist](const Point& a, const Point& b)
        {
            return dist[a] < dist[b];
        });

        Point u = *it;

        std::swap(Q[std::distance(Q.begin(), it)], Q[Q.size() - 1]);
        Q.erase(Q.end() - 1);

        for (const Point& p : vec<Point> {{-1,0},{+1,0},{0,-1},{0,+1}})
        {
            auto v = Point {u.r + p.r, u.c + p.c};
            if (not valid(image, v))
                continue;

            float alt = dist[u] + weight(image, u, v);

            if (alt < dist[v])
            {
                dist[v] = alt;
                prev[v] = u;
            }
        }
    }

    return dist;

#if 0
    for (const auto& [r, row] : std::views::enumerate(image))
    {
        for (const auto& [c, value] : std::views::enumerate(row))
        {
            distance.insert({{r, c}, Inf});
        }
    }

    distance[start] = 0.0f;

    Q.push({start, 0});

    while (not Q.empty())
    {
        auto u = Q.top().first;
        Q.pop();

        if (S.count(u) != 0)
            continue;

        S.insert(u);

        // for each neighbor v of u:
        for (const Point& p : vec<Point> {{-1,0},{+1,0},{0,-1},{0,+1}})
        {
            auto v = Point {u.r + p.r, u.c + p.c};
            if (not valid(image, v))
                continue;

            if (u == Point {2 - 1,10 - 1})
            {
                int s = 0;
            }

            if (distance[u] == Inf)
                continue;

            float alt = distance[u] + weight(image, u, v);

            if (alt < distance[v])
            {
                distance[v] = alt;
                Q.push({v, alt});
            }
        }
    }

    return distance;
#endif // 0

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

float solve(const Image& image)
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

    map<pair<u16, u16>, float> res;

    for (const Point& galaxy : galaxies)
    {
        auto distances = dijkstra(image, galaxy);

        for (const Point& other_galaxy : galaxies)
        {
            if (other_galaxy == galaxy)
                continue;

            float dist = distances[other_galaxy];
            dist -= (image[other_galaxy.r][other_galaxy.c] - 1.0f);

            auto from = image[galaxy.r][galaxy.c];
            auto to = image[other_galaxy.r][other_galaxy.c];

            /*cout << std::format("from galaxy {} ({},{}) to {} ({},{}) length {}",
                                from, galaxy.r + 1, galaxy.c + 1,
                                to, other_galaxy.r + 1, other_galaxy.c + 1,
                                dist)
                << endl;*/

            auto from_to = std::make_pair(from, to);
            auto to_from = std::make_pair(to, from);

            if (
                (res.find(from_to) == res.end()) 
                and
                (res.find(to_from) == res.end())
                )
            {
                res.insert({from_to, dist});
            }
        }
        //cout << endl;
    }

#if 0
    //for (const Point& galaxy : galaxies)
//{
//    // adjust weights
//    for (auto& [p, val] : distances)
//    {
//        auto it = std::find(galaxies.begin(), galaxies.end(), p);

//        if (it != galaxies.end()
//            /*and
//            p.r != galaxy.r and p.c != galaxy.c*/)
//        {
//            float dist = distances[p];
//            dist -= (image[p.r][p.c] - 1.0f);

//            cout << std::format("from galaxy {} ({},{}) to {} ({},{}) length {}",
//                                image[galaxy.r][galaxy.c], galaxy.r + 1, galaxy.c + 1,
//                                image[p.r][p.c], p.r + 1, p.c + 1,
//                                dist)
//                << endl;
//            res += dist;
//        }
//    }
//    cout << endl;
//    int s = 0;
//}  
#endif // 0


#if 0

    for (const auto& [r, row] : std::views::enumerate(image))
    {
        for (const auto& [c, value] : std::views::enumerate(row))
        {
            if (value != 0)
            {
                auto distance = dijkstra(image, Point {r,c});

                for (auto& pair : distance)
                {
                    if (galaxies.find(pair.first) != galaxies.end() and
                        pair.first.r != r and
                        pair.first.c != c)
                    {
                        distance[pair.first] += -image[pair.first.r][pair.first.c] + 1;
                    }
                }

                for (auto& [key, val] : galaxies)
                {
                    if (key.r != r and
                        key.c != c)
                    {
                        galaxies[key] = distance[key];
                    }
                }
                int s = 0;
            }
        }
    }

#endif // 0

    return std::accumulate(res.begin(), res.end(), 0.0f,
        [](float acc, const auto& pair) {
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
