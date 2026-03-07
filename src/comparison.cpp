#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

namespace Color {
    constexpr std::string_view RESET      = "\033[0m";
    constexpr std::string_view BOLD       = "\033[1m";
    constexpr std::string_view CYBER_BLUE = "\033[38;2;0;229;255m";
}

int main() {
    std::ifstream metricsFile("build/metrics.csv");
    if (!metricsFile.is_open()) {
        std::cerr << "Error: Run legacyTxs.cpp and segWitTxs.cpp first to generate metrics.csv\n";
        return 1;
    }

    std::string line;
    std::vector<std::vector<std::string>> data;

    while (std::getline(metricsFile, line)) {
        std::stringstream ss(line);
        std::string cell;
        std::vector<std::string> row;
        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        data.push_back(row);
    }

    std::cout << Color::CYBER_BLUE << Color::BOLD << "\n=== Transaction Size Analysis ===" << Color::RESET << "\n\n";

    std::cout << std::left 
              << std::setw(25) << "Format" 
              << std::setw(15) << "Transaction" 
              << std::setw(15) << "Size(Bytes)" 
              << std::setw(15) << "VSize(vBytes)" 
              << std::setw(15) << "Weight(WU)" << "\n";
    std::cout << std::string(85, '-') << "\n";

    for (size_t i = 1; i < data.size(); ++i) {
        std::cout << std::left 
                  << std::setw(25) << data[i][0]
                  << std::setw(15) << data[i][1]
                  << std::setw(15) << data[i][2]
                  << std::setw(15) << data[i][3]
                  << std::setw(15) << data[i][4] << "\n";
    }

    return 0;
}
