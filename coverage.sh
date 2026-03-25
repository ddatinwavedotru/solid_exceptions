#!/bin/bash

set -e

echo "Cleaning previous builds..."
rm -rf build
mkdir -p build
cd build

echo "Configuring project with coverage..."
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..

echo "Building project..."
make -j4

echo "Running tests..."
./tests/runTests || true

echo "Generating coverage report with gcovr..."

# Проверяем доступность gcovr
if command -v gcovr >/dev/null 2>&1; then
    gcovr -r .. \
          --filter ../src/ \
          --exclude ../tests/ \
          --exclude ../extern/ \
          --exclude ../_deps/ \
          --html --html-details -o coverage_report.html \
          --print-summary
    
    # Генерируем XML для Codecov
    gcovr -r .. \
          --filter ../src/ \
          --exclude ../tests/ \
          --exclude ../extern/ \
          --exclude ../_deps/ \
          --xml > coverage.xml
          
    echo "==========================================="
    echo "Coverage report generated:"
    echo "HTML: build/coverage_report.html"
    echo "XML: build/coverage.xml"
    echo "==========================================="
else
    echo "ERROR: gcovr not found. Please install gcovr."
    echo "On Ubuntu/Debian: sudo apt-get install gcovr"
    exit 1
fi