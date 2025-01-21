#include "System.h"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include "utils.h" 
#include <future>

// Constructor initializes the connection, trading object, and thread pool
System::System(Connection& conn, size_t threadCount) :
    conn(conn),
    trading(conn),
    threadPool(threadCount) {}

// Place a single order synchronously
rapidjson::Document System::placeOrder(const std::string &token, const std::string &instrument, const std::string &type, double amount, double price, const std::string &label)
{
    return trading.placeOrder(token, instrument, type, amount, price, label);
}

// Place multiple orders asynchronously using a thread pool
std::vector<rapidjson::Document> System::placeOrdersAsync(
    const std::string& token,
    const std::vector<std::tuple<std::string, std::string, double, double, std::string>>& orderParams) {

    std::vector<std::future<rapidjson::Document>> futures;
    futures.reserve(orderParams.size());

    for (const auto& params : orderParams) {
        // Capture variables by reference in lambda for efficiency
        futures.push_back(threadPool.enqueue([this, token, params]() mutable -> rapidjson::Document { 
            try {
                const auto& [instrument, type, amount, price, label] = params;
                return placeOrder(token, instrument, type, amount, price, label); 
            } catch (const std::exception& e) {
                std::cerr << "Error placing order: " << e.what() << std::endl;
                rapidjson::Document errorDoc;
                errorDoc.SetObject();
                rapidjson::Document::AllocatorType& allocator = errorDoc.GetAllocator();
                errorDoc.AddMember("error", rapidjson::StringRef(e.what()), allocator);
                return errorDoc; 
            }
        }));
    }

    std::vector<rapidjson::Document> results;
    results.reserve(futures.size());

    for (auto& future : futures) {
        try {
            results.emplace_back(std::move(future.get())); // Use emplace_back with move for efficiency
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving order result: " << e.what() << std::endl;
            rapidjson::Document errorDoc;
            errorDoc.SetObject();
            rapidjson::Document::AllocatorType& allocator = errorDoc.GetAllocator();
            errorDoc.AddMember("error", rapidjson::StringRef(e.what()), allocator);
            results.emplace_back(std::move(errorDoc)); 
        }
    }

    return results;
}

// Modify an existing order
rapidjson::Document System::modifyOrder(const std::string &order_id, const std::string &token, const std::optional<double> &amount, const std::optional<double> &contracts, const std::optional<double> &price, const std::optional<std::string> &advanced, const std::optional<bool> &post_only, const std::optional<bool> &reduce_only)
{
    return trading.modifyOrder(order_id, token, amount, contracts, price, advanced, post_only, reduce_only);
}

// Place a single sell order synchronously
rapidjson::Document System::sellOrder(const std::string &token, const std::string &instrument, const std::optional<double> &amount, const std::optional<double> &contracts, const std::optional<double> &price, const std::optional<std::string> &type, const std::optional<std::string> &trigger, const std::optional<double> &trigger_price)
{
    return trading.sellOrder(token, instrument, amount, contracts, price, type, trigger, trigger_price);
}

// Place multiple sell orders asynchronously using a thread pool
std::vector<rapidjson::Document> System::sellOrdersAsync(
    const std::string& token,
    const std::vector<std::tuple<std::string, std::optional<double>, std::optional<double>, std::optional<double>, std::optional<std::string>, std::optional<std::string>, std::optional<double>>>& orderParams) {

    std::vector<std::future<rapidjson::Document>> futures;
    futures.reserve(orderParams.size());

    for (const auto& params : orderParams) {
        futures.push_back(threadPool.enqueue([this, token, params]() mutable -> rapidjson::Document { 
            try {
                const auto& [instrument, amount, contracts, price, type, trigger, trigger_price] = params;
                return sellOrder(token, instrument, amount, contracts, price, type, trigger, trigger_price); 
            } catch (const std::exception& e) {
                std::cerr << "Error placing sell order: " << e.what() << std::endl;
                rapidjson::Document errorDoc;
                errorDoc.SetObject();
                rapidjson::Document::AllocatorType& allocator = errorDoc.GetAllocator();
                errorDoc.AddMember("error", rapidjson::StringRef(e.what()), allocator);
                return errorDoc; 
            }
        }));
    }

    std::vector<rapidjson::Document> results;
    results.reserve(futures.size());

    for (auto& future : futures) {
        try {
            results.push_back(std::move(future.get())); // Use std::move to avoid unnecessary copies
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving sell order result: " << e.what() << std::endl;
            rapidjson::Document errorDoc;
            errorDoc.SetObject();
            rapidjson::Document::AllocatorType& allocator = errorDoc.GetAllocator();
            errorDoc.AddMember("error", rapidjson::StringRef(e.what()), allocator);
            results.emplace_back(std::move(errorDoc)); 
        }
    }

    return results;
}

// Cancel a single order synchronously
rapidjson::Document System::cancelOrder(const std::string &orderid, const std::string &token)
{
    return trading.cancelOrder(orderid, token);
}

// Cancel multiple orders asynchronously using a thread pool
std::vector<rapidjson::Document> System::cancelOrdersAsync(
    const std::vector<std::string>& orderParams,
    const std::string& token) {

    std::vector<std::future<rapidjson::Document>> futures;
    futures.reserve(orderParams.size());

    for (const auto& params : orderParams) {
        futures.push_back(threadPool.enqueue([this, token, params]() mutable -> rapidjson::Document { 
            try {
                const auto& orderid = params;
                return cancelOrder(orderid, token); 
            } catch (const std::exception& e) {
                std::cerr << "Error canceling order: " << e.what() << std::endl;
                rapidjson::Document errorDoc;
                errorDoc.SetObject();
                rapidjson::Document::AllocatorType& allocator = errorDoc.GetAllocator();
                errorDoc.AddMember("error", rapidjson::StringRef(e.what()), allocator);
                return errorDoc; 
            }
        }));
    }

    std::vector<rapidjson::Document> results;
    results.reserve(futures.size());

    for (auto& future : futures) {
        try {
            results.push_back(std::move(future.get())); // Use std::move to avoid unnecessary copies
        } catch (const std::exception& e) {
            std::cerr << "Error retrieving cancel order result: " << e.what() << std::endl;
            rapidjson::Document errorDoc;
            errorDoc.SetObject();
            rapidjson::Document::AllocatorType& allocator = errorDoc.GetAllocator();
            errorDoc.AddMember("error", rapidjson::StringRef(e.what()), allocator);
            results.emplace_back(std::move(errorDoc)); 
        }
    }

    return results;
}

// Cancel all open orders
rapidjson::Document System::cancelAllOrder(const std::string &token)
{
    return trading.cancelAllOrder(token);
}

// Get all open orders
rapidjson::Document System::getOpenOrder(const std::string &token)
{
    return trading.getOpenOrder(token);
}
rapidjson::Document System::getOrderState(const std::string &orderid, const std::string &token)
{
    return trading.getOrderState(orderid, token);
}
// Get user trades by order
rapidjson::Document System::getOrderBook(const std::string& instrument_name) {
    return trading.getOrderBook(instrument_name);
}
// Get user trades by order
rapidjson::Document System::getPositions(const std::string &token)
{
    return trading.getPositions(token);
}
// Get user trades by order
rapidjson::Document System::getPosition(const std::string &token, const std::string &currency)
{
    return trading.getPosition(token, currency);
}
