# src/CMakeLists.txt
include(FetchContent)
message(STATUS "Try to find OpenESL by FetchContent")

set(OPENESL_USE_CURL4ESL    OFF)
set(OPENESL_USE_MHD4ESL     OFF)
set(OPENESL_USE_OPENGTX4ESL OFF)
set(OPENESL_USE_ODBC4ESL    OFF)
set(OPENESL_USE_SQLITE4ESL  OFF)
set(OPENESL_USE_ZSYSTEM4ESL ON)

FetchContent_Declare(
    openesl
    GIT_REPOSITORY https://github.com/slukasde/open-esl
    #GIT_TAG        v1.6.0
    GIT_SHALLOW    TRUE
    OVERRIDE_FIND_PACKAGE
)
find_package(openesl REQUIRED)

file(GLOB_RECURSE ${PROJECT_NAME}_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC openesl::openesl)
