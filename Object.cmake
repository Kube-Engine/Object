project(KubeObject)

get_filename_component(KubeObjectDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeObjectSources
    ${KubeObjectDir}/Object.hpp
    ${KubeObjectDir}/Object.ipp
    ${KubeObjectDir}/Tree.hpp
    ${KubeObjectDir}/Tree.ipp
    ${KubeObjectDir}/Runtime.hpp
    ${KubeObjectDir}/Reflection.hpp
    ${KubeObjectDir}/Reflection.cpp
    ${KubeObjectDir}/Register.hpp
    ${KubeObjectDir}/Make.hpp
)

add_library(${PROJECT_NAME} ${KubeObjectSources})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    KubeMeta
)

if(${KF_TESTS})
    include(${KubeObjectDir}/Tests/ObjectTests.cmake)
endif()

if(${KF_BENCHMARKS})
    include(${KubeObjectDir}/Benchmarks/ObjectBenchmarks.cmake)
endif()