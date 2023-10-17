LIB_NAME := uuid4
LIB_VERSION := 2.0.0
LIB_PROCESSOR := x86_64
LIB_PLATFORM := linux
LIB_COMPILER := gcc11
LIB_STANDARD := c17
LIB_MAJOR_VER := $(firstword $(subst ., ,$(LIB_VERSION)))

INCLUDE_CPATHS := $(CURDIR)/usr/include
LIBRARY_LDPATHS := $(CURDIR)/usr/lib
LIBRARY_LDNAMES := utils

COMMON_CFLAGS := \
	-std=$(LIB_STANDARD) \
	-Wall -Wextra \
	-Iinclude \
	$(patsubst %, -I%, $(INCLUDE_CPATHS))
COMMON_LDFLAGS := \
	$(patsubst %, -L%, $(LIBRARY_LDPATHS)) \
	$(patsubst %, -l%, $(LIBRARY_LDNAMES))

HDR_SUFFIX := .h
SRC_SUFFIX := .c
OBJ_SUFFIX := .o
HDR_DIR := include
SRC_DIR := src
OBJ_DIR := obj

GCC := gcc

.PHONY: all shared

LIB_NAME_PREFIX := lib
LIB_SHARED_SUFFIX := .so
LIB_SHARED_FILE := $(patsubst %, $(LIB_NAME_PREFIX)%$(LIB_SHARED_SUFFIX), $(LIB_NAME))

LIB_CFLAGS := \
	-fPIC \
	$(COMMON_CFLAGS)
LIB_LDFLAGS := \
	-shared \
	$(COMMON_LDFLAGS) \
	-Wl,-rpath=usr/lib

LIB_SRC_FILES := uuid4.c
LIB_OBJ_FILES := $(patsubst %$(SRC_SUFFIX), %$(OBJ_SUFFIX), $(LIB_SRC_FILES))

all: check shared

shared: $(LIB_SHARED_FILE)

$(LIB_SHARED_FILE): $(patsubst %, $(OBJ_DIR)/%, $(LIB_OBJ_FILES))
	$(GCC) $^ -o $@ $(LIB_LDFLAGS)

$(OBJ_DIR)/%$(OBJ_SUFFIX): $(SRC_DIR)/%$(SRC_SUFFIX)
	mkdir -p $(dir $@)
	$(GCC) $(LIB_CFLAGS) -c $< -o $@

.PHONY: archive

LIBRARY_DIR := lib
ARCHIVE_DIR := archive
ARCHIVE_NAME := $(LIB_NAME)$(LIB_VERSION)-$(LIB_PROCESSOR)bit-$(LIB_PLATFORM)-$(LIB_COMPILER)-$(LIB_STANDARD)
ARCHIVE_FILE := $(ARCHIVE_NAME).tar.gz

archive: ;
	@if ! [ -f $(LIB_SHARED_FILE) ]; then \
		echo "file $(LIB_SHARED_FILE) does not exist."; \
		exit 1; \
	fi
	rm -rf $(ARCHIVE_DIR)/$(ARCHIVE_NAME)
	mkdir -p \
		$(ARCHIVE_DIR) \
		$(ARCHIVE_DIR)/$(ARCHIVE_NAME) \
		$(ARCHIVE_DIR)/$(ARCHIVE_NAME)/$(LIBRARY_DIR)
	cp -a \
		$(HDR_DIR) \
		$(ARCHIVE_DIR)/$(ARCHIVE_NAME)
	cp -a \
		$(LIB_SHARED_FILE) \
		$(ARCHIVE_DIR)/$(ARCHIVE_NAME)/$(LIBRARY_DIR)
	tar -czvf $(ARCHIVE_FILE) -C $(ARCHIVE_DIR) $(ARCHIVE_NAME)

.PHONY: test

TEST_LIBRARY_LDNAMES := 

TEST_DIR := test

TEST_CFLAGS := \
	$(COMMON_CFLAGS) \
	-g
TEST_LDFLAGS := \
	$(COMMON_LDFLAGS) \
	$(patsubst %, -l%, $(TEST_LIBRARY_LDNAMES)) \
	$(foreach path, $(LIBRARY_LDPATHS), -Wl,-rpath=$(path))

TEST_SRC_FILES := main.c

TEST_TARGET_FILE := test_program

test: check $(TEST_TARGET_FILE)

$(TEST_TARGET_FILE): $(patsubst %, $(TEST_DIR)/%, $(TEST_SRC_FILES))
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
	$(TEST_TARGET_FILE) \
	$(ARCHIVE_DIR) \
	$(ARCHIVE_FILE)