CC := gcc
OPTIMIZATION_FLAG := -Og
CFLAGS := -g -pedantic -Werror -Wall -Wextra $(OPTIMIZATION_FLAG)

ifeq ($(OS),Windows_NT)
	SANITIZER_FLAGS :=
else
	SANITIZER_FLAGS := -fsanitize=address -fsanitize=undefined -fsanitize=leak
endif

SRC_DIR := src
OUT_DIR := out
INCLUDE_DIR := include
TEST_SRC_DIR := tests
TEST_LIB_DIR := testlib

TESTLIB_SRC_DIR := ./tests/testlib
TESTLIB_SO := libtest.so

TESTLIB_SRCS := $(TESTLIB_SRC_DIR)/asserts.c $(TESTLIB_SRC_DIR)/tests.c

TESTLIB_TEST_SRCS := $(wildcard $(TESTLIB_SRC_DIR)/*.c)
TESTLIB_TEST_SRCS := $(filter-out $(TESTLIB_SRCS), $(TESTLIB_TEST_SRCS))

TESTLIB_TEST_EXES :=  $(patsubst $(TESTLIB_SRC_DIR)/%.c,$(OUT_DIR)/%.test_internal,$(TESTLIB_TEST_SRCS))

TESTLIB_TEST_LDFLAGS := -L$(TEST_LIB_DIR) -l$(subst .so,,$(subst lib,,$(TESTLIB_SO)))

BASE_BUILD_COMMAND := $(CC) $(CFLAGS) $(SANITIZER_FLAGS) -I $(INCLUDE_DIR)

.PHONY: all

all: build

build:
	$(error Build is not yet supported)

build_test:
	$(error Build test is not yet supported)

build_test_internal: $(TESTLIB_TEST_EXES) | $(OUT_DIR)

test_internal: $(TESTLIB_TEST_EXES)
	for f in $^; do ./$$f; done


$(OUT_DIR):
	mkdir -p $@

$(TEST_LIB_DIR):
	mkdir -p $@

$(OUT_DIR)/%.test_internal: $(TESTLIB_SRC_DIR)/%.c | $(OUT_DIR) $(TEST_LIB_DIR)/$(TESTLIB_SO)
	$(BASE_BUILD_COMMAND) -I $(TESTLIB_SRC_DIR) $(TESTLIB_TEST_LDFLAGS) -o $@ $<


$(TEST_LIB_DIR)/$(TESTLIB_SO): $(TESTLIB_SRCS) | $(TEST_LIB_DIR)
	$(BASE_BUILD_COMMAND) -I $(TESTLIB_SRC_DIR) -fPIC -shared -o $@ $^

clean:
	rm -fr $(OUT_DIR)
