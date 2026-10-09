CXX = g++
CXXFLAGS = -O3 -mavx2 -std=c++20 -Wall -Wextra -Iinclude

TARGET = bda_das_lego_engine
SRC = src/bda_das_lego_engine.cpp
TEST_TARGET = test_bda
TEST_SRC = tests/test_bda_ahj_lego_cortex.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET)
