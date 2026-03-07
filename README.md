# CS 216: Bitcoin Transaction Lab Assignment

## Team Information
* **Arnav Kumar**            - 240001013
* **Aryaman Awanish Tiwari** - 240001014
* **Ayush Singh Rana**       - 240001015
* **Hrishabh Mittal**        - 240001035

## Project Overview
This repository contains a C++ implementation for generating, signing, and broadcasting Bitcoin transactions on a local `regtest` network. It demonstrates the differences between Legacy (P2PKH) and SegWit (P2SH-P2WPKH) address formats, automating the extraction of locking/unlocking scripts and providing a data-driven comparison of transaction sizes (Bytes, vBytes, and Weight Units).

## Prerequisites
To run this project, ensure you have the following installed:
* **Docker & Docker Compose:** To run the isolated `bitcoind` regtest node.
* **C++20 Compiler (g++ or clang++):** For compiling the project.
* **Make:** For build automation.
* **libcurl:** Required for making HTTP POST requests to the Bitcoin RPC. 
* **btcdeb (Optional but recommended):** The Bitcoin Script Debugger, used for manual validation of the extracted transaction scripts.

## How to Run the Code

**1. Configure the Environment**
Copy the template environment file to create your active `.env` file. 
```bash
cp .env.example .env

```

**2. Start the Bitcoin Regtest Node**
Spin up the local Bitcoin daemon using Docker Compose. This automatically configures the network with the exact fee metrics required for the assignment.

```bash
docker compose up -d

```

**3. Build and Execute the Pipeline**
Use the provided Makefile command to compile all C++ binaries, run the Legacy and SegWit transaction workflows, and execute the final size comparison analysis sequentially.

```bash
make run

```

**4. Teardown**
Once finished, safely spin down the Docker container and clean up the compiled binaries.

```bash
docker compose down
make clean

```

## Repository Structure & File Descriptions

### Source Code (`src/`)

* **`legacyTxs.cpp`**: Executes Part 1. Generates Legacy (P2PKH) addresses, funds them, creates a transaction chain (A -> B -> C), and logs the size metrics to a CSV.
* **`segWitTxs.cpp`**: Executes Part 2. Performs the exact same workflow as Part 1, but utilizes P2SH-SegWit addresses (A' -> B' -> C') to demonstrate modern transaction structures.
* **`comparison.cpp`**: Executes Part 3. Parses the generated metrics CSV and outputs a formatted table comparing the sizes, vSizes, and weights, alongside an analytical conclusion.
* **`BitcoinRpcClient.cpp`**: The core implementation of the RAII-compliant HTTP client used to interface with the `bitcoind` JSON-RPC API via `libcurl`.

### Headers (`include/`)

* **`BitcoinRpcClient.hpp`**: Header definition for the RPC client.
* **`DotEnv.hpp`**: A lightweight, custom single-header utility to parse the `.env` file and load configuration parameters dynamically.
* **`json.hpp`**: The `nlohmann/json` library used for serializing and deserializing the JSON-RPC payloads.

### Configuration & Tooling

* **`docker-compose.yml`**: Container orchestration file that deploys `ruimarinho/bitcoin-core:latest` in `regtest` mode with hardcoded network fee parameters.
* **`Makefile`**: Automates the compilation of the shared object files and distinct binaries, ensuring smooth execution.
* **`.env.example`**: .env template.
