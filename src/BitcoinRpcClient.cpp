#include <BitcoinRpcClient.hpp>
#include <curl/curl.h>
#include <memory>
#include <stdexcept>

struct CurlDeleter {
    void operator()(CURL *curlHandle) const {
        if (curlHandle)
            curl_easy_cleanup(curlHandle);
    }
};

struct CurlSlistDeleter {
    void operator()(curl_slist *slistHandle) const {
        if (slistHandle)
            curl_slist_free_all(slistHandle);
    }
};

using CurlPtr = std::unique_ptr<CURL, CurlDeleter>;
using CurlSlistPtr = std::unique_ptr<curl_slist, CurlSlistDeleter>;

BitcoinRpcClient::BitcoinRpcClient(std::string_view rpcUser,
                                   std::string_view rpcPassword,
                                   std::string_view rpcIp, int rpcPort)
    : m_rpcUser(rpcUser), m_rpcPassword(rpcPassword) {
    m_rpcUrl = "http://" + std::string(rpcIp) + ":" + std::to_string(rpcPort);
}

json BitcoinRpcClient::callMethod(std::string_view rpcMethod,
                                  const json &rpcParams) {
    CurlPtr curlHandle{curl_easy_init()};
    if (!curlHandle) {
        throw std::runtime_error("Failed to initialize CURL.");
    }

    json requestPayload = {{"jsonrpc", "1.0"},
                           {"id", "bitcoinLab"},
                           {"method", rpcMethod},
                           {"params", rpcParams}};
    std::string payloadStr = requestPayload.dump();

    curl_easy_setopt(curlHandle.get(), CURLOPT_URL, m_rpcUrl.c_str());
    curl_easy_setopt(curlHandle.get(), CURLOPT_POST, 1L);
    curl_easy_setopt(curlHandle.get(), CURLOPT_POSTFIELDS, payloadStr.c_str());

    std::string authStr = m_rpcUser + ":" + m_rpcPassword;
    curl_easy_setopt(curlHandle.get(), CURLOPT_USERPWD, authStr.c_str());

    CurlSlistPtr headerList{
        curl_slist_append(nullptr, "Content-Type: text/plain")};
    curl_easy_setopt(curlHandle.get(), CURLOPT_HTTPHEADER, headerList.get());

    std::string readBuffer;

    curl_easy_setopt(
        curlHandle.get(), CURLOPT_WRITEFUNCTION,
        +[](void *responseContents, size_t size, size_t nmemb,
            void *userPtr) -> size_t {
            auto *targetBuffer = static_cast<std::string *>(userPtr);
            targetBuffer->append(static_cast<char *>(responseContents),
                                 size * nmemb);
            return size * nmemb;
        });
    curl_easy_setopt(curlHandle.get(), CURLOPT_WRITEDATA, &readBuffer);

    if (auto curlRes = curl_easy_perform(curlHandle.get());
        curlRes != CURLE_OK) {
        throw std::runtime_error(std::string("CURL request failed: ") +
                                 curl_easy_strerror(curlRes));
    }

    json jsonResponse = json::parse(readBuffer);

    if (!jsonResponse["error"].is_null()) {
        throw std::runtime_error("RPC Error: " + jsonResponse["error"].dump());
    }

    return jsonResponse["result"];
}
//f3fd4fe2b505e2470e5908213f0a18b4a2e7a1aa82231dd17fefda30009b22ea
