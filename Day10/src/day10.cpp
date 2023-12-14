// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <array>
#include <assert.h>
#include <cstdint>
#include <exception>
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
using std::ranges::find_if;

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

enum class Direction
{
    none, up, down, left, right
};

std::ostream& operator<<(std::ostream& os, Direction value)
{
    switch (value)
    {
        case Direction::up:
        {
            os << "up";
        } break;

        case Direction::down:
        {
            os << "down";
        } break;

        case Direction::left:
        {
            os << "left";
        } break;

        case Direction::right:
        {
            os << "right";
        } break;

        case Direction::none:
        {
            os << "!none!";
        } break;
    }

    return os;
}

Direction opposite(Direction dir)
{
    switch (dir)
    {
        case Direction::up:
        return Direction::down;

        case Direction::down:
        return Direction::up;

        case Direction::left:
        return Direction::right;

        case Direction::right:
        return Direction::left;

        case Direction::none:
        throw "Direction none has no opposite!";
    }

    return Direction::left;
}

struct Tile
{
    char name {};

    i32 r {};
    i32 c {};

    Direction conn1 {};
    Direction conn2 {};

    bool part_of_loop {false};
};

using Maze = vec<vec<Tile>>;

bool operator==(const Tile& t, char ch)
{
    return t.name == ch;
}

bool operator!=(const Tile& t, char ch)
{
    return t.name != ch;
}


Tile* can_move(Maze& maze, const Tile& tile,
               Direction dir, bool start_tile = false)
{
    Tile* next_tile {nullptr};

    switch (dir)
    {
        case Direction::up:
        {
            if (tile.r - 1 >= 0)
            {
                next_tile = &maze.at(tile.r - 1).at(tile.c);
            }
        } break;

        case Direction::down:
        {
            if (tile.r + 1 < maze.size()) // assumed maze is always sqare
            {
                next_tile = &maze.at(tile.r + 1).at(tile.c);
            }
        } break;

        case Direction::left:
        {
            if (tile.c - 1 >= 0)
            {
                next_tile = &maze.at(tile.r).at(tile.c - 1);
            }
        } break;

        case Direction::right:
        {
            if (tile.c + 1 < maze.at(0).size()) // assumed maze is always sqare
            {
                next_tile = &maze.at(tile.r).at(tile.c + 1);
            }
        } break;

        case Direction::none:
        {
            next_tile = &maze.at(tile.r).at(tile.c);
        } break;

    }

    if (next_tile)
    {
        bool res = false;

        if (start_tile)
        {
            res =
                next_tile->conn1 == opposite(dir)
                or
                next_tile->conn2 == opposite(dir);
        }
        else
        {
            res =
                (next_tile->conn1 == opposite(dir) or next_tile->conn2 == opposite(dir))
                and
                (tile.conn1 == dir or tile.conn2 == dir);
        }

        if (not res)
        {
            next_tile = nullptr;
        }
    }

    return next_tile;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Maze maze;
    i32 tile_start_r {0};
    i32 tile_start_c {0};

    i32 r = 0;
    for (str line;
         std::getline(ifs, line);
         )
    {
        vec<Tile> row;
        i32 c = 0;

        for (char ch : line)
        {
            Tile tile;

            tile.name = ch;

            tile.r = r;
            tile.c = c;

            switch (ch)
            {
                case '|':
                {
                    tile.conn1 = Direction::up;
                    tile.conn2 = Direction::down;
                } break;

                case '-':
                {
                    tile.conn1 = Direction::left;
                    tile.conn2 = Direction::right;
                } break;

                case 'L':
                {
                    tile.conn1 = Direction::up;
                    tile.conn2 = Direction::right;
                } break;

                case 'J':
                {
                    tile.conn1 = Direction::up;
                    tile.conn2 = Direction::left;
                } break;

                case '7':
                {
                    tile.conn1 = Direction::down;
                    tile.conn2 = Direction::left;
                } break;

                case 'F':
                {
                    tile.conn1 = Direction::down;
                    tile.conn2 = Direction::right;
                } break;

                case '.':
                {
                    tile.conn1 = Direction::none;
                    tile.conn2 = Direction::none;
                } break;

                case 'S':
                {
                    tile.conn1 = Direction::none;
                    tile.conn2 = Direction::none;

                    tile_start_r = r;
                    tile_start_c = c;
                } break;

                default:
                throw std::format("Unknown cell <{} ({},{})>",
                                  ch, r, c);
            }

            ++c;
            row.push_back(std::move(tile));
        }

        ++r;

        maze.push_back(std::move(row));
    }

    Tile* start = &maze.at(tile_start_r).at(tile_start_c);

    // setup connection for start tile
    for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right})
    {
        if (can_move(maze, *start, dir, true))
        {
            if (start->conn1 == Direction::none)
                start->conn1 = dir;
            else if (start->conn2 == Direction::none)
                start->conn2 = dir;
            else
                throw "??????";
        }
    }

    const Tile* curr_tile = start;
    Direction from = Direction::none;
    u32 steps = 0;

    do
    {
#ifdef SANITY_CHECK
        i32 movs = 0;
        cout << "tile " << curr_tile.name << " [";
        for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        {
            if (dir != from and
                can_move(maze, curr_tile, dir))
            {
                //cout << dir << ", ";
                ++movs;
            }
        }
        cout << "] (" << movs << ")" << endl;

        if (movs > 1)
        {
            int s = 0;
        }

        bool moved = false;
#endif // SANITY_CHECK

        for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        {
            if (dir != from)
            {
                if (auto* new_tile = can_move(maze, *curr_tile, dir);
                    new_tile != nullptr)
                {
                    curr_tile = new_tile;
                    from = opposite(dir); // keep track of where you come from
                    ++steps;
#ifdef SANITY_CHECK
                    moved = true;
#endif // SANITY_CHECK
                    break;
                }
            }
        }

#ifdef SANITY_CHECK
        if (not moved)
        {
            // stuck? 
            int s = 0;
            throw "i'm stuck!";
        }
#endif // SANITY_CHECK


    } while (curr_tile->name != 'S');

    if (steps % 2 != 0)
        throw std::format("steps is odd! <{}>", steps);

    auto res = steps / 2;
    cout << "day 10 part 1 (" << file_path << ") " << res << endl;
}

void draw_maze(const Maze& maze)
{
    ranges::for_each(maze, [](const auto& row) {
        ranges::for_each(row, [](const Tile& tile) {
            if (tile.part_of_loop)
                cout << tile.name;
            else if (str("I").contains(tile.name))
                cout << tile.name;
            else
                cout << ".";
        });
        cout << endl;
    });
}

void part2()
{
    //#define SANITY_CHECK

    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Maze maze;
    i32 tile_start_r {0};
    i32 tile_start_c {0};

    {
        i32 r = 0;
        for (str line;
             std::getline(ifs, line);
             )
        {
            vec<Tile> row;
            i32 c = 0;

            for (char ch : line)
            {
                Tile tile;

                tile.name = ch;

                tile.r = r;
                tile.c = c;

                switch (ch)
                {
                    case '|':
                    {
                        tile.conn1 = Direction::up;
                        tile.conn2 = Direction::down;
                    } break;

                    case '-':
                    {
                        tile.conn1 = Direction::left;
                        tile.conn2 = Direction::right;
                    } break;

                    case 'L':
                    {
                        tile.conn1 = Direction::up;
                        tile.conn2 = Direction::right;
                    } break;

                    case 'J':
                    {
                        tile.conn1 = Direction::up;
                        tile.conn2 = Direction::left;
                    } break;

                    case '7':
                    {
                        tile.conn1 = Direction::down;
                        tile.conn2 = Direction::left;
                    } break;

                    case 'F':
                    {
                        tile.conn1 = Direction::down;
                        tile.conn2 = Direction::right;
                    } break;

                    case '.':
                    {
                        tile.conn1 = Direction::none;
                        tile.conn2 = Direction::none;
                    } break;

                    case 'S':
                    {
                        tile.conn1 = Direction::none;
                        tile.conn2 = Direction::none;

                        tile_start_r = r;
                        tile_start_c = c;
                    } break;

                    default:
                    throw std::format("Unknown cell <{} ({},{})>",
                                      ch, r, c);
                }

                ++c;
                row.push_back(std::move(tile));
            }

            ++r;

            maze.push_back(std::move(row));
        }
    }

    Tile* start = &maze.at(tile_start_r).at(tile_start_c);
    start->part_of_loop = true;

    // setup connection for start tile
    for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right})
    {
        if (can_move(maze, *start, dir, true))
        {
            if (start->conn1 == Direction::none)
                start->conn1 = dir;
            else if (start->conn2 == Direction::none)
                start->conn2 = dir;
            else
                throw "??????";
        }
    }

    Tile* curr_tile = start;
    Direction from = Direction::none;
    do
    {
#ifdef SANITY_CHECK
        i32 movs = 0;
        cout << "(" << curr_tile->r + 1 << "," << curr_tile->c + 1 << ") tile " << curr_tile->name << " [";
        for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        {
            if (dir != from and
                can_move(maze, *curr_tile, dir))
            {
                cout << dir << ", ";
                ++movs;
            }
        }
        cout << "] (" << movs << ")" << endl;

        if (movs > 1)
        {
            int s = 0;
        }

        bool moved = false;
#endif // SANITY_CHECK

        for (Direction dir : {Direction::up, Direction::down, Direction::left, Direction::right})
        {
            if (dir != from)
            {
                if (auto* new_tile = can_move(maze, *curr_tile, dir);
                    new_tile != nullptr)
                {
                    curr_tile = new_tile;
                    curr_tile->part_of_loop = true;
                    from = opposite(dir); // keep track of where you come from
#ifdef SANITY_CHECK
                    moved = true;
#endif // SANITY_CHECK
                    break;
                }
            }
        }

#ifdef SANITY_CHECK
        if (not moved)
        {
            // stuck? 
            int s = 0;
            throw "i'm stuck!";
        }
#endif // SANITY_CHECK

    } while (curr_tile->name != 'S');

    // calculate Point in polygon
    u32 inside {0};    
    for (auto [r, row&] : maze | std::views::enumerate)
    {
        for (auto [c, tile&] : row | std::views::enumerate)
        {
            // if the point we want to start tracing from
            // is part of the loop, it's automatically
            // outside the loop
            if (tile.part_of_loop)
                continue;
            
            // start scanning from left to right
            u32 intersection {0};
            for (u64 cc = c;
                 cc < maze.at(0).size();
                 ++cc)
            {
                const auto* tile = &maze.at(r).at(cc);

                if (tile->part_of_loop and
                    str("|LJ").contains(tile->name)) // why 'L' and 'J'? i have no idea
                {
                    ++intersection;
                }
            }

            if (intersection % 2 == 1)
            {
                ++inside;
            }
        }
    }

    //draw_maze(maze);

    auto res = inside;
    cout << "day 10 part 2 (" << file_path << ") " << res << endl;
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
