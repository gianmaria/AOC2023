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
using std::array;
using std::set;
using std::map;
using std::pair;

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


u64 compute(str_cref input, const vec<u64>& spring_groups)
{
    auto create_str = [](const vec<u64>& indexes,
                         str_cref alphabet,
                         array<char, 512>& dest)
    {
        for (auto [index, _] : views::enumerate(indexes))
        {
            dest[index] = alphabet[indexes[static_cast<u64>(index)]];
        }
    };

    // group of broken springs
    auto is_valid = [](const char* input,
                       const vec<u64>& spring_groups)
    {
        /*if (strcmp(input, ".#...#....###.") == 0)
        {
            int s = 0;
        }*/

        vec<u64> spring_groups_found;
        spring_groups_found.reserve(2*spring_groups.size());

        auto end = strlen(input);
        for (u64 i = 0;
             i < end;
             ++i)
        {
            if (input[i] == '#')
            {
                u64 counter = 0;
                while (i < end and
                       input[i++] == '#')
                {
                    ++counter;
                }
                spring_groups_found.push_back(counter);
                --i;
            }
        }

        if (spring_groups_found.size() != spring_groups.size())
            return false;

        for (u64 i = 0;
             i < spring_groups_found.size();
             ++i)
        {
            if (spring_groups_found[i] != spring_groups[i])
                return false;
        }

        return true;
    };

    const str alphabet = ".#";
    array<char, 512> buffer {};
    array<char, 512> to_test {};

    auto qmark_count = static_cast<u64>(ranges::count(input, '?'));
    auto max = static_cast<u64>(std::pow(alphabet.size(), qmark_count));

    u64 correct = 0;
    auto indexes = vec<u64>(qmark_count, 0);
    u64 counter = 0;
    // generate all the possible permutations of the alphabet
    while (counter < max)
    {
        for (u64 i = 0;
             i < alphabet.size();
             ++i, ++counter)
        {
            create_str(indexes, alphabet, buffer);

            u64 elem_index = 0;
            for (u64 j = 0;
                 j < input.size();
                 ++j)
            {
                if (input[j] == '?')
                    to_test[j] = buffer[elem_index++];
                else
                    to_test[j] = input[j];
            }
            to_test[to_test.size() - 1] = 0;

#if 0
            if (std::regex_match(to_test.data(), regex))
            {
                ++correct;
            }
#else
            if (is_valid(to_test.data(), spring_groups))
            {
                ++correct;
            }
#endif // 0

            indexes[0] += 1;
        }

        for (u64 i = 0;
             i < indexes.size() - 1;
             ++i)
        {
            if (indexes[i] >= alphabet.size())
            {
                indexes[i] = 0;
                indexes[i + 1] += 1;
            }
        }
    }

    return correct;
}

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    u64 acc = 0;
    for (str line;
         std::getline(ifs, line);
         )
    {
        auto parts = split_string(line, ' ');
        auto nums_str = split_string(parts[1], ',');

        vec<u64> nums;
        std::transform(nums_str.begin(),
                       nums_str.end(),
                       std::back_inserter(nums),
                       [](str_cref str) { return std::stoull(str); }
        );

        //cout << std::format("{}) working with <{}>", ++count, parts[0]) << endl;
        acc += compute(parts[0], nums);
    }

    u64 res = acc;
    return res;
}

u64 part2()
{
    auto file_path = "res\\test.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    u64 acc = 0;
    for (str line;
         std::getline(ifs, line);
         )
    {
        auto parts = split_string(line, ' ');
        auto unfolded = std::format("{},{},{},{},{}",
                                    parts[1], parts[1], parts[1], parts[1], parts[1]);
        auto nums_str = split_string(unfolded, ',');

        vec<u64> nums;
        std::transform(nums_str.begin(),
                       nums_str.end(),
                       std::back_inserter(nums),
                       [](str_cref str) { return std::stoull(str); }
        );
        auto input = parts[0];
        input = std::format("{}?{}?{}?{}?{}",
                            input, input, input, input, input);


        //cout << std::format("{}) working with <{}>", ++count, input) << endl;
        acc += compute(input, nums);
    }

    u64 res = acc;
    return res;
}

int main()
{
    try
    {
        auto p1 = part1();
        cout << "day 12 part 1: " << p1 << endl;

        auto p2 = part2();
        cout << "day 12 part 2: " << p2 << endl;
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
