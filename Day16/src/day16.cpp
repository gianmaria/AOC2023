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

template<typename T>
using Matrix = vec<vec<T>>;

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

enum class Direction: u32
{
    none, up, down, left, right
};

const char* to_str(Direction dir)
{
    switch (dir)
    {
        case Direction::up:
        {
            return "^";
        } break;

        case Direction::down:
        {
            return "v";
        } break;

        case Direction::left:
        {
            return "<";
        } break;

        case Direction::right:
        {
            return ">";
        } break;

        case Direction::none:
        {
            return "?";
        } break;

        default:
        return "null";
    }
}

//template<typename T>
//struct Vec2D
//{
//    union
//    {
//        T x {0};
//        T c;
//    };
//
//    union
//    {
//        T y {0};
//        T r;
//    };
//
//    bool operator<(const Vec2D& other) const
//    {
//        if (y == other.y)
//        {
//            return x < other.x;
//        }
//        else
//        {
//            return y < other.y;
//        }
//    }
//};



struct Tile
{
    u64 r = 0;
    u64 c = 0;
    Direction dir = Direction::none;
};

struct Tile_Comparator
{
    bool operator()(const Tile& a, const Tile& b) const
    {
        if (a.r == b.r)
            return a.c < b.c;
        else
            return a.r < b.r;
    }
};

bool operator<(const Tile& a, const Tile& b)
{
    if (a.r == b.r)
    {
        if (a.c == b.c)
        {
            // is direction is important? yes
            return a.dir < b.dir;
        }
        else
        {
            return a.c < b.c;
        }
    }
    else
    {
        return a.r < b.r;
    }
}

template<typename T>
void draw(const Matrix<T>& map, const Tile& tile)
{
    for (auto itr = map.begin();
         itr != map.end();
         ++itr)
    {
        u64 r = std::distance(map.begin(), itr);

        for (auto itc = map.at(r).begin();
             itc != map.at(r).end();
             ++itc)
        {
            u64 c = std::distance(map.at(r).begin(), itc);

            if (r == tile.r and c == tile.c)
            {
                cout << to_str(tile.dir);
            }
            else
            {
                cout << *itc;
            }
        }
        cout << endl;
    }
    cout << endl;
}

void advance(Tile& tile, char ch)
{
    switch (tile.dir)
    {
        case Direction::up:
        {
            if (ch == '.' or ch == '|')
            {
                tile.r -= 1;
            }
            else if (ch == '/')
            {
                tile.dir = Direction::right;
                tile.c += 1;
            }
            else if (ch == '\\')
            {
                tile.dir = Direction::left;
                tile.c -= 1;
            }
            else
            {
                throw std::format("Unknown char <{}>", ch);
            }
        } break;

        case Direction::down:
        {
            if (ch == '.' or ch == '|')
            {
                tile.r += 1;
            }
            else if (ch == '/')
            {
                tile.dir = Direction::left;
                tile.c -= 1;
            }
            else if (ch == '\\')
            {
                tile.dir = Direction::right;
                tile.c += 1;
            }
            else
            {
                throw std::format("Unknown char <{}>", ch);
            }
        } break;

        case Direction::left:
        {
            if (ch == '.' or ch == '-')
            {
                tile.c -= 1;
            }
            else if (ch == '/')
            {
                tile.dir = Direction::down;
                tile.r += 1;
            }
            else if (ch == '\\')
            {
                tile.dir = Direction::up;
                tile.r -= 1;
            }
            else
            {
                throw std::format("Unknown char <{}>", ch);
            }
        }
        break;

        case Direction::right:
        {
            if (ch == '.' or ch == '-')
            {
                tile.c += 1;
            }
            else if (ch == '/')
            {
                tile.dir = Direction::up;
                tile.r -= 1;
            }
            else if (ch == '\\')
            {
                tile.dir = Direction::down;
                tile.r += 1;
            }
            else
            {
                throw std::format("Unknown char <{}>", ch);
            }
        }
        break;

        case Direction::none:
        {
            int s = 0;
        } break;
    }
}

template<typename T>
u64 walk(const Matrix<T>& map, Tile start)
{
    auto in_bounds = [](const Matrix<T>& map, Tile tile)
    {
        bool res =
            (tile.r < map.size())
            and
            (tile.c < map.at(0).size());

        return res;
    };

    std::queue<Tile> to_visit;
    to_visit.push(start);

    std::set<Tile> visited;

    while (not to_visit.empty())
    {
        Tile tile = to_visit.front();
        to_visit.pop();

        while (in_bounds(map, tile))
        {
            if (visited.contains(tile))
                break;

            visited.insert(tile);

            char ch = map.at(tile.r).at(tile.c);

            //cout << std::format("[{}] ({},{}) '{}' {}",
            //                    visited.size(),
            //                    tile.r + 1, tile.c + 1,
            //                    ch, to_str(tile.dir))
            //    << endl;

            if (ch == '|'
                and
                (tile.dir == Direction::left or tile.dir == Direction::right))
            {
                // split
                Tile opposite {tile.r + 1, tile.c, Direction::down};
                to_visit.push(opposite);

                tile.r -= 1;
                tile.dir = Direction::up;
            }
            else if (ch == '-'
                     and
                     (tile.dir == Direction::down or tile.dir == Direction::up))
            {
                // split
                Tile opposite {tile.r, tile.c - 1, Direction::left};
                to_visit.push(opposite);

                tile.c += 1;
                tile.dir = Direction::right;
            }
            else
            {
                advance(tile, ch);
            }
        }

        //cout << endl;
    }

    // visited contains dulicate tiles with different direction!
    // we need to get rid of those, hence the different comparator
    return set<Tile, Tile_Comparator>(visited.begin(), visited.end()).size();
}

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Matrix<char> map;

    for (str line;
         std::getline(ifs, line);
         )
    {
        map.emplace_back(vec<char>{line.begin(), line.end()});
    }

    u64 res = walk(map, {0, 0, Direction::right});
    return res;
}

u64 part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Matrix<char> map;

    for (str line;
         std::getline(ifs, line);
         )
    {
        map.emplace_back(vec<char>{line.begin(), line.end()});
    }

    u64 res = 0;
    u64 rows = map.size();
    u64 cols = map.at(0).size();

    // top row, going down
    for (u64 c = 0; c < cols; ++c)
    {
        auto count = walk(map, {0, c, Direction::down});
        res = std::max(res, count);
    }

    // bottom row, going up
    for (u64 c = 0; c < cols; ++c)
    {
        auto count = walk(map, {rows - 1, c, Direction::up});
        res = std::max(res, count);
    }

    // leftmost column, going right
    for (u64 r = 0; r < rows; ++r)
    {
        auto count = walk(map, {r, 0, Direction::right});
        res = std::max(res, count);
    }

    // rightmost column, going left
    for (u64 r = 0; r < rows; ++r)
    {
        auto count = walk(map, {r, cols - 1, Direction::left});
        res = std::max(res, count);
    }

    return res;
}

int main()
{
    try
    {
        auto p1 = part1();
        cout << "day 16 part 1: " << p1 << endl;

        auto p2 = part2();
        cout << "day 16 part 2: " << p2 << endl;
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
