// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <assert.h>
#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
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

static map<char, u32> scoring_map {
            {'A',13},{'K',12},{'Q',11},
            {'J',10},{'T',9},{'9',8},
            {'8',7},{'7',6},{'6',5},
            {'5',4},{'4',3},{'3',2},
            {'2',1}
};

struct Hand
{
    vec<char> cards;
    u32 bid {};
    u32 score {};
};

std::array<u32, 6> fill_occurr_map(const map<char, u32>& map)
{
    std::array<u32, 6> occurr_map {};

    for (const auto& [key, value] : map)
    {
        occurr_map.at(value) += 1;
    }

    return occurr_map;
}

auto calculate_hand_score(const vec<char>& hand) -> u32
{
    map<char, u32> map;

    for (char card : hand)
    {
        map[card] += 1;
    }

    auto occurr_map = fill_occurr_map(map);

    u32 score = 0;

    if (occurr_map.at(5) != 0)
    {
        // five of a kind
        score = 7;
    }
    else if (occurr_map.at(4) != 0)
    {
        // four of a kind
        score = 6;
    }
    else if (occurr_map.at(3) != 0)
    {
        if (occurr_map.at(2) == 1)
        {
            // full house
            score = 5;
        }
        else if (occurr_map.at(1) == 2)
        {
            // three of a kind
            score = 4;
        }
        else
        {
            throw std::format("WTF?!? <{}>", str {hand.begin(), hand.end()});
        }
    }
    else if (occurr_map.at(2) == 2)
    {
        // two pair
        score = 3;
    }
    else if (occurr_map.at(2) == 1)
    {
        // one pair
        score = 2;
    }
    else if (occurr_map.at(1) == 5)
    {
        // high card
        score = 1;
    }
    else
    {
        throw std::format("Unknown score for hand <{}>", str {hand.begin(), hand.end()});
    }

    return score;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    vec<Hand> hands;

    for (str line;
         std::getline(ifs, line);
         )
    {
        auto parts = split_string(line, ' ');

        auto cards = vec<char>(parts[0].begin(), parts[0].end());
        auto bid = std::stoul(parts[1]);
        auto score = calculate_hand_score(cards);

        hands.emplace_back(cards, bid, score);
    }

    std::sort(hands.begin(), hands.end(),
              [](const Hand& lhs, const Hand& rhs)
    {
        return lhs.score > rhs.score;
    });

    std::sort(hands.begin(), hands.end(),
              [](const Hand& lhs, const Hand& rhs)
    {
        if (lhs.score == rhs.score)
        {
            for (u32 i = 0;
                 i < lhs.cards.size();
                 ++i)
            {
                auto lhs_score = scoring_map.at(lhs.cards.at(i));
                auto rhs_score = scoring_map.at(rhs.cards.at(i));

                if (lhs_score != rhs_score)
                    return lhs_score < rhs_score;
            }

            throw "you are not supposed to be here...";
        }
        else
        {
            return lhs.score < rhs.score;
        }
    });

    u64 res = 0;

    for (u64 i = 0;
         i < hands.size();
         ++i)
    {
        res += hands.at(i).bid * (i+1);
    }

    cout << "part 1 (" << file_path << ") " << res << endl;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    u64 res = std::numeric_limits<u64>::max();
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
        cout << "[EXC] " << e.what() << endl;
    }

    return 0;
}
