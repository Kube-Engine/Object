project(KubeObject)

get_filename_component(KubeObjectDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeObjectSources
    ${KubeObjectDir}/Make.hpp
    ${KubeObjectDir}/Reflection.hpp
    ${KubeObjectDir}/Reflection.cpp
)

add_library(${PROJECT_NAME} ${KubeObjectSources})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    KubeCore
    KubeMeta
)

if(${KF_TESTS})
    include(${KubeObjectDir}/Tests/ObjectTests.cmake)
endif()

if(${KF_BENCHMARKS})
    include(${KubeObjectDir}/Benchmarks/ObjectBenchmarks.cmake)
endif()