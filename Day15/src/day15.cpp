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
#include <deque>

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


u8 hash(str_cref input)
{
    u32 value = 0;

    for (char ch : input)
    {
        auto ascii_code = static_cast<u32>(ch);
        value += ascii_code;
        value *= 17;
        value %= 256;
    }

    return static_cast<u8>(value);
}

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    str line;
    std::getline(ifs, line);

    auto steps = split_string(line, ',');

    u64 res = 0;
    for (const auto& step : steps)
    {
        res += hash(step);
    }

    return res;
}

struct Lens
{
    u32 focal_length = 0;
    str label = "";

    bool operator==(const Lens& other) const
    {
        return other.focal_length == focal_length and other.label == label;
    }
    bool operator==(str_cref other_label) const
    {
        return other_label == label;
    }
};

struct Box
{
    u32 num = 0;
    std::deque<Lens> lenses;

    bool contains(str_cref label)
    {
        return std::find(lenses.cbegin(), lenses.cend(), label) != lenses.cend();
    }

    void replace(str_cref label, u32 new_focal_len)
    {
        auto it = std::find(lenses.begin(), lenses.end(), label);
        if (it == lenses.end())
        {
            throw "Cannot find label in labels";
        }

        it->focal_length = new_focal_len;
    }

    void add_lens(u32 focal_length, str_cref label)
    {
        lenses.push_back({focal_length, label});
    }

    void remove_lens(str_cref label)
    {
        auto it = std::find(lenses.begin(), lenses.end(), label);
        if (it == lenses.end())
        {
            throw "Cannot find label in labels @remove_lens";
        }

        lenses.erase(it);
    }

};

void print_boxes(const std::array<Box, 256> boxes)
{
    // Box 0: [rn 1] [cm 2]
    // Box 1: [qp 3]

    for (const auto& box : boxes)
    {
        if (box.lenses.empty())
            continue;

        cout << std::format("Box {}: ", box.num);
        for (const auto& lens : box.lenses)
        {
            cout << std::format("[{} {}] ", lens.label, lens.focal_length);
        }
        cout << endl;
    }
    cout << endl;
}

auto create_boxes()
{
    std::array<Box, 256> boxes;
    u32 num = 0;
    for (auto& box : boxes)
    {
        box.num = num++;
    }
    return boxes;
}

u64 part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    str line;
    std::getline(ifs, line);

    auto steps = split_string(line, ',');

    auto boxes = create_boxes();

    for (const auto& step : steps)
    {
        //cout << "After '" << step << "':" << endl;

        if (step.find("=") != str::npos)
        {
            auto res = split_string(step, '=');
            auto label = res[0];
            u32 focal_len = std::stoul(res[1]);

            u32 box_num = hash(label);

            auto& box = boxes.at(box_num);

            if (box.contains(label))
            {
                box.replace(label, focal_len);
            }
            else
            {
                box.add_lens(focal_len, label);
            }
        }
        else if (step.find("-") != str::npos)
        {
            auto res = split_string(step, '-');
            auto label = res[0];

            u32 box_num = hash(label);

            auto& box = boxes.at(box_num);

            if (box.contains(label))
            {
                box.remove_lens(label);
            }
            else
            {
                // do nothing
            }

        }
        else
        {
            throw "Weird step, uga buga";
        }

        //print_boxes(boxes);
    }

    u64 res = 0;
    for (const auto& box : boxes)
    {
        if (box.lenses.empty())
            continue;        
        for (const auto& lens : box.lenses)
        {
            u64 slot = std::distance(
                box.lenses.cbegin(),
                std::find(box.lenses.cbegin(), box.lenses.cend(), lens));

            u64 tmp = (box.num + 1) * (slot + 1) * lens.focal_length;
            res += tmp;
        }
    }

    return res;
}

int main()
{
    try
    {
        auto part_1 = part1();
        cout << "day 15 part 1: " << part_1 << endl;

        auto part_2 = part2();
        cout << "day 15 part 2: " << part_2 << endl;
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
