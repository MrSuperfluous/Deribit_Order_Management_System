#ifndef SYSTEM_H
#define SYSTEM_H

#include "Trading.h"
#include "Connection.h"
#include "ThreadPool.h"
#include "rapidjson/document.h"
#include <vector>
class System {
public:
    System(Connection& conn, size_t threadCount);
     // Trading-related functions
    rapidjson::Document placeOrder(const std::string& token, const std::string& instrument, const std::string& type, double amount, double price, const std::string& label = "");
    std::vector<rapidjson::Document> placeOrdersAsync(const std::string &token,const std::vector<std::tuple<std::string, std::string, double, double, std::string>>& orderParams);
    rapidjson::Document modifyOrder(const std::string& order_id, const std::string& token, const std::optional<double>& amount = std::nullopt, const std::optional<double>& contracts = std::nullopt, const std::optional<double>& price = std::nullopt, const std::optional<std::string>& advanced = std::nullopt, const std::optional<bool>& post_only = std::nullopt, const std::optional<bool>& reduce_only = std::nullopt);
    rapidjson::Document sellOrder(const std::string& token, const std::string& instrument, const std::optional<double>& amount = std::nullopt, const std::optional<double>& contracts = std::nullopt, const std::optional<double>& price = std::nullopt, const std::optional<std::string>& type = std::nullopt, const std::optional<std::string>& trigger = std::nullopt, const std::optional<double>& trigger_price = std::nullopt);
    std::vector<rapidjson::Document> sellOrdersAsync(const std::string &token,const std::vector<std::tuple<std::string, std::optional<double>, std::optional<double>, std::optional<double>, std::optional<std::string>, std::optional<std::string>, std::optional<double>>>& orderParams);
    rapidjson::Document cancelOrder(const std::string& orderid, const std::string& token);
    std::vector<rapidjson::Document> cancelOrdersAsync(const std::vector<std::string>& orderParams,const std::string &token);
    rapidjson::Document cancelAllOrder(const std::string& token);
    rapidjson::Document getOpenOrder(const std::string& token);
    rapidjson::Document getOrderState(const std::string& orderid, const std::string& token);
    rapidjson::Document getInstruments(const std::string& currency);
    rapidjson::Document getOrderBook(const std::string& instrument_name);
    rapidjson::Document getPositions(const std::string& token);
    rapidjson::Document getPosition(const std::string& token, const std::string& instrument_name);
private:
    Connection& conn;
    Trading trading;
    ThreadPool threadPool;
};

#endif // SYSTEM_H
