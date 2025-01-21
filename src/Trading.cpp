#include "Trading.h"

// Constructor initializes the connection object
Trading::Trading(Connection& conn) : conn(conn) {}

// Place an order 
// - Takes parameters for token, instrument, type, amount, price, and label
// - Constructs the request URL and parameters
// - Sends the request using the connection object
rapidjson::Document Trading::placeOrder(
    const std::string& token, 
    const std::string& instrument, 
    const std::string& type, 
    double amount, 
    double price, 
    const std::string& label) {

    std::unordered_map<std::string, std::string> params; 
    params["instrument_name"] = instrument; 
    params["type"] = type; 
    params["amount"] = std::to_string(amount); 

    // Add price only for limit orders
    if (type == "limit") { 
        params["price"] = std::to_string(price);; 
    }

    // Add label if provided
    if (!label.empty()) { 
        params["label"] = label; 
    }

    return conn.sendRequest("/api/v2/private/buy", params, "GET", token); 
}

// Modify an existing order
// - Takes parameters for order_id, token, and optional fields for amount, contracts, price, etc.
// - Constructs the request URL and parameters with the provided modifications
// - Sends the request using the connection object
rapidjson::Document Trading::modifyOrder(
    const std::string& order_id, 
    const std::string& token, 
    const std::optional<double>& amount, 
    const std::optional<double>& contracts, 
    const std::optional<double>& price, 
    const std::optional<std::string>& advanced, 
    const std::optional<bool>& post_only, 
    const std::optional<bool>& reduce_only) {

    std::unordered_map<std::string, std::string> params; 
    params["order_id"] = order_id; 

    // Add parameters based on provided optional values
    if (amount) { 
        params["amount"] = std::to_string(amount.value()); 
    }
    if (contracts) { 
        params["contracts"] = std::to_string(contracts.value()); 
    }
    if (price) { 
        params["price"] = std::to_string(price.value()); 
    }
    if (advanced) { 
        params["advanced"] = advanced.value(); 
    }
    if (post_only) { 
        params["post_only"] = post_only.value() ? "true" : "false"; 
    }
    if (reduce_only) { 
        params["reduce_only"] = reduce_only.value() ? "true" : "false"; 
    }

    return conn.sendRequest("/api/v2/private/edit", params, "GET", token); 
}

// Place a sell order
// - Takes parameters for token, instrument, and optional fields for amount, contracts, price, type, trigger, etc.
// - Constructs the request URL and parameters
// - Sends the request using the connection object
rapidjson::Document Trading::sellOrder(
    const std::string& token, 
    const std::string& instrument, 
    const std::optional<double>& amount, 
    const std::optional<double>& contracts, 
    const std::optional<double>& price, 
    const std::optional<std::string>& type, 
    const std::optional<std::string>& trigger, 
    const std::optional<double>& trigger_price) {

    std::unordered_map<std::string, std::string> params; 
    params["instrument_name"] = instrument; 

    // Add parameters based on provided optional values
    if (amount) { 
        params["amount"] = std::to_string(amount.value()); 
    }
    if (contracts) { 
        params["contracts"] = std::to_string(contracts.value()); 
    }
    if (price) { 
        params["price"] = std::to_string(price.value()); 
    }
    if (type) { 
        params["type"] = type.value(); 
    }
    if (trigger) { 
        params["trigger"] = trigger.value(); 
    }
    if (trigger_price) { 
        params["trigger_price"] = std::to_string(trigger_price.value()); 
    }

    return conn.sendRequest("/api/v2/private/sell", params, "GET", token); 
}

// Cancel a specific order
// - Takes parameters for order ID and token
// - Constructs the request URL and parameters
// - Sends the request using the connection object
rapidjson::Document Trading::cancelOrder(const std::string& orderid, const std::string& token) {
    std::unordered_map<std::string, std::string> params; 
    params["orderid"] = orderid; 

    return conn.sendRequest("/api/v2/private/cancel", params, "GET", token); 
}

// Cancel all open orders
// - Takes the token as input
// - Constructs the request URL
// - Sends the request using the connection object
rapidjson::Document Trading::cancelAllOrder(const std::string& token) {
    std::unordered_map<std::string, std::string> params; 

    return conn.sendRequest("/api/v2/private/cancel_all", params, "GET", token); 
}

// Get all open orders
// - Takes the token as input
// - Constructs the request URL
// - Sends the request using the connection object
rapidjson::Document Trading::getOpenOrder(const std::string& token) {
    std::unordered_map<std::string, std::string> params; 

    return conn.sendRequest("/api/v2/private/get_open_orders", params, "GET", token); 
}
// Get the state of a specific order
// - Takes order ID and token as input
// - Constructs the request URL and parameters
// - Sends the request using the connection object
rapidjson::Document Trading::getOrderState(const std::string& orderid, const std::string& token) {
    std::unordered_map<std::string, std::string> params; 
    params["orderid"] = orderid; 

    return conn.sendRequest("/api/v2/private/get_order_state", params, "GET", token); 
}
// Get order book for a given instrument
// - Takes instrument name as input
// - Constructs the request URL and parameters
// - Sends the request using the connection object
rapidjson::Document Trading::getOrderBook(const std::string& instrument_name) {
    std::unordered_map<std::string, std::string> params; 
    params["instrument_name"] = instrument_name; 

    return conn.sendRequest("/api/v2/public/get_order_book", params, "GET"); 
}

// Get all open positions
// - Takes token as input
// - Constructs the request URL 
// - Sends the request using the connection object
rapidjson::Document Trading::getPositions(const std::string& token) {
    std::unordered_map<std::string, std::string> params; 

    return conn.sendRequest("/api/v2/private/get_positions", params, "GET", token); 
}

// Get position for a specific instrument
// - Takes token and instrument name as input
// - Constructs the request URL and parameters
// - Sends the request using the connection object
rapidjson::Document Trading::getPosition(const std::string& token, const std::string& instrument_name) {
    std::unordered_map<std::string, std::string> params; 
    params["instrument_name"] = instrument_name; 

    return conn.sendRequest("/api/v2/private/get_position", params, "GET", token); 
}