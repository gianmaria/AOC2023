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
#include <string_view>
#include <string>
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
        res.push_back(std::move(trim(token)));
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

struct Dir
{
    str left;
    str right;
};

struct Map
{
    str instructions;
    map<str, Dir> nodes;
};

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Map map;

    // first line is directions
    str line;
    std::getline(ifs, line);

    map.instructions = line;

    const auto pattern = std::regex(R"((\w{3})\s+=\s+\((\w{3}),\s+(\w{3})\))");

    for (;
         std::getline(ifs, line);
         )
    {
        auto token = line;

        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            map.nodes[match[1].str()] = Dir {match[2].str(), match[3].str()};

            token = match.suffix();
        }
    }

    str current_node = "AAA";
    u64 index = 0;
    u64 steps = 0;
    auto instructions_size = map.instructions.size();

    while (current_node != "ZZZ")
    {
        index = index % instructions_size;
        char dir = map.instructions.at(index);

        //cout << steps << ". " << current_node << " (" << dir << ") ";

        if (dir == 'L')
        {
            current_node = map.nodes.at(current_node).left;
        }
        else if (dir == 'R')
        {
            current_node = map.nodes.at(current_node).right;
        }
        else
        {
            throw "no bueno!";
        }
        //cout << current_node << endl;

        ++index;
        ++steps;
    }

    u64 res = steps;
    cout << "part 1 (" << file_path << ") " << res << endl;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    Map map;

    // first line is directions
    str line;
    std::getline(ifs, line);

    map.instructions = line;

    const auto pattern = std::regex(R"((\w{3})\s+=\s+\((\w{3}),\s+(\w{3})\))");

    for (;
         std::getline(ifs, line);
         )
    {
        auto token = line;

        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            map.nodes[match[1].str()] = Dir {match[2].str(), match[3].str()};

            token = match.suffix();
        }
    }


    constexpr auto ends_with_a = [](str_cref key)
    {
        return key.ends_with("A");
    };

    auto start_nodes =
        map.nodes |
        std::views::keys |
        std::views::filter(ends_with_a) |
        std::ranges::to<std::vector>();

    auto inst_size = map.instructions.size();

    vec<u64> cycles;

    for (auto curr_node : start_nodes)
    {
        u64 index = 0;
        u64 cycle_len = 0;

        while (not curr_node.ends_with("Z"))
        {
            index = index % inst_size;
            char dir = map.instructions.at(index);

            if (dir == 'L')
            {
                curr_node = map.nodes.at(curr_node).left;
            }
            else if (dir == 'R')
            {
                curr_node = map.nodes.at(curr_node).right;
            }
            else
            {
                throw "no bueno!";
            }

            ++index;
            ++cycle_len;
        }

        cycles.push_back(cycle_len);
    }

    u64 res = std::accumulate(cycles.begin(), cycles.end(), 1ULL,
                              [](u64 acc, u64 value)
    {
        u64 res = std::lcm(acc, value);
        return res;
    });

    cout << "part 2 (" << file_path << ") " << res << endl;
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
