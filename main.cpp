#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// #include <curl/curl.h>
#include "csv.hpp"
using csv::operator ""_csv;

using namespace csv;


using namespace std;
namespace fs = std::filesystem;

bool file_exists(const fs::path& p, fs::file_status s = fs::file_status{})
{
    std::cout << p;
    if (fs::status_known(s) ? fs::exists(s) : fs::exists(p))
    {
        std::cout << " exists\n";
        return true;
    }
    else
    {
        std::cout << " does not exist\n";
        return false;
    }
}



int main()
{
    // std::map<std::string_view, std::map<std::string_view, int>> big_data{};
    std::unordered_map<std::string_view, int> big_data_category{};
    std::unordered_map<std::string_view, int> big_data_maker{};
    std::unordered_map<std::string_view, int> big_data_fuel{};


    const fs::path datadirectory{"data"};

    std::cout << "Current path is " << fs::current_path() << '\n';

    if (!file_exists(datadirectory))
    {
        fs::create_directory(datadirectory);
    }
    else
    {
        cout << "Directory data exists!!!" << endl;
    }

    CSVFormat format;
    format.delimiter({ '\t',',','\n'})
    .quote(false);
    format.variable_columns(false); // Short-hand
    format.variable_columns(VariableColumnPolicy::IGNORE_ROW);

    for (auto const& dir_entry : fs::directory_iterator{datadirectory}) 
    {
        //std::cout << dir_entry.path() << '\n';
        std::string datapath = fs::path(dir_entry.path()).string();

        auto info = get_file_info(datapath.c_str());
        cout << "Number of rows in " << datapath << " : " << info.n_rows << endl;

        CSVReader reader(datapath.c_str(), format);
        // std::vector<std::string> col_names{reader.get_col_names()};
        // for (const auto& col : col_names)
        // {
        //     cout << col << " ";
        // }
        // cout << endl;

        for (const CSVRow& row : reader)
        {
            if (row.size() != 15)
            {
                continue;
            }
            
            if (row["category"].is_str())
            {
                std::string_view value = row["category"].get_sv();
                if (big_data_category.contains(value))
                {
                    big_data_category.at(value) += 1;
                }
                else
                {
                    big_data_category.try_emplace(value, 1);
                }
                
            }
            // big_data_category[row["category"].get_sv()] += 1;
            // big_data_maker[row["makerName"].get_sv()] += 1;
            // big_data_fuel[row["fuel"].get_sv()] += 1;
            
            // std::string_view makerName_field{row["category"].get_sv()};
            // std::string_view makerName_field{row["makerName"].get_sv()};
            // std::string_view fuel_field{row["fuel"].get_sv()};

        }

    }

    cout << "category: " << endl;
    for (const auto& item : big_data_category)
    {
        cout << "\t" << item.first << " : " << item.second << endl;
    }

    cout << "fuel: " << endl;
    for (const auto& item : big_data_fuel)
    {
        cout << "\t" << item.first << " : " << item.second << endl;
    }

    cout << "makerName: " << endl;
    for (const auto& item : big_data_maker)
    {
        cout << "\t" << item.first << " : " << item.second << endl;
    }

    //cout << "Hello World!: " << datapath << endl;
    return 0;
}