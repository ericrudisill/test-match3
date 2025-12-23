# Match3Game Development Makefile
# Simplifies common build, test, and run commands

.PHONY: all build test run clean rebuild configure configure-test help

# Default target
all: build

# Configure and build the game
build:
	@mkdir -p build
	@cd build && cmake .. && cmake --build . -j$$(nproc)
	@cp -r assets build/ 2>/dev/null || true

# Configure and build with tests enabled
build-test:
	@mkdir -p build
	@cd build && cmake .. -DBUILD_TESTS=ON && cmake --build . -j$$(nproc)

# Run the game
run: build
	@cd build && ./Match3Game

# Run all tests
test: build-test
	@cd build && ctest --output-on-failure

# Run tests with a specific tag (e.g., make test-tag TAG=scoring)
test-tag: build-test
	@cd build && ./Match3Tests "[$(TAG)]"

# Run tests verbosely
test-verbose: build-test
	@cd build && ./Match3Tests --success

# Clean build artifacts
clean:
	@rm -rf build

# Clean and rebuild
rebuild: clean build

# Just configure (useful for IDE integration)
configure:
	@mkdir -p build
	@cd build && cmake ..

# Configure with tests
configure-test:
	@mkdir -p build
	@cd build && cmake .. -DBUILD_TESTS=ON

# Show available targets
help:
	@echo "Match3Game Build System"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  build        - Build the game (default)"
	@echo "  build-test   - Build with tests enabled"
	@echo "  run          - Build and run the game"
	@echo "  test         - Build and run all tests"
	@echo "  test-tag     - Run tests by tag (e.g., make test-tag TAG=scoring)"
	@echo "  test-verbose - Run tests with detailed output"
	@echo "  clean        - Remove build directory"
	@echo "  rebuild      - Clean and rebuild"
	@echo "  configure    - Just run cmake (for IDE integration)"
	@echo "  help         - Show this help message"
