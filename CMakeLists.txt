cmake_minimum_required(VERSION 3.12)

project(ParserApp)

# for matplotlibcpp
find_package(Python3 REQUIRED
     COMPONENTS Interpreter Development NumPy
)

find_package(PythonLibs 3.0 REQUIRED)
include_directories(${PYTHON3_INCLUDE_DIRS} ${NumPy_INCLUDE_DIRS})

include(FetchContent)

FetchContent_Declare(
    csv  
    GIT_REPOSITORY https://github.com/vincentlaucsb/csv-parser.git
    GIT_SHALLOW TRUE 
    GIT_TAG 2.3.0
)

FetchContent_MakeAvailable(csv)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

if (NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/data/telangana_vehicle_sales.csv")
    message(STATUS "CSV file does not exists!!!!")

    execute_process(
        COMMAND mkdir data
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE KAGGLE_DATASET_RESULT)

    execute_process(
        COMMAND kaggle datasets download zubairatha/revving-up-telangana-vehicle-sales-2023
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data
        RESULT_VARIABLE KAGGLE_DATASET_RESULT)
    
    execute_process(
        COMMAND unzip *.zip
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data
        RESULT_VARIABLE KAGGLE_DATASET_RESULT)

    execute_process(
        COMMAND find . -type f ! -name "telangana_vehicle_sales.csv" -exec rm -rf {} \;
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data
        RESULT_VARIABLE KAGGLE_DATASET_RESULT
    )

    execute_process(
        COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/csv_splitter.sh ${CMAKE_CURRENT_BINARY_DIR}/data/telangana_vehicle_sales.csv
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data
        RESULT_VARIABLE KAGGLE_DATASET_RESULT
    )

    execute_process(
        COMMAND find . -type f -name "telangana_vehicle_sales.csv" -exec rm -rf {} \;
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/data
        RESULT_VARIABLE KAGGLE_DATASET_RESULT
    )

    if(NOT KAGGLE_DATASET_RESULT EQUAL "0")
        message(FATAL_ERROR "Getting of the dataset FAILED")
    endif()
else()
    message(STATUS "CSV file exists!!!!")
endif()

include_directories(SYSTEM ${matplotlib_SOURCE_DIR})

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories( ${PROJECT_NAME} PRIVATE csv)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(${PROJECT_NAME} ${PYTHON_LIBRARIES} Python3::NumPy csv "--coverage" nlohmann_json::nlohmann_json)
