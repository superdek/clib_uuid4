LIB_NAME := uuid4
LIB_VERSION := 1.0.0
LIB_PROCESSOR := 64
LIB_PLATFORM := linux
LIB_COMPILER := gcc11
LIB_STANDARD := c17
LIB_MAJOR_VER := $(firstword $(subst ., ,$(LIB_VERSION)))

INCLUDE_CPATHS := $(CURDIR)/usr/include
LIBRARY_LDPATHS := $(CURDIR)/usr/lib
LIBRARY_LDNAMES := syserr

COMMON_CFLAGS := \
	-std=$(LIB_STANDARD) -m$(LIB_PROCESSOR)  \
	-Wall -Wextra \
	-Wno-discarded-qualifiers \
	$(patsubst %, -I%, $(INCLUDE_CPATHS))
COMMON_LDFLAGS := \
	$(patsubst %, -L%, $(LIBRARY_LDPATHS)) \
	$(patsubst %, -l%, $(LIBRARY_LDNAMES))

HDR_SUFFIX := .h
SRC_SUFFIX := .c
OBJ_SUFFIX := .o
HDR_DIR := src
SRC_DIR := src
OBJ_DIR := obj

GCC := gcc

.PHONY: all shared

LIB_NAME_PREFIX := lib
LIB_SHARED_SUFFIX := .so
LIB_SHARED_FILE := $(patsubst %,$(LIB_NAME_PREFIX)%$(LIB_SHARED_SUFFIX),$(LIB_NAME))

LIB_CFLAGS := \
	-fPIC \
	$(COMMON_CFLAGS)
LIB_LDFLAGS := \
	-shared \
	$(COMMON_LDFLAGS) \
	-Wl,-rpath=usr/lib

LIB_SRC_FILES := \
	src/uuid4.c
LIB_OBJ_FILES := $(patsubst $(SRC_DIR)/%$(SRC_SUFFIX), $(OBJ_DIR)/%$(OBJ_SUFFIX), $(LIB_SRC_FILES))

all: check shared

shared: $(LIB_SHARED_FILE)

$(LIB_SHARED_FILE): $(LIB_OBJ_FILES)
	$(GCC) $^ -o $@ $(LIB_LDFLAGS)

$(OBJ_DIR)/%$(OBJ_SUFFIX): $(SRC_DIR)/%$(SRC_SUFFIX)
	mkdir -p $(OBJ_DIR)
	$(GCC) $(LIB_CFLAGS) -c $< -o $@

.PHONY: archive

HEADER_FILES := \
	src/uuid4.h

ARCHIVE_DIR := archive
ADIR_INCLUDE_DIR := include
ADIR_LIBRARY_DIR := lib
ARCHIVE_FILE := $(LIB_NAME)$(LIB_VERSION)-$(LIB_PROCESSOR)bit-$(LIB_PLATFORM)-$(LIB_COMPILER)-$(LIB_STANDARD).tar.gz

archive: ;
	rm -rf $(ARCHIVE_DIR)
	mkdir -p \
		$(ARCHIVE_DIR) \
		$(ARCHIVE_DIR)/$(ADIR_INCLUDE_DIR) \
		$(ARCHIVE_DIR)/$(ADIR_LIBRARY_DIR)
	cp -a \
		$(HEADER_FILES) \
		$(ARCHIVE_DIR)/$(ADIR_INCLUDE_DIR)
	cp -a \
		$(LIB_SHARED_FILE) \
		$(ARCHIVE_DIR)/$(ADIR_LIBRARY_DIR)
	rm -rf $(ARCHIVE_FILE)
	tar -czvf \
		$(ARCHIVE_FILE) \
		-C $(ARCHIVE_DIR) \
		$(ADIR_INCLUDE_DIR) $(ADIR_LIBRARY_DIR)

.PHONY: test

TEST_SRC_FILES := \
	test/main.c

TEST_LIBRARY_LDNAMES := tester

TEST_CFLAGS := \
	$(COMMON_CFLAGS) \
	-g
TEST_LDFLAGS := \
	$(COMMON_LDFLAGS) \
	$(patsubst %, -l%, $(TEST_LIBRARY_LDNAMES)) \
	$(foreach path, $(LIBRARY_LDPATHS), -Wl,-rpath=$(path))

TEST_TARGET := test_program

test: check $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC_FILES)
	$(GCC) $(TEST_CFLAGS) $^ -o $@ $(TEST_LDFLAGS)

.PHONY: check

GCC_VERSION := $(shell $(GCC) -dumpversion)

check:
	@if [ $(GCC)$(GCC_VERSION) \!= $(LIB_COMPILER) ]; then \
    echo "GCC version must be equal to $(LIB_COMPILER)."; \
    exit 1; \
  fi

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR) \
	$(LIB_SHARED_FILE) \
	$(TEST_TARGET) \
	$(ARCHIVE_DIR) \
	$(ARCHIVE_FILE)