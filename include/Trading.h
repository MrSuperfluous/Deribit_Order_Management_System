#ifndef TRADING_H
#define TRADING_H

#include "Connection.h"
#include "rapidjson/document.h"
#include <unordered_map>
#include <optional>

class Trading {
public:
    Trading(Connection& conn);

    rapidjson::Document placeOrder(
        const std::string& token,
        const std::string& instrument,
        const std::string& type,
        double amount = 0.0,
        double price = 0.0,
        const std::string& label = "");

    rapidjson::Document modifyOrder(
        const std::string& order_id,
        const std::string& token,
        const std::optional<double>& amount = std::nullopt,
        const std::optional<double>& contracts = std::nullopt,
        const std::optional<double>& price = std::nullopt,
        const std::optional<std::string>& advanced = std::nullopt,
        const std::optional<bool>& post_only = std::nullopt,
        const std::optional<bool>& reduce_only = std::nullopt);

    rapidjson::Document sellOrder(
        const std::string& token,
        const std::string& instrument,
        const std::optional<double>& amount = std::nullopt,
        const std::optional<double>& contracts = std::nullopt,
        const std::optional<double>& price = std::nullopt,
        const std::optional<std::string>& type = std::nullopt,
        const std::optional<std::string>& trigger = std::nullopt,
        const std::optional<double>& trigger_price = std::nullopt);

    rapidjson::Document cancelOrder(const std::string& orderid, const std::string& token);
    rapidjson::Document cancelAllOrder(const std::string& token);
    rapidjson::Document getOpenOrder(const std::string& token);
    rapidjson::Document getOrderState(const std::string& orderid, const std::string& token);
    rapidjson::Document getOrderBook(const std::string& instrument_name);
    rapidjson::Document getPositions(const std::string& token);
    rapidjson::Document getPosition(const std::string& token,const std::string& instrument_name);
private:
    Connection& conn;
};

#endif // TRADING_H
