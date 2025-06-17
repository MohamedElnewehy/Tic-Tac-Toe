#include <gtest/gtest.h>
#include <iostream>

// This file serves as the main test runner for all test suites
// Individual test files will be linked together to create comprehensive test coverage

int main(int argc, char** argv) {
    std::cout << "=== Tic Tac Toe Comprehensive Test Suite ===" << std::endl;
    std::cout << "Running all game component tests..." << std::endl;
    std::cout << std::endl;
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Configure test output
    ::testing::FLAGS_gtest_color = "yes";
    ::testing::FLAGS_gtest_print_time = true;
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    std::cout << std::endl;
    if (result == 0) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "The Tic Tac Toe game is ready for deployment!" << std::endl;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        std::cout << "Please review and fix the failing tests before deployment." << std::endl;
    }
    
    return result;
}
