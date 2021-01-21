project(KubeObjectTests)

get_filename_component(KubeObjectTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeObjectTestsSources
    ${KubeObjectTestsDir}/tests_ObjectSignal.cpp
    ${KubeObjectTestsDir}/tests_ObjectTree.cpp
    ${KubeObjectTestsDir}/tests_TemplateReflection.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeObjectTestsSources})

add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeObject
    GTest::GTest GTest::Main
)

if(KF_COVERAGE)
    target_compile_options(${PROJECT_NAME} PUBLIC --coverage)
    target_link_options(${PROJECT_NAME} PUBLIC --coverage)
endif()