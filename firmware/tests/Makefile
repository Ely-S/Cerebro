SDK      = $(shell xcrun --show-sdk-path 2>/dev/null)
CXX      = $(shell xcrun -f clang++ 2>/dev/null || echo clang++)
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -I.. \
           -isysroot $(SDK) \
           -I$(SDK)/usr/include/c++/v1
TARGET   = test_algorithm

.PHONY: all run clean

all: $(TARGET)

$(TARGET): test_algorithm.cpp ../cerebral_oximeter/algorithm.h
	$(CXX) $(CXXFLAGS) -o $@ test_algorithm.cpp -lm

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
