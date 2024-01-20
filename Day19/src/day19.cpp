// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

#include <array>
#include <assert.h>
#include <functional>
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
#include <stack>
#include <string_view>
#include <vector>
#include <print>
#include <optional>
#include <chrono>
#include <ctime>


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
using std::print;
using std::println;
using std::optional;

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

template<typename T>
void print_matrix(const Matrix<T>& m)
{
    for (const auto& row : m)
    {
        for (const auto& elem : row)
        {
            print("{}", elem);
        }
        println("");
    }
    println("");
}

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

vector<str> split_string(str_cref input, str_cref separator)
{
    vector<str> tokens;
    size_t start = 0, end;

    while ((end = input.find(separator, start)) != str::npos)
    {
        tokens.push_back(input.substr(start, end - start));
        start = end + separator.length();
    }

    tokens.push_back(input.substr(start));

    return tokens;
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

str get_time()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;

    if (localtime_s(&local_time, &time) == 0)
    {
        char buffer[512] {};
        if (strftime(buffer, sizeof(buffer), "%H:%M:%S", &local_time) != 0)
        {
            return {buffer};
        }
    }
    return "";
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



struct Condition
{
    str rating;
    str op;
    u32 value;
};

struct Rule
{
    optional<Condition> condition;
    str destination;
};

u64 part1()
{
    auto file_path = "res\\test.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    auto tokens = split_string(input, "\n\n");
    const auto& workflows_s = tokens[0];
    const auto& ratings_s = tokens[1];

    map<str, vec<Rule>, std::less<>> workflows;
    vec<map<str, u32>> parts;

    // line: {x=787,m=2655,a=1222,s=2876}
    std::regex pattern(R"((\w)=(\d+))");
    for (auto line : split_string(ratings_s, "\n"))
    {
        std::sregex_iterator it(line.begin(), line.end(), pattern);
        std::sregex_iterator end;

        map<str, u32> part;
        while (it != end)
        {
            std::smatch match = *it;

            str rating = match[1].str();
            u32 value = std::stoul(match[2].str());

            part[rating] = value;

            ++it;
        }
        parts.emplace_back(part);
    }

    // rfg{s<537:gd,x>2440:R,A}
    // px{a<2006:qkq,m>2090:A,rfg}
    for (auto line : split_string(workflows_s, "\n"))
    {
        str workflow_name;

        u64 pos = 0;
        while (line[pos] != '{')
        {
            workflow_name.push_back(line[pos]);
            ++pos;
        }
        line = line.substr(pos);
        // {a<2006:qkq,m>2090:A,rfg}
        line = line.substr(1);
        line.pop_back();
        // a<2006:qkq,m>2090:A,rfg

        for (auto rule_str : split_string(line, ","))
        {
            // part = a<2006:qkq
            // part = m>2090:A
            // part = rfg

            if (rule_str.contains(':'))
            {
                auto cond_dest_str = split_string(rule_str, ":");
                // cond_dest_str[0] = a<2006
                // cond_dest_str[1] = qkq

                str split_on;
                if (cond_dest_str[0].contains('<'))
                {
                    split_on = "<";
                }
                else if (cond_dest_str[0].contains('>'))
                {
                    split_on = ">";
                }
                else
                {
                    throw "invalid op!";
                }

                auto condition_parts = split_string(cond_dest_str[0], split_on);
                // cond_dest[0] = a<2006
                // condition_parts[0] = a
                // condition_parts[1] = <
                // condition_parts[2] = 2006

                Condition condition;
                condition.rating = condition_parts[0];
                condition.op = split_on;
                std::from_chars(condition_parts[1].data(),
                                condition_parts[1].data() + condition_parts[1].size(),
                                condition.value);

                Rule rule;
                rule.destination = cond_dest_str[1];
                rule.condition = condition;

                workflows[workflow_name].push_back(rule);
            }
            else
            {
                Rule rule;
                rule.destination = rule_str;
                rule.condition = std::nullopt;

                workflows[workflow_name].push_back(rule);
            }
        }

    }

    int s = 0;

    auto apply = [](Rule& rule, map<str, u32>& part)
    {
        auto part_val = part[rule.condition->rating];

        if (rule.condition->op == ">")
        {
            return part_val > rule.condition->value;
        }
        else
        {
            return part_val < rule.condition->value;
        }

    };

    u64 acc = 0;
    for (auto& part : parts)
    {
        // part: {x=787,m=2655,a=1222,s=2876}
        cout << "{"
            << "x=" << part["x"] << ","
            << "m=" << part["m"] << ","
            << "a=" << part["a"] << ","
            << "s=" << part["s"] << ","
            << "}: ";

        auto* workflow = &workflows["in"];
        cout << "in";

        // workflow = px{a<2006:qkq,m>2090:A,rfg}
        bool found = false;
        while (not found)
        {
            for (auto& rule : *workflow)
            {
                if (rule.condition.has_value())
                {
                    if (apply(rule, part))
                    {
                        if (rule.destination == "A" or
                            rule.destination == "R")
                        {
                            if (rule.destination == "A")
                            {
                                cout << " -> A";
                                acc += part["x"] + part["m"] + part["a"] + part["s"];
                            }
                            else
                            {
                                cout << " -> R";
                            }

                            found = true;
                            break;
                        }
                        else
                        {

                            workflow = &workflows[rule.destination];
                            cout << " -> " << rule.destination;
                            break;
                        }
                    }
                }
                else
                {
                    if (rule.destination == "A" or
                        rule.destination == "R")
                    {
                        if (rule.destination == "A")
                        {
                            cout << " -> A";
                            acc += part["x"] + part["m"] + part["a"] + part["s"];
                        }
                        else
                        {
                            cout << " -> R";
                        }
                        found = true;
                        break;
                    }
                    else
                    {
                        workflow = &workflows[rule.destination];
                        cout << " -> " << rule.destination;
                        break;
                    }
                }
            }

        }
        cout << endl;

    }


#if 0
    // workflow: {name:str, rules: [rule1, rule2,...]}
    // rule1: {condition, destination:str}
    // rule2: {condition, destination:str}
    // condition: {rating: str, value: u32, op:char}

    // parts: {part, rating}
    apply(rule, part)
    {
        part_val = part[rule.condition.rating];
        if (rule.condition.op == '>')
        {
            res = part_val > rule.condition.value;
        }
        else
        {
            res = part_val < rule.condition.value;
        }
        return res
    }

    for (part : parts)
    {
        // part: {x=787,m=2655,a=1222,s=2876}

        workflow = workflows["in"];

        while (true)
        {
            for (rule : workflow.rules)
            {
                if (apply(rule, part))
                {
                    workflow = workflows[rule.destination];
                    break;
                }
                        }

            if (workflow.name == "A")
            {
                acc += part.x + part.m, part.a, part.s;
                break;
            }
            else if (workflow.name == "R")
            {
                break;
            }
                    }

                }
#endif

    return acc;
}

u64 part2()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    /*for (auto& line : split_string(input, "\n"))
    {

    }*/

    u64 acc = 0;

    u64 res = acc;
    return res;
}

int main()
{
    try
    {
        println("day 19 part 1: {}", part1());
        println("day 19 part 2: {}", part2());

        return 0;
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

    return 1;
}
