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
    vector<str> res{};
    std::istringstream iss{ line };

    str token{};

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

// ============================================================
// ============================================================
// ============================================================
// ============================================================
// ============================================================
// ============================================================
// ============================================================
// ============================================================


struct Conversion_Table
{
    u64 dest;
    u64 src;
    u64 len;
};

struct Conversion_Table_Comp
{
    bool operator()(const Conversion_Table& lhs,
                    const Conversion_Table& rhs) const
    {
        return lhs.src < rhs.src;
    }
};

struct Map
{
    str name;
    set<Conversion_Table, Conversion_Table_Comp> mappings;
};

struct Almanac
{
    vec<u64> seeds;
    vec<Map> maps;
};

vec<u64> parse_seeds(std::ifstream& ifs)
{
    vec<u64> seeds;

    str line;
    std::getline(ifs, line);

    const auto pattern = std::regex(R"((seeds:\s+)?(\s*\d+\s*))");
    auto token = line;

    for (std::smatch match;
         std::regex_search(token, match, pattern);
         )
    {
        seeds.push_back(std::stoull(match[2].str()));

        token = match.suffix();
    }

    return seeds;
}

set<Conversion_Table, Conversion_Table_Comp> parse_mappings(std::ifstream& ifs)
{
    set<Conversion_Table, Conversion_Table_Comp> mappings;

    for (str line;
         std::getline(ifs, line) and (line != "");
         )
    {
        auto nums = split_string(line, ' ');
        if (nums.size() != 3)
            throw std::format("expected to find three numbers for mapping: {}", line);

        Conversion_Table ct;
        ct.dest = std::stoull(nums[0]);
        ct.src = std::stoull(nums[1]);
        ct.len = std::stoull(nums[2]);

        mappings.insert(std::move(ct));
    }

    return mappings;
}

void part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    Almanac almanac;

    // first line is seeds
    almanac.seeds = parse_seeds(ifs);

    // parsing mappings
    for (str line;
         std::getline(ifs, line);
         )
    {
        if (contains(line, "map:"))
        {
            Map map;

            map.name = split_string(line, ' ')[0];
            map.mappings = parse_mappings(ifs);

            almanac.maps.push_back(std::move(map));
        }

    }

    u64 res = std::numeric_limits<u64>::max();

    for (auto seed : almanac.seeds)
    {
        //cout << "seed " << seed << endl;

        for (const Map& map : almanac.maps)
        {
            const Conversion_Table* ct_to_use = nullptr;

            for (const Conversion_Table& ct : map.mappings)
            {
                if (is_between(seed, ct.src, ct.src + ct.len - 1))
                {
                    ct_to_use = &ct;
                    break;
                }
            }

            //cout << "    " << seed << " -> ";
            if (ct_to_use != nullptr)
            {
                auto offset = seed - ct_to_use->src;
                auto new_seed = ct_to_use->dest + offset;
                seed = new_seed;
            }
            // else seed maps to itself

            //cout << seed << endl;
        }

        res = std::min(res, seed);
    }

    cout << "part 1 (" << file_path << ") " << res << endl;
}

void part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);

    Almanac almanac;

    // first line is seeds
    almanac.seeds = parse_seeds(ifs);

    // parsing mappings
    for (str line;
         std::getline(ifs, line);
         )
    {
        if (contains(line, "map:"))
        {
            Map map;

            map.name = split_string(line, ' ')[0];
            map.mappings = parse_mappings(ifs);

            almanac.maps.push_back(std::move(map));
        }

    }

    u64 res = std::numeric_limits<u64>::max();

    for (u64 i = 0;
         i < almanac.seeds.size() - 1;
         i += 2)
    {
        auto start = almanac.seeds[i];
        auto end = almanac.seeds[i] + almanac.seeds[i + 1];

        for (u64 seed = start;
             seed < end;
             ++seed)
        {
            auto current_seed = seed;

            //cout << "seed " << current_seed << endl;

            for (const Map& map : almanac.maps)
            {
                const Conversion_Table* ct_to_use = nullptr;

                for (const Conversion_Table& ct : map.mappings)
                {
                    if (is_between(current_seed, ct.src, ct.src + ct.len - 1))
                    {
                        ct_to_use = &ct;
                        break;
                    }
                }

                //cout << "    " << current_seed << " -> ";
                if (ct_to_use != nullptr)
                {
                    auto offset = current_seed - ct_to_use->src;
                    auto new_seed = ct_to_use->dest + offset;
                    current_seed = new_seed;
                }
                // else seed maps to itself

                //cout << current_seed << endl;
            }

            res = std::min(res, current_seed);
        }
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
