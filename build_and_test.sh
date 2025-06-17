#!/bin/bash

# Tic Tac Toe Test Suite Build and Test Script
# This script compiles and runs all test executables

set -e  # Exit on any error

# Configuration
BUILD_DIR="build"
BUILD_TYPE="Debug"
PARALLEL_JOBS=$(nproc 2>/dev/null || echo "4")

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check dependencies
check_dependencies() {
    print_status "Checking dependencies..."
    
    local missing_deps=()
    
    if ! command -v cmake >/dev/null 2>&1; then
        missing_deps+=("cmake")
    fi
    
    if ! command -v g++ >/dev/null 2>&1; then
        missing_deps+=("g++")
    fi
    
    if ! command -v make >/dev/null 2>&1; then
        missing_deps+=("make")
    fi
    
    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "Missing dependencies: ${missing_deps[*]}"
        print_error "Please install the missing dependencies and try again."
        exit 1
    fi
    
    print_success "All dependencies are available"
}

# Function to build the project
build_project() {
    print_status "Configuring and building project..."
    
    # Create build directory
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configure with CMake
    print_status "Running CMake configuration..."
    if cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE"; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
    
    # Build the project
    print_status "Building all test executables..."
    if cmake --build . --config "$BUILD_TYPE" --parallel "$PARALLEL_JOBS"; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
    
    cd ..
}

# Function to run tests
run_tests() {
    print_status "Running test suite..."
    
    cd "$BUILD_DIR"
    
    # List of test executables
    local tests=(
        "bin/core_game_tests"
        "bin/overwrite_mode_tests"
        "bin/ai_engine_tests"
        "bin/database_tests"
        "bin/all_tests"
    )
    
    local passed_tests=0
    local failed_tests=0
    
    for test in "${tests[@]}"; do
        if [ -x "$test" ]; then
            print_status "Running ${test##*/}..."
            if ./"$test" --gtest_brief=1; then
                print_success "✓ ${test##*/} PASSED"
                ((passed_tests++))
            else
                print_error "✗ ${test##*/} FAILED"
                ((failed_tests++))
            fi
            echo ""
        else
            print_warning "Test executable not found: $test"
            ((failed_tests++))
        fi
    done
    
    cd ..
    
    # Print summary
    echo "======================================"
    echo "TEST SUMMARY"
    echo "======================================"
    print_success "Passed: $passed_tests"
    if [ $failed_tests -gt 0 ]; then
        print_error "Failed: $failed_tests"
        return 1
    else
        print_success "All tests passed!"
        return 0
    fi
}

# Main execution
main() {
    echo "========================================"
    echo "Tic Tac Toe Test Suite Builder"
    echo "========================================"
    echo ""
    
    check_dependencies
    build_project
    run_tests
    
    if [ $? -eq 0 ]; then
        print_success "All tests completed successfully!"
        echo ""
        print_status "Test executables available in $BUILD_DIR/bin/:"
        echo "  • core_game_tests      - Core game logic tests"
        echo "  • overwrite_mode_tests - Overwrite mode tests"
        echo "  • ai_engine_tests      - AI engine tests"
        echo "  • database_tests       - Database tests"
        echo "  • all_tests            - Combined test suite"
    else
        print_error "Some tests failed. Please check the output above."
        exit 1
    fi
}

# Run main function
main "$@"
