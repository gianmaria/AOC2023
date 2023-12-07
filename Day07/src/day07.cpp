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

struct Hand
{
    vec<char> original_cards;
    vec<char> mod_cards;
    u32 bid {};
    u32 score {};
};

/**
* calculate how many A, K, Q, J, etc you have in a single hand
*/
map<char, u32> fill_freq_map(const vec<char>& hand)
{
    map<char, u32> freq_map;
    for (char card : hand)
    {
        freq_map[card] += 1;
    }

    return freq_map;
}

/**
* calculates how many High card, One pair, Two pair, etc you have in single hand
*/
std::array<u32, 6> fill_occurr_map(const vec<char>& hand)
{
    auto freq_map = fill_freq_map(hand);

    std::array<u32, 6> occurr_map {};
    for (const auto& [key, value] : freq_map)
    {
        occurr_map.at(value) += 1;
    }

    return occurr_map;
}

auto calculate_hand_score(const vec<char>& hand) -> u32
{
    auto freq_map = fill_freq_map(hand);
    auto occurr_map = fill_occurr_map(hand);

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
        auto bid = static_cast<u32>(std::stoul(parts[1]));
        auto score = calculate_hand_score(cards);

        hands.emplace_back(cards, vec<char>{}, bid, score);
    }

    std::sort(hands.begin(), hands.end(),
              [](const Hand& lhs, const Hand& rhs)
    {
        return lhs.score > rhs.score;
    });

    std::sort(hands.begin(), hands.end(),
              [](const Hand& lhs, const Hand& rhs)
    {
        static map<char, u32> scoring_map {
            {'A',13},{'K',12},{'Q',11},
            {'J',10},{'T',9},{'9',8},
            {'8',7},{'7',6},{'6',5},
            {'5',4},{'4',3},{'3',2},
            {'2',1}
        };

        if (lhs.score == rhs.score)
        {
            for (u32 i = 0;
                 i < lhs.original_cards.size();
                 ++i)
            {
                auto lhs_score = scoring_map.at(lhs.original_cards.at(i));
                auto rhs_score = scoring_map.at(rhs.original_cards.at(i));

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
        res += hands.at(i).bid * (i + 1);
    }

    cout << "part 1 (" << file_path << ") " << res << endl;
}

vec<char> replace_J_in_hand(const vec<char>& hand)
{
    auto find_replacement_card = [](auto freq_map, u32 num) -> char
    {
        char replacement_card {};

        for (const auto& [key, value] : freq_map)
        {
            if (value == num)
            {
                replacement_card = key;
                break;
            }
        }

        return replacement_card;
    };

    auto mod_hand = vec<char> {hand};

    auto num_of_J = std::count(hand.begin(), hand.end(), 'J');

    auto freq_map = fill_freq_map(hand);
    freq_map.erase('J');

    auto occurr_map = fill_occurr_map(hand);

    char replacement_card {};

    if (num_of_J == 5)
    {
        replacement_card = 'A';
    }
    else if (num_of_J == 0)
    {
        // nothing to do
        return mod_hand;
    }
    else
    {
        for (u64 i = 5 - num_of_J;
             i >= 1;
             --i)
        {
            if (occurr_map.at(i) != 0)
            {
                replacement_card = find_replacement_card(freq_map, i);
                break;
            }
        }
    }


#if 0

    if (num_of_J == 5)
    {
        // JJJJJ
        replacement_card = 'A';
    }
    else if (num_of_J == 4)
    {
        // JJJJ1
        replacement_card = find_replacement_card(freq_map, 1);
    }
    else if (num_of_J == 3)
    {
        // could be: JJJ22
        if (occurr_map.at(2) == 1)
        {
            replacement_card = find_replacement_card(freq_map, 2);
        }
        // could be: JJJ12
        else if (occurr_map.at(1) == 2)
        {
            replacement_card = find_replacement_card(freq_map, 1);
        }
        else
        {
            throw "Something went wrong, sorry bro... 1";
        }

        int s = 0;
    }
    else if (num_of_J == 2)
    {

        // could be: JJ111 -> 11111
        if (occurr_map.at(3) == 1)
        {
            replacement_card = find_replacement_card(freq_map, 3);
        }
        // could be: JJ112 -> 11112
        else if (occurr_map.at(2) == 1)
        {
            replacement_card = find_replacement_card(freq_map, 2);
        }
        // could be: JJ123 -> 11123
        else if (occurr_map.at(1) == 3)
        {
            replacement_card = find_replacement_card(freq_map, 1);
        }
        else
        {
            throw "No bueno you miss a case... 2";
        }

        int s = 0;
    }
    else if (num_of_J == 1)
    {
        // could be: JAAAA
        if (occurr_map.at(4) == 1)
        {
            replacement_card = find_replacement_card(freq_map, 4);
        }
        // could be: JQAAA
        else if (occurr_map.at(3) == 1)
        {
            replacement_card = find_replacement_card(freq_map, 3);
        }
        // could be: JAAQQ
        else if (occurr_map.at(2) == 2)
        {
            replacement_card = find_replacement_card(freq_map, 2);
        }
        // could be: JAA12
        else if (occurr_map.at(2) == 1)
        {
            replacement_card = find_replacement_card(freq_map, 2);
        }
        // could be: J1234
        else if (occurr_map.at(1) == 4)
        {
            replacement_card = find_replacement_card(freq_map, 1);
        }
        else
        {
            throw "No bueno you miss a case... 1";
        }

        int s = 0;
    }
    else if (num_of_J == 0)
    {
        return mod_hand;
    }
    else
    {
        throw "BRUUH";
    }

#endif // 0


    std::replace_if(mod_hand.begin(), mod_hand.end(),
                    [](char ch) { return ch == 'J'; },
                    replacement_card);

    // sanity check
    {
        auto score_before = calculate_hand_score(hand);
        auto score_after = calculate_hand_score(mod_hand);
        if (score_before >= score_after and
            num_of_J != 5)
        {
            throw std::format("Your algorithm suuuuuuucks: before <{}:{}> after <{}:{}>",
                              str {hand.begin(), hand.end()}, score_before,
                              str {mod_hand.begin(), mod_hand.end()}, score_after);
        }
    }

    return mod_hand;

}

void part2()
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
        auto mod_cards = replace_J_in_hand(cards);

        auto bid = std::stoul(parts[1]);
        auto mod_score = calculate_hand_score(mod_cards);

        hands.emplace_back(cards, mod_cards, bid, mod_score);
    }

    std::sort(hands.begin(), hands.end(),
              [](const Hand& lhs, const Hand& rhs)
    {
        return lhs.score > rhs.score;
    });

    std::sort(hands.begin(), hands.end(),
              [](const Hand& lhs, const Hand& rhs)
    {
        static map<char, u32> strength_map {
            {'A',13},{'K',12},{'Q',11},
            {'J',0},{'T',9},{'9',8},
            {'8',7},{'7',6},{'6',5},
            {'5',4},{'4',3},{'3',2},
            {'2',1}
        };

        if (lhs.score == rhs.score)
        {
            for (u32 i = 0;
                 i < lhs.original_cards.size();
                 ++i)
            {
                auto lhs_card_strength = strength_map.at(lhs.original_cards.at(i));
                auto rhs_card_strength = strength_map.at(rhs.original_cards.at(i));

                if (lhs_card_strength != rhs_card_strength)
                    return lhs_card_strength < rhs_card_strength;
            }

            throw "you are not supposed to be here...";
        }
        else
        {
            return lhs.score < rhs.score;
        }
    });

    u64 res = 0;
    u32 pos = 1;

    std::for_each(hands.begin(), hands.end(),
                  [&res, &pos](const Hand& hand)
    {
        res += hand.bid * pos++;
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
