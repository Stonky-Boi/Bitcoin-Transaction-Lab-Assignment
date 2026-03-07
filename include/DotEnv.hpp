#pragma once

#include <cstdlib>
#include <fstream>
#include <string>
#include <unordered_map>

class DotEnv {
  private:
    std::unordered_map<std::string, std::string> m_envMap;

  public:
    DotEnv(std::string filename = ".env") {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#')
                continue;

            auto delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                auto key = line.substr(0, delimiterPos);
                auto value = line.substr(delimiterPos + 1);
                m_envMap[key] = value;
            }
        }
    }

    std::string get(const std::string &key,
                    const std::string &defaultValue = "") const {
        if (m_envMap.find(key) != m_envMap.end()) {
            return m_envMap.at(key);
        }

        const char *envVal = std::getenv(key.c_str());
        if (envVal) {
            return std::string(envVal);
        }

        return defaultValue;
    }
};
