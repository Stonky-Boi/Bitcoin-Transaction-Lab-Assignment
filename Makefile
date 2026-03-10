CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -Iinclude
LDFLAGS = -lcurl

SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build

ifeq ($(OS),Windows_NT)
	SHELL := cmd.exe
	EXT = .exe

	MKDIR_OBJ = if not exist "$(subst /,\,$(OBJ_DIR))" mkdir "$(subst /,\,$(OBJ_DIR))"
	MKDIR_BIN = if not exist "$(subst /,\,$(BIN_DIR))" mkdir "$(subst /,\,$(BIN_DIR))"
	RM_DIR = if exist "$(subst /,\,$(BIN_DIR))" rmdir /s /q "$(subst /,\,$(BIN_DIR))"
	

	EXEC_LEGACY = $(subst /,\,$(BIN_DIR)\legacyTxs$(EXT))
	EXEC_SEGWIT = $(subst /,\,$(BIN_DIR)\segWitTxs$(EXT))
	EXEC_COMP = $(subst /,\,$(BIN_DIR)\comparison$(EXT))
	
	# I dont know how this line works Gemini came up with it. Copies all the relavent dlls to build folder.
	COPY_DLLS = powershell -NoProfile -Command "$$binDir = Split-Path (Get-Command g++.exe).Source; $$dlls = @('libcurl*.dll', 'libcrypto*.dll', 'libssl*.dll', 'zlib*.dll', 'libidn2*.dll', 'libnghttp2*.dll', 'libssh2*.dll', 'libpsl*.dll', 'libbrotli*.dll', 'libzstd*.dll', 'libunistring*.dll', 'libiconv*.dll'); foreach ($$dll in $$dlls) { Copy-Item -Path \"$$binDir\$$dll\" -Destination \"$(subst /,\,$(BIN_DIR))\" -ErrorAction SilentlyContinue }"
else
	EXT = .out

	MKDIR_OBJ = mkdir -p $(OBJ_DIR)
	MKDIR_BIN = mkdir -p $(BIN_DIR)
	RM_DIR = rm -rf $(BIN_DIR)

	EXEC_LEGACY = ./$(BIN_DIR)/legacyTxs$(EXT)
	EXEC_SEGWIT = ./$(BIN_DIR)/segWitTxs$(EXT)
	EXEC_COMP = ./$(BIN_DIR)/comparison$(EXT)
	
	COPY_DLLS = @echo ">>> Linux detected. DLL copy skipped."
endif

RPC_OBJ = $(OBJ_DIR)/BitcoinRpcClient.o

LEGACY_TXS = $(BIN_DIR)/legacyTxs$(EXT)
SEGWIT_TXS = $(BIN_DIR)/segWitTxs$(EXT)
COMP_TXS = $(BIN_DIR)/comparison$(EXT)

all: directories $(LEGACY_TXS) $(SEGWIT_TXS) $(COMP_TXS) copy-dlls

directories:
	@$(MKDIR_OBJ)
	@$(MKDIR_BIN)

$(RPC_OBJ): $(SRC_DIR)/BitcoinRpcClient.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LEGACY_TXS): $(SRC_DIR)/legacyTxs.cpp $(RPC_OBJ)
	$(CXX) $(CXXFLAGS) $< $(RPC_OBJ) -o $@ $(LDFLAGS)

$(SEGWIT_TXS): $(SRC_DIR)/segWitTxs.cpp $(RPC_OBJ)
	$(CXX) $(CXXFLAGS) $< $(RPC_OBJ) -o $@ $(LDFLAGS)

$(COMP_TXS): $(SRC_DIR)/comparison.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

copy-dlls:
	@echo ">>> Resolving and copying MinGW DLL dependencies to build folder..."
	@$(COPY_DLLS)

run: all
	@echo ">>> Executing Part 1 (Legacy)..."
	@$(EXEC_LEGACY)
	@echo ">>> Executing Part 2 (SegWit)..."
	@$(EXEC_SEGWIT)
	@echo ">>> Running Final Comparison ..."
	@$(EXEC_COMP)

clean:
	@$(RM_DIR)

.PHONY: all directories copy-dlls run clean