// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <assert.h>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
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
using str = std::string;
using str_cref = std::string const&;
using std::ranges::find_if;

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace fs = std::filesystem;

template<typename T>
using vec = vector<T>;

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


void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    i32 acc {};

    for (str line;
         std::getline(ifs, line);
         )
    {
        auto parts = split_string(line, ':');
        assert(parts.size() == 2);

        parts = split_string(parts.at(1), '|');
        assert(parts.size() == 2);

        const auto pattern = std::regex(R"((\d+))");

        vec<i32> winning_numbers;
        vec<i32> your_numbers;

        // exctract winning numbers
        auto token = parts.at(0);
        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            assert(match.size() == 2);

            winning_numbers.push_back(std::stoi(match[1].str()));

            token = match.suffix();
        }

        // exctract your numbers
        token = parts.at(1);
        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            assert(match.size() == 2);

            your_numbers.push_back(std::stoi(match[1].str()));

            token = match.suffix();
        }

        vec<i32> common;
        std::ranges::copy_if(winning_numbers, std::back_inserter(common),
                             [&your_numbers](i32 val)
        {
            return std::ranges::find(your_numbers, val) != your_numbers.end();
        });

        if (common.size() > 0)
        {
            i32 res = std::pow(2, common.size() - 1);
            acc += res;
        }
        int s = 0;
    }


    i32 res = acc;
    cout << "part 1 (" << file_path << ") " << res << endl;
}

struct Card
{
    i32 num {};
    i32 wins {};
};

i32 compute_scratchcards(const vec<Card>& scratchcards,
                         const Card& card)
{
    i32 counter {card.wins};

    for (auto j = 0;
         j < card.wins;
         ++j)
    {
        counter += compute_scratchcards(scratchcards,
                                        scratchcards[j + card.num]);
    }

    return counter;
}



void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    i32 id = 1;

    vec<Card> scratchcards;

    // collect all the cards
    for (str line;
         std::getline(ifs, line);
         )
    {
        Card card;
        card.num = id++;

        auto parts = split_string(line, ':');
        assert(parts.size() == 2);

        parts = split_string(parts.at(1), '|');
        assert(parts.size() == 2);

        const auto pattern = std::regex(R"((\d+))");
        vec<i32> winning_numbers;
        // exctract winning numbers
        auto token = parts.at(0);
        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            assert(match.size() == 2);

            winning_numbers.push_back(std::stoi(match[1].str()));

            token = match.suffix();
        }


        vec<i32> your_numbers;
        // exctract your numbers
        token = parts.at(1);
        for (std::smatch match;
             std::regex_search(token, match, pattern);
             )
        {
            assert(match.size() == 2);

            your_numbers.push_back(std::stoi(match[1].str()));

            token = match.suffix();
        }

        vec<i32> common;
        std::ranges::copy_if(winning_numbers, std::back_inserter(common),
                             [&](i32 val)
        {
            return std::ranges::find(your_numbers, val) != your_numbers.end();
        });

        card.wins = common.size();

        scratchcards.push_back(std::move(card));
        int s = 0;
    }


    i32 counter = scratchcards.size();
    for (const auto& card : scratchcards)
    {
        counter += compute_scratchcards(scratchcards,
                                        card);
    }

    i32 res = counter;

    cout << "part 2 (" << file_path << ") " << res << endl;
}


int main()
{

    try
    {
        part1();
        part2();
    }
    catch (const std::exception& e)
    {
        cout << e.what() << endl;
    }

    return 0;
}
