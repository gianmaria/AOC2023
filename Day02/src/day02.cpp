// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <set>
#include <ranges>
#include <format>
#include <vector>
#include <regex>
#include <iterator>
#include <map>

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
str ltrim(str_cref s) {
    auto it = std::find_if(s.begin(), s.end(), [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return str(it, s.end());
}

// Trim from the end (right)
str rtrim(str_cref s) {
    auto it = std::find_if(s.rbegin(), s.rend(), [](char ch) {
        return !std::isspace(static_cast<unsigned char>(ch));
    });
    return str(s.begin(), it.base());
}

// Trim from both ends
str trim(str_cref s) {
    return ltrim(rtrim(s));
}

vector<str> split_string(str_cref line, char sep)
{
    vector<str> res{};
    std::istringstream iss{line};

    str token{};

    while (std::getline(iss, token, sep))
    {
        res.push_back(std::move(trim(token)));
    }

    return res;
}



struct Draw
{
    i32 red{0};
    i32 green{0};
    i32 blue{0};
};

struct Game
{
    i32 id {-1};
    vector<Draw> draws;
};

i32 extract_game_id(str_cref input)
{
    i32 res = -1;

    auto pattern = std::regex(R"(Game\s+(\d+))");
    std::smatch match{};

    if (std::regex_search(input, match, pattern))
    {
        if (match.size() != 2)
            throw std::runtime_error(std::format("[ERROR] expected two parts for input <{}>", input));

        res = std::stoi(match[1]);
    }
    else
    {
        throw std::runtime_error(std::format("[ERROR] no match for input <{}>", input));
    }

    return res;
}

void extract_draws(Game& game, str input)
{
    // input -> "3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"
    
    auto parts = split_string(input, ';');
    // parts -> [
    //      "3 blue, 4 red", 
    //      "1 red, 2 green, 6 blue", 
    //      "2 green"
    // ]

    for (auto part : parts)
    {
        auto token = part;

        // token -> "1 red, 2 green, 6 blue"
        Draw draw{};

        const auto pattern = std::regex(R"((\d+)\s+(\w+),?)");

        for (std::smatch match; 
             std::regex_search(token, match, pattern);
             )
        {
            // match -> [(1 red), (1), (red)]

            if (match.size() != 3)
                throw std::runtime_error(std::format("[ERROR] expected 3 matches for input <{}>", input));

            auto count = match[1].str();
            auto color = match[2].str();

            if (color == "red")
            {
                draw.red = std::stoi(count);
            }
            else if (color == "green")
            {
                draw.green = std::stoi(count);
            }
            else if (color == "blue")
            {
                draw.blue = std::stoi(count);
            }
            else
            {
                throw std::runtime_error(std::format("[ERROR] unknown color <{}>", color));
            }

            token = match.suffix();
        }
        
        game.draws.push_back(std::move(draw));

        int s = 0;
    }

}

i32 sum_possible_games(vector<Game>& games, 
                       i32 max_red, i32 max_green, i32 max_blue)
{
    i32 acc = 0;

    for (auto& game : games)
    {
        i32 tot_red = 0;
        i32 tot_green = 0;
        i32 tot_blue = 0;
        
        for (auto& draw : game.draws)
        {
            tot_red   = std::max(tot_red, draw.red);
            tot_green = std::max(tot_green,draw.green);
            tot_blue = std::max(tot_blue, draw.blue);
        }

        if (tot_red <= max_red and
            tot_green <= max_green and
            tot_blue <= max_blue)
        {
            //cout << "valid game id " << game.id << endl;
            acc += game.id;
        }
    }

    return acc;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    str line{};

    vector<Game> games{};

    while (std::getline(ifs, line))
    {
        auto parts = split_string(line, ':');

        if (parts.size() != 2)
            throw std::runtime_error(std::format("[ERROR] expected two parts for line <{}>", line));

        Game game{};
        game.id = extract_game_id(parts[0]);
        extract_draws(game, parts[1]);

        games.push_back(std::move(game));

        int s = 0;
    }

     int s = 0;
     auto num_games = sum_possible_games(games, 12, 13, 14);

    i32 res = num_games;
    cout << "part 1 (" << file_path << ") " << res << endl;
}

i32 sum_possible_games_p2(vector<Game>& games)
{
    i32 acc = 0;

    for (auto& game : games)
    {
        i32 tot_red = 0;
        i32 tot_green = 0;
        i32 tot_blue = 0;

        for (auto& draw : game.draws)
        {
            tot_red   = std::max(tot_red, draw.red);
            tot_green = std::max(tot_green,draw.green);
            tot_blue = std::max(tot_blue, draw.blue);
        }

        acc += tot_red * tot_green * tot_blue;
    }

    return acc;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    str line{};

    vector<Game> games{};

    while (std::getline(ifs, line))
    {
        auto parts = split_string(line, ':');

        if (parts.size() != 2)
            throw std::runtime_error(std::format("[ERROR] expected two parts for line <{}>", line));

        Game game{};
        game.id = extract_game_id(parts[0]);
        extract_draws(game, parts[1]);

        games.push_back(std::move(game));

        int s = 0;
    }

    int s = 0;
    auto num_games = sum_possible_games_p2(games);

    i32 res = num_games;
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
