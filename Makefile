# Compiler selection
CC := gcc

# Optimization level for debugging
OPTIMIZATION_FLAG := -Og

# Base compiler flags: debugging symbols, pedantic mode, treat warnings as errors, enable extra warnings
BASE_CFLAGS := -g -pedantic -Werror -Wall -Wextra $(OPTIMIZATION_FLAG)

# Sanitizer flags: empty for Windows, otherwise use AddressSanitizer, UndefinedBehaviorSanitizer, and LeakSanitizer
ifeq ($(OS),Windows_NT)
    SANITIZER_FLAGS :=
else
    SANITIZER_FLAGS := -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

# Directory structure
SRC_DIR := src
INCLUDE_DIR := include
TEST_SRC_DIR := tests
TESTLIB_SRC_DIR := ./tests/testlib

# Output directories for ASan (AddressSanitizer) and Valgrind builds
OUT_DIR := out
OUT_DIR_ASAN := $(OUT_DIR)/asan
OUT_DIR_VALGRIND := $(OUT_DIR)/valgrind
TEST_LIB_DIR := testlib
TEST_LIB_DIR_ASAN := $(TEST_LIB_DIR)/asan
TEST_LIB_DIR_VALGRIND := $(TEST_LIB_DIR)/valgrind

# Name of the test library
TESTLIB_SO := libtest.so

# Source files for the test library
TESTLIB_SRCS := $(TESTLIB_SRC_DIR)/asserts.c $(TESTLIB_SRC_DIR)/tests.c

# All test source files
TESTLIB_TEST_SRCS := $(wildcard $(TESTLIB_SRC_DIR)/*.c)
# Filter out the test library source files from all test sources
TESTLIB_TEST_SRCS := $(filter-out $(TESTLIB_SRCS), $(TESTLIB_TEST_SRCS))

# Function to generate paths for test executables
define test_executables
$(patsubst $(TESTLIB_SRC_DIR)/%.c,$(1)/%.test_internal,$(TESTLIB_TEST_SRCS))
endef

# Generate lists of test executables for ASan and Valgrind
TESTLIB_TEST_EXES_ASAN := $(call test_executables,$(OUT_DIR_ASAN))
TESTLIB_TEST_EXES_VALGRIND := $(call test_executables,$(OUT_DIR_VALGRIND))

# Base build commands for ASan and Valgrind
BASE_BUILD_COMMAND_ASAN := $(CC) $(BASE_CFLAGS) $(SANITIZER_FLAGS) -I $(INCLUDE_DIR)
BASE_BUILD_COMMAND_VALGRIND := $(CC) $(BASE_CFLAGS) -I $(INCLUDE_DIR)

VALGRIND_COMMAND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes

# Phony targets (targets that don't represent files)
.PHONY: all clean build_testlib_asan build_testlib_valgrind test_testlib_asan test_testlib_valgrind

# Default target: build both ASan and Valgrind binaries
all: build

build:
	$(error build is not supported yet)

# ASan Targets

# Create output directory for ASan
$(OUT_DIR_ASAN):
	mkdir -p $@

# Create library directory for ASan
$(TEST_LIB_DIR_ASAN):
	mkdir -p $@

# Build the test library with ASan
$(TEST_LIB_DIR_ASAN)/$(TESTLIB_SO): $(TESTLIB_SRCS) | $(TEST_LIB_DIR_ASAN)
	$(BASE_BUILD_COMMAND_ASAN) -I $(TESTLIB_SRC_DIR) -fPIC -shared -o $@ $^

# Build each test executable with ASan
$(OUT_DIR_ASAN)/%.test_internal: $(TESTLIB_SRC_DIR)/%.c | $(OUT_DIR_ASAN) $(TEST_LIB_DIR_ASAN)/$(TESTLIB_SO)
	$(BASE_BUILD_COMMAND_ASAN) -I $(TESTLIB_SRC_DIR) -o $@ $< -L$(TEST_LIB_DIR_ASAN) -ltest

# Build all ASan test executables
build_testlib_asan: $(TESTLIB_TEST_EXES_ASAN)
	@echo "ASan build completed."

# Run all ASan tests
test_testlib_asan: build_testlib_asan
	@printf "\n\nRunning ASan tests...\n\n"
	@failed_tests=""; \
	export LD_LIBRARY_PATH="$(shell pwd)/$(TEST_LIB_DIR_ASAN):${LD_LIBRARY_PATH}"; \
	$(foreach test,$(TESTLIB_TEST_EXES_ASAN), \
		printf "%s ... " $$(basename $$(basename $(test) .test_internal)); \
		if output=$$(./$(test) 2>&1); then \
			echo "Ok"; \
		else \
			echo "Fail"; \
			failed_tests="$$failed_tests\n$$(basename $$(basename $(test) .test_internal))\n$$output\n"; \
		fi; \
	) \
	if [ -n "$$failed_tests" ]; then \
		printf "\nFailed tests and their outputs:\n$$failed_tests"; \
		exit 1; \
	else \
		echo "All tests passed!"; \
	fi

# Valgrind Targets

# Create output directory for Valgrind
$(OUT_DIR_VALGRIND):
	mkdir -p $@

# Create library directory for Valgrind
$(TEST_LIB_DIR_VALGRIND):
	mkdir -p $@

# Build the test library for Valgrind (without ASan)
$(TEST_LIB_DIR_VALGRIND)/$(TESTLIB_SO): $(TESTLIB_SRCS) | $(TEST_LIB_DIR_VALGRIND)
	$(BASE_BUILD_COMMAND_VALGRIND) -I $(TESTLIB_SRC_DIR) -fPIC -shared -o $@ $^

# Build each test executable for Valgrind (without ASan)
$(OUT_DIR_VALGRIND)/%.test_internal: $(TESTLIB_SRC_DIR)/%.c | $(OUT_DIR_VALGRIND) $(TEST_LIB_DIR_VALGRIND)/$(TESTLIB_SO)
	$(BASE_BUILD_COMMAND_VALGRIND) -I $(TESTLIB_SRC_DIR) -o $@ $< -L$(TEST_LIB_DIR_VALGRIND) -ltest

# Build all Valgrind test executables
build_testlib_valgrind: $(TESTLIB_TEST_EXES_VALGRIND)
	@echo "Valgrind build successful."

# Run all tests under Valgrind
test_testlib_valgrind: build_testlib_valgrind
	@printf "\n\nRunning Valgrind tests...\n\n"
	@failed_tests=""; \
	export LD_LIBRARY_PATH="$(shell pwd)/$(TEST_LIB_DIR_VALGRIND):${LD_LIBRARY_PATH}"; \
	$(foreach test,$(TESTLIB_TEST_EXES_VALGRIND), \
		printf "%s ... " $$(basename $$(basename $(test) .test_internal)); \
		if output=$$($(VALGRIND_COMMAND) ./$(test)); then \
			echo "Ok"; \
		else \
			echo "Fail"; \
			failed_tests="$$failed_tests\n$$(basename $$(basename $(test) .test_internal))\n$$output\n"; \
		fi; \
	) \
	if [ -n "$$failed_tests" ]; then \
		printf "\nFailed tests and their outputs:\n$$failed_tests"; \
		exit 1; \
	else \
		echo "All tests passed!"; \
	fi

# Clean up build artifacts
clean:
	rm -rf $(OUT_DIR) $(TEST_LIB_DIR)
