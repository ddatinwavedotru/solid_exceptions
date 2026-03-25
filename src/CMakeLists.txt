# Создание библиотеки
add_library(solid_exceptions
    datatypes.h datatypes.cpp
    interfaces.h interfaces.cpp
    implementations.h implementations.cpp
    adaptors.h adaptors.cpp
    commands.h commands.cpp
    library.h
    command_queue.h command_queue.cpp
)

# Интерфейс библиотеки (заголовки)
target_include_directories(solid_exceptions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

