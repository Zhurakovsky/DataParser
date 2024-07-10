#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

#include "csv.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace csv;
using namespace std;
namespace fs = std::filesystem;

template<typename A, typename B>
std::pair<B,A> flip_pair(const std::pair<A,B> &p)
{
    return std::pair<B,A>(p.second, p.first);
}

template<typename A, typename B>
std::multimap<B,A> flip_map(const std::map<A,B> &src)
{
    std::multimap<B,A> dst;
    std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
                   flip_pair<A,B>);
    return dst;
}

void fill_container(const CSVRow& row, std::map<std::string, size_t>& container, const string& key)
{
    if (!row[key].is_null())
    {
        if(row[key].is_str())
        {
            std::string key(row[key].get_sv());    
            if(container.contains(key))
            {
                container[key] += 1;
            }
            else
            {
                container[key] = 1;
            }
        }
    }
}

template<typename A>
void print_results_console(const A& container, const std::string& key)
{
    cout << key << ": " << endl;
    for (const auto& item : container)
    {
        cout << "\t" << item.second << " : " << item.first << endl;
    }
}

int main()
{
    /***
     * To be more explicit let's store all distributions in a separated containers
     */
    std::map<std::string, size_t> big_data_category{};
    std::map<std::string, size_t> big_data_maker{};
    std::map<std::string, size_t> big_data_fuel{};

    const fs::path datadirectory{"data"};

    CSVFormat format;
    format.delimiter(',')
          .quote(false);
    format.variable_columns(false);
    format.variable_columns(VariableColumnPolicy::IGNORE_ROW);

    for (auto const& dir_entry : fs::directory_iterator{datadirectory}) 
    {
        if (!dir_entry.is_regular_file())
        {
            continue;
        }

        std::string datapath = fs::path(dir_entry.path()).string();
        auto reader = std::unique_ptr<CSVReader>(new CSVReader(datapath.c_str(), format)); 

        for (const CSVRow& row : *reader)
        {
            fill_container(row, big_data_fuel, "fuel");
            fill_container(row, big_data_category, "category");
            fill_container(row, big_data_maker, "makerName");
        }
    }

    std::multimap<size_t, std::string> sorted_big_data_fuel = flip_map(big_data_fuel);
    std::multimap<size_t, std::string> sorted_big_data_category = flip_map(big_data_category);
    std::multimap<size_t, std::string> sorted_big_data_maker = flip_map(big_data_maker);

    print_results_console(sorted_big_data_fuel, "fuel");
    print_results_console(sorted_big_data_category, "category");
    print_results_console(sorted_big_data_maker, "makerName");

    json j;
    j["fuel"] = json(big_data_fuel);
    j["category"] = json(big_data_category);
    j["makerName"] = json(big_data_maker);

    // write prettified JSON to file
    std::ofstream writeToFile("result_output.json");
    writeToFile << std::setw(4) << j << std::endl;
    writeToFile.close();

    return 0;
}
