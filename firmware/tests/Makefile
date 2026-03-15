CXX      ?= c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -O2 -I..

# macOS: clang needs an explicit SDK path and libc++ headers
UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
  SDK := $(shell xcrun --show-sdk-path 2>/dev/null)
  ifneq ($(SDK),)
    CXXFLAGS += -isysroot $(SDK) -I$(SDK)/usr/include/c++/v1
  endif
endif
TARGET   = test_algorithm

.PHONY: all run clean

all: $(TARGET)

$(TARGET): test_algorithm.cpp ../cerebral_oximeter/algorithm.h
	$(CXX) $(CXXFLAGS) -o $@ test_algorithm.cpp -lm

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
