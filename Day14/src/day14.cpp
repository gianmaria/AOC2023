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

u64 part1()
{
    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    Matrix<char> matrix;

    for (auto& line : split_string(input, "\n"))
    {
        matrix.emplace_back(line.begin(), line.end());
    }

    for (auto [r, row] : views::enumerate(matrix))
    {
        for (auto [c, col] : views::enumerate(row))
        {
            if (matrix.at(r).at(c) == 'O')
            {
                auto curr_row = r;
                while (curr_row > 0)
                {
                    if (matrix.at(curr_row - 1).at(c) == '.')
                    {
                        matrix.at(curr_row - 1).at(c) = 'O';
                        matrix.at(curr_row).at(c) = '.';
                    }
                    else
                    {
                        break;
                    }

                    --curr_row;
                }
            }
        }
    }

    u64 acc = 0;
    for (auto [r, row] : views::enumerate(matrix))
    {
        auto occurrences = ranges::count(row, 'O');
        acc += occurrences * (matrix.size() - r);
    }

    u64 res = acc;
    return res;
}

template<typename T>
str stringify(const Matrix<T> m)
{
    auto concat = [](str_cref acc, const vec<char>& row)
    {
        return acc + str(row.begin(), row.end());
    };
    return std::accumulate(m.begin(), m.end(), str(), concat);
}

template<typename T>
u64 murmur64(const Matrix<T> m) 
{
    u64 h = 0;
    for (const auto row : m)
    {
        for (char ch : row)
        {
            h += static_cast<u64>(ch);
            h ^= h >> 33;
            h *= 0xff51afd7ed558ccdL;
            h ^= h >> 33;
            h *= 0xc4ceb9fe1a85ec53L;
            h ^= h >> 33;
        }
    }

    return h;
}

template<typename T>
void perform_full_tilt(Matrix<T>& matrix)
{
    auto rows = matrix.size();
    auto cols = matrix.at(0).size();

    // tilt up
    for (i64 r = 1;
         r < rows;
         ++r)
    {
        for (i64 c = 0;
             c < cols;
             ++c)
        {
            if (matrix.at(r).at(c) == 'O')
            {
                auto curr_row = r;
                while (curr_row > 0)
                {
                    auto up_row = curr_row - 1;
                    if (matrix.at(up_row).at(c) == '.')
                    {
                        matrix.at(up_row).at(c) = 'O';
                        matrix.at(curr_row).at(c) = '.';
                    }
                    else
                    {
                        break;
                    }

                    --curr_row;
                }
            }
        }
    }

    // tilt left
    for (i64 c = 1;
         c < cols;
         ++c)
    {
        for (i64 r = 0;
             r < rows;
             ++r)
        {
            if (matrix.at(r).at(c) == 'O')
            {
                auto curr_col = c;
                while (curr_col > 0)
                {
                    auto left_col = curr_col - 1;
                    if (matrix.at(r).at(left_col) == '.')
                    {
                        matrix.at(r).at(left_col) = 'O';
                        matrix.at(r).at(curr_col) = '.';
                    }
                    else
                    {
                        break;
                    }

                    --curr_col;
                }
            }
        }
    }

    // tilt down
    for (i64 r = rows - 2;
         r >= 0;
         --r)
    {
        for (i64 c = 0;
             c < cols;
             ++c)
        {
            if (matrix.at(r).at(c) == 'O')
            {
                auto curr_row = r;
                while (curr_row < rows - 1)
                {
                    auto down_row = curr_row + 1;
                    if (matrix.at(down_row).at(c) == '.')
                    {
                        matrix.at(down_row).at(c) = 'O';
                        matrix.at(curr_row).at(c) = '.';
                    }
                    else
                    {
                        break;
                    }

                    ++curr_row;
                }
            }
        }
    }

    // tilt right
    for (i64 c = cols - 2;
         c >= 0;
         --c)
    {
        for (i64 r = 0;
             r < rows;
             ++r)
        {
            if (matrix.at(r).at(c) == 'O')
            {
                auto curr_col = c;
                while (curr_col < cols - 1)
                {
                    auto right_col = curr_col + 1;
                    if (matrix.at(r).at(right_col) == '.')
                    {
                        matrix.at(r).at(right_col) = 'O';
                        matrix.at(r).at(curr_col) = '.';
                    }
                    else
                    {
                        break;
                    }

                    ++curr_col;
                }
            }
        }
    }

}

template<typename T>
auto find_cycle(Matrix<T>& matrix)
{
    using matrix_hash = u64;
    using tilt_num = u64;
    map<matrix_hash, tilt_num> GRIDS;

    u64 tilt_counter = 1;
    while (true)
    {
        perform_full_tilt(matrix);
        auto hash = murmur64(matrix);

        if (GRIDS.contains(hash))
        {
            // we found our cycle
            u64 cycle_len = tilt_counter - GRIDS[hash];
            return std::make_tuple(tilt_counter, cycle_len);
        }
        else
        {
            GRIDS[hash] = tilt_counter;
            ++tilt_counter;
        }
    }
}

u64 part2()
{
    // with the help of: https://www.youtube.com/watch?v=BBgflSmf0a0

    auto file_path = "res\\input.txt";
    auto ifs = std::ifstream(file_path);
    if (not ifs.is_open())
        throw std::format("Cannot open file <{}>", file_path);

    auto input = str(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());

    Matrix<char> matrix;

    // read input
    for (auto& line : split_string(input, "\n"))
    {
        matrix.emplace_back(line.begin(), line.end());
    }

    auto [tilt_counter, cycle_len] = find_cycle(matrix);

    const u64 cycles_to_perform = 1'000'000'000;
    const u64 tilt_wo_cycles = tilt_counter - cycle_len;
    const u64 num_cycles = (cycles_to_perform - tilt_wo_cycles) / cycle_len;
    i64 cycles_left_to_perform = cycles_to_perform - tilt_wo_cycles - (cycle_len * num_cycles);
    
    while (cycles_left_to_perform > 0)
    {
        perform_full_tilt(matrix);
        --cycles_left_to_perform;
    }

    u64 acc = 0;
    for (auto [r, row] : views::enumerate(matrix))
    {
        auto occurrences = ranges::count(row, 'O');
        acc += occurrences * (matrix.size() - r);
    }

    u64 res = acc;
    return res;
}

int main()
{
    try
    {
        println("day 14 part 1: {}", part1());
        println("day 14 part 2: {}", part2());

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
