#pragma once

#include "json.hpp"
#include <string>
#include <string_view>

using json = nlohmann::json;

class BitcoinRpcClient {
  private:
    std::string m_rpcUser;
    std::string m_rpcPassword;
    std::string m_rpcUrl;

  public:
    BitcoinRpcClient(std::string_view rpcUser, std::string_view rpcPassword,
                     std::string_view rpcIp = "127.0.0.1", int rpcPort = 18443);

    json callMethod(std::string_view rpcMethod,
                    const json &rpcParams = json::array());
};
