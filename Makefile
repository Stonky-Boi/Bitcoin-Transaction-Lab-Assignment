CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -Iinclude
LDFLAGS = -lcurl

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build

RPC_OBJ = $(OBJ_DIR)/BitcoinRpcClient.o

LEGACY_TXS = $(BIN_DIR)/legacyTxs.out
SEGWIT_TXS = $(BIN_DIR)/segWitTxs.out
COMP_TXS = $(BIN_DIR)/comparison.out

all: directories $(LEGACY_TXS) $(SEGWIT_TXS) $(COMP_TXS)

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

$(RPC_OBJ): $(SRC_DIR)/BitcoinRpcClient.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LEGACY_TXS): $(SRC_DIR)/legacyTxs.cpp $(RPC_OBJ)
	$(CXX) $(CXXFLAGS) $< $(RPC_OBJ) -o $@ $(LDFLAGS)

$(SEGWIT_TXS): $(SRC_DIR)/segWitTxs.cpp $(RPC_OBJ)
	$(CXX) $(CXXFLAGS) $< $(RPC_OBJ) -o $@ $(LDFLAGS)

$(COMP_TXS): $(SRC_DIR)/comparison.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run: all
	@echo ">>> Executing Part 1 (Legacy)..."
	@./$(LEGACY_TXS)
	@echo ">>> Executing Part 2 (SegWit)..."
	@./$(SEGWIT_TXS)
	@echo ">>> Running Final Comparison ..."
	@./$(COMP_TXS)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all directories run clean
