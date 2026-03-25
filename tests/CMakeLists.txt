# Поиск GoogleTest
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/../googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/../googletest-build)

# Создание тестового исполняемого файла
add_executable(runTests tests.cpp)

# Линковка с GoogleTest и нашей библиотекой
target_link_libraries(runTests PRIVATE
    GTest::gtest_main 
    solid_exceptions
)

# Добавление теста в CTest
include(GoogleTest)
gtest_discover_tests(runTests)

# Для coverage добавляем флаги компиляции
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(runTests PRIVATE --coverage)
    target_link_libraries(runTests PRIVATE --coverage)
endif()
