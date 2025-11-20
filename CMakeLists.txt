cmake_minimum_required(VERSION 3.14)

project(Lab4_Synchronization)

set(CMAKE_CXX_STANDARD 17)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# === ОСНОВНЫЕ ПРОГРАММЫ ===
add_executable(Receiver receiver.cpp common.h)
add_executable(Sender sender.cpp common.h)

# Настройка папок вывода
set_target_properties(Receiver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(Sender PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# === ТЕСТОВЫЙ ПРОЕКТ ===
enable_testing()

add_executable(UnitTests tests.cpp common.h)

# Линкуем тесты с библиотекой gtest
target_link_libraries(UnitTests PRIVATE GTest::gtest GTest::gtest_main)

# Чтобы тесты тоже падали в папку bin
set_target_properties(UnitTests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Регистрация тестов для Test Explorer
include(GoogleTest)
gtest_discover_tests(UnitTests)