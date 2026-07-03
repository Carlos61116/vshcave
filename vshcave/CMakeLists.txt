include(FetchContent)
FetchContent_Declare(
    vshlog
    GIT_REPOSITORY https://github.com/Carlos61116/vshlog.git
    GIT_TAG main
)
FetchContent_MakeAvailable(vshlog)


include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(json)

add_executable(vshcave "src/vshcave.cpp" "include/vshcave/vshcave.h" "include/vshcave/vshjson.h" "src/vshjson.cpp" "include/vshcave/vshnoise.h" "src/vshnoise.cpp" "include/vshcave/vshrandom.h" "src/vshrandom.cpp")

target_link_libraries(vshcave PRIVATE vshlog)

target_link_libraries(vshcave PRIVATE nlohmann_json::nlohmann_json)

target_include_directories(vshcave PUBLIC include)

add_custom_command(
    TARGET vshcave POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/vshcave/data
        $<TARGET_FILE_DIR:vshcave>/data
)

set_property(TARGET vshcave PROPERTY CXX_STANDARD 20)
