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

map<char, u32> fill_freq_map(const vec<char>& hand)
{
    map<char, u32> freq_map;
    for (char card : hand)
    {
        freq_map[card] += 1;
    }

    return freq_map;
}

std::array<u32, 6> fill_occurr_map(const map<char, u32>& freq_map)
{
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
    auto occurr_map = fill_occurr_map(freq_map);

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

vec<char> replace_J_in_hand(const vec<char>& cards)
{
    auto find_replacement_card = [](auto freq_map, u32 num) -> char
    {
        char replacement_char {};

        for (const auto& [key, value] : freq_map)
        {
            if (value == num)
            {
                replacement_char = key;
                break;
            }
        }

        return replacement_char;
    };

    auto mod_cards = vec<char> {cards};

    auto num_of_J = std::count(cards.begin(), cards.end(), 'J');

    auto freq_map = fill_freq_map(cards);
    freq_map.erase('J');
    auto occurr_map = fill_occurr_map(freq_map);
    char replacement_card {};

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
        return mod_cards;
    }
    else
    {
        throw "BRUUH";
    }


    std::replace_if(mod_cards.begin(), mod_cards.end(),
                    [](char ch) { return ch == 'J'; },
                    replacement_card);

    /*auto it = std::find(mod_cards.begin(),
                        mod_cards.end(),
                        'J');

    if (it == mod_cards.end())
        throw "Something went wrong, sorry bro...";

    auto index = std::distance(mod_cards.begin(), it);
    mod_cards.at(index) = replacement_card;*/


#if 0
    for (u32 j = 0;
         j < num_of_J;
         ++j)
    {
        // find J pos
        auto it = std::find(hand.begin(), hand.end(), 'J');
        auto j_pos = std::distance(hand.begin(), it);

        u32 highest_score = 0;
        char best_card = '0';

        for (char c : "AKQT98765432")
        {
            // replace J with new card
            mod_hand.at(j_pos) = c;
            auto score = calculate_hand_score(mod_hand);

            if (score > highest_score)
            {
                highest_score = score;
                best_card = c;
}
}
    }

#endif // 0

    // sanity check
    {
        auto score_before = calculate_hand_score(cards);
        auto score_after = calculate_hand_score(mod_cards);
        if (score_before >= score_after and
            num_of_J != 5)
        {
            throw std::format("Your algorithm suuuuuuucks: before <{}:{}> after <{}:{}>",
                              str {cards.begin(), cards.end()}, score_before,
                              str {mod_cards.begin(), mod_cards.end()}, score_after);
        }
    }

    return mod_cards;
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
        auto score = calculate_hand_score(cards);
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

    for (u64 i = 0;
         i < hands.size();
         ++i)
    {
        res += hands.at(i).bid * (i + 1);
    }

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
