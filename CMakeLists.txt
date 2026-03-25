cmake_minimum_required(VERSION 3.16)

project(solid_exceptions LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опция для включения coverage
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(STATUS "Enabling code coverage")

        # Для coverage используем Debug build
        if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
            message(STATUS "Forcing Debug build type for coverage")
            set(CMAKE_BUILD_TYPE Debug)
        endif()

        # Флаги для coverage
        add_compile_options(
            -g
            -O0
            -fprofile-arcs
            -ftest-coverage
            -fno-inline
            -fno-inline-small-functions
            -fno-default-inline
        )
        add_link_options(-fprofile-arcs -ftest-coverage)

        # Находим lcov и genhtml
        find_program(LCOV_PATH lcov)
        find_program(GENHTML_PATH genhtml)

        if(LCOV_PATH AND GENHTML_PATH)
            set(COVERAGE_AVAILABLE ON)
        else()
            message(WARNING "lcov or genhtml not found - coverage reports disabled")
        endif()
    else()
        message(WARNING "Code coverage is only supported for GCC and Clang")
    endif()
endif()

# Включение CTest
include(CTest)

# Скачивание GoogleTest
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download
)


# Добавление поддиректориев
add_subdirectory(src)
add_subdirectory(tests)

# Добавляем цель для генерации coverage отчета
if(ENABLE_COVERAGE AND COVERAGE_AVAILABLE)
    add_custom_target(coverage
        # Очистка предыдущих данных
        COMMAND ${LCOV_PATH} --directory . --zerocounters
        # Запуск тестов
        COMMAND $<TARGET_FILE:runTests>
        # Сбор данных
        COMMAND ${LCOV_PATH} --directory . --capture --output-file coverage.info
        # Удаление системных и тестовых файлов
        COMMAND ${LCOV_PATH} --remove coverage.info '*/usr/*' '*/tests/*' '*/extern/*' '*/_deps/*' '*/googletest/*' --output-file coverage.info
        # Генерация HTML отчета
        COMMAND ${GENHTML_PATH} coverage.info --output-directory coverage_report --demangle-cpp
        # Показать сводку
        COMMAND ${LCOV_PATH} --list coverage.info
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating code coverage report"
        DEPENDS runTests
    )
endif()

