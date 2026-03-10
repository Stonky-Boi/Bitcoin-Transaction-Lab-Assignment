#include "BitcoinRpcClient.hpp"
#include "DotEnv.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

namespace Color {
constexpr std::string_view RESET = "\033[0m";
constexpr std::string_view BOLD = "\033[1m";
constexpr std::string_view BTC_ORANGE = "\033[38;2;247;147;26m";
constexpr std::string_view CYBER_BLUE = "\033[38;2;0;229;255m";
constexpr std::string_view NEON_GREEN = "\033[38;2;0;255;127m";
constexpr std::string_view HACKER_PURPLE = "\033[38;2;187;134;252m";
constexpr std::string_view ALERT_RED = "\033[38;2;255;76;76m";
} // namespace Color

json getUtxoForAddress(const json &unspentList,
                       std::string_view targetAddress) {
    for (const auto &utxo : unspentList) {
        if (utxo["address"] == targetAddress)
            return utxo;
    }
    throw std::runtime_error("No UTXO found for address: " +
                             std::string(targetAddress));
}

int main() {
    try {
        DotEnv envConfig(".env");
        std::string rpcUser = envConfig.get("RPC_USER", "defaultUser");
        std::string rpcPassword = envConfig.get("RPC_PASSWORD", "defaultPass");
        std::string rpcIp = envConfig.get("RPC_IP", "127.0.0.1");
        int rpcPort = std::stoi(envConfig.get("RPC_PORT", "18443"));

        BitcoinRpcClient btcClient{rpcUser, rpcPassword, rpcIp, rpcPort};

        auto miningAddress =
            btcClient
                .callMethod("getnewaddress", {"miningAddr_SegWit", "legacy"})
                .get<std::string>();

        std::cout << Color::CYBER_BLUE << Color::BOLD
                  << "\n=== P2SH-SegWit Addresses ===" << Color::RESET << '\n';
        auto addressA =
            btcClient
                .callMethod("getnewaddress", {"addressA_prime", "p2sh-segwit"})
                .get<std::string>();
        auto addressB =
            btcClient
                .callMethod("getnewaddress", {"addressB_prime", "p2sh-segwit"})
                .get<std::string>();
        auto addressC =
            btcClient
                .callMethod("getnewaddress", {"addressC_prime", "p2sh-segwit"})
                .get<std::string>();

        std::cout << "Address A': " << Color::NEON_GREEN << addressA
                  << Color::RESET << '\n'
                  << "Address B': " << Color::NEON_GREEN << addressB
                  << Color::RESET << '\n'
                  << "Address C': " << Color::NEON_GREEN << addressC
                  << Color::RESET << '\n';

        std::cout << Color::BTC_ORANGE
                  << "\nFunding Address A' with 2.0 BTC...\n"
                  << Color::RESET;
        btcClient.callMethod("sendtoaddress", {addressA, 2.0});
        btcClient.callMethod("generatetoaddress", {1, miningAddress});

        // Transaction A' -> B'
        std::cout << Color::CYBER_BLUE << Color::BOLD
                  << "\n=== Transaction A' to B' ===" << Color::RESET << '\n';
        auto unspentList = btcClient.callMethod("listunspent");
        auto utxoA = getUtxoForAddress(unspentList, addressA);

        json inputsA = {{{"txid", utxoA["txid"]}, {"vout", utxoA["vout"]}}};
        json outputsA = {{addressB, 1.999}};

        auto rawTxAB =
            btcClient.callMethod("createrawtransaction", {inputsA, outputsA})
                .get<std::string>();
        auto decodedTxAB =
            btcClient.callMethod("decoderawtransaction", {rawTxAB});

        std::cout
            << Color::BOLD
            << "Locking Script (ScriptPubKey Challenge) for B':\n"
            << Color::RESET << Color::HACKER_PURPLE
            << decodedTxAB["vout"][0]["scriptPubKey"]["asm"].get<std::string>()
            << Color::RESET << "\n\n";

        auto signedTxAB =
            btcClient.callMethod("signrawtransactionwithwallet", {rawTxAB});

        auto fullyDecodedAB = btcClient.callMethod(
            "decoderawtransaction", {signedTxAB["hex"].get<std::string>()});
        std::ofstream outFileAB("build/metrics.csv", std::ios::app);
        outFileAB << "SegWit (P2SH-P2WPKH),A'->B'," << fullyDecodedAB["size"]
                  << "," << fullyDecodedAB["vsize"] << ","
                  << fullyDecodedAB["weight"] << "\n";
        outFileAB.close();

        auto txidAB = btcClient
                          .callMethod("sendrawtransaction",
                                      {signedTxAB["hex"].get<std::string>()})
                          .get<std::string>();
        std::cout << "Broadcasted TX A'->B' with TXID: " << Color::NEON_GREEN
                  << txidAB << Color::RESET << '\n';
        btcClient.callMethod("generatetoaddress", {1, miningAddress});

        std::cout << Color::CYBER_BLUE << Color::BOLD
                  << "\n=== Transaction B' to C' ===" << Color::RESET << '\n';
        unspentList = btcClient.callMethod("listunspent");
        auto utxoB = getUtxoForAddress(unspentList, addressB);

        json inputsB = {{{"txid", utxoB["txid"]}, {"vout", utxoB["vout"]}}};
        json outputsB = {{addressC, 1.998}};

        auto rawTxBC =
            btcClient.callMethod("createrawtransaction", {inputsB, outputsB})
                .get<std::string>();
        auto signedTxBC =
            btcClient.callMethod("signrawtransactionwithwallet", {rawTxBC});
        auto decodedTxBC = btcClient.callMethod(
            "decoderawtransaction", {signedTxBC["hex"].get<std::string>()});

        std::ofstream outFileBC("build/metrics.csv", std::ios::app);
        outFileBC << "SegWit (P2SH-P2WPKH),B'->C'," << decodedTxBC["size"]
                  << "," << decodedTxBC["vsize"] << "," << decodedTxBC["weight"]
                  << "\n";
        outFileBC.close();

        std::cout
            << Color::BOLD << "Redeem Script (ScriptSig) from B':\n"
            << Color::RESET << Color::HACKER_PURPLE
            << decodedTxBC["vin"][0]["scriptSig"]["asm"].get<std::string>()
            << Color::RESET << '\n';

        std::cout << Color::BOLD << "\nWitness Data (Signature & Public Key):\n"
                  << Color::RESET;
        if (decodedTxBC["vin"][0].contains("txinwitness")) {
            for (const auto &witnessItem :
                 decodedTxBC["vin"][0]["txinwitness"]) {
                std::cout << Color::HACKER_PURPLE
                          << witnessItem.get<std::string>() << Color::RESET
                          << '\n';
            }
        } else {
            std::cout << Color::ALERT_RED << "No witness data found."
                      << Color::RESET << '\n';
        }

        auto txidBC = btcClient
                          .callMethod("sendrawtransaction",
                                      {signedTxBC["hex"].get<std::string>()})
                          .get<std::string>();
        std::cout << "\nBroadcasted TX B'->C' with TXID: " << Color::NEON_GREEN
                  << txidBC << Color::RESET << '\n';
        btcClient.callMethod("generatetoaddress", {1, miningAddress});

    } catch (const std::exception &exceptionCaught) {
        std::cerr << Color::ALERT_RED << Color::BOLD
                  << "Exception caught: " << Color::RESET
                  << exceptionCaught.what() << '\n';
    }
    return 0;
}
