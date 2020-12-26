project(KubeObjectBenchmarks)

get_filename_component(KubeObjectBenchmarksDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeObjectBenchmarksSources
    ${KubeObjectBenchmarksDir}/Main.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeObjectBenchmarksSources})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeObject
    benchmark::benchmark
)