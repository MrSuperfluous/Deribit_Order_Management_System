#include "utils.h"
#include <iostream>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h" // For PrettyWriter
using namespace std;

// Convert a rapidjson::Document to a formatted JSON string with indentation
std::string jsonToString(const rapidjson::Document& document) {
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    writer.SetIndent(' ', 4); // Set indent character and width (4 spaces in this case)
    document.Accept(writer);
    return buffer.GetString();
}

// Display the main menu for network connection selection
void showNetworkMenu(){
    cout << "Establish connection via:" << endl;
    cout << "1. Using WebSocket\n";
    cout << "2. Over HTTP\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

// Display the main trading menu
void showTradeMenu() {
    cout << "\n======= Order Management System =======\n";
    cout << "1. Place Order\n";
    cout << "2. Modify Order\n";
    cout << "3. Sell Order\n";
    cout << "4. Cancel Order\n";
    cout << "5. Cancel All Orders\n";
    cout << "6. Get Open Orders\n";
    cout << "7. Get Order State\n";
    cout << "8. Get OrderBook by Instrument\n";
    cout << "9. Get Position by Instrument\n";
    cout << "10. Get Positions\n";
    cout << "11. Get Latency Difference\n"; 
    cout << "12. Exit to Network Selection Menu\n";
    cout << "======================================\n";
    cout << "Enter your choice: ";
}

// Function to test order placement performance (synchronous vs. asynchronous)
void testOrderPlacement(int numCalls, std::string token, System& system) {
    std::vector<std::tuple<std::string, std::string, double, double, std::string>> orderParams;
    for (int i = 0; i < numCalls; ++i) {
        orderParams.emplace_back("ETH-PERPETUAL", "market", 5, 200000, "TestOrder" + std::to_string(i)); 
    }

    const std::string outputFolder = "./testout"; 
    if (!std::filesystem::exists(outputFolder)) {
        std::filesystem::create_directory(outputFolder);
    }

    std::ofstream outFile(outputFolder + "/order_placement_results.txt");
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file!" << std::endl;
        return;
    }

    outFile << std::fixed << std::setprecision(2); // Set precision for output formatting

    outFile << "Order Placement Test Results\n";
    outFile << "===========================\n";

    // --- Synchronous Test ---
    outFile << "\nSynchronous Test:\n";
    outFile << "-----------------\n";

    cout << "\nSynchronous Test:\n";
    cout << "-----------------\n";
    auto sync_start_time = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < orderParams.size(); ++i) {
        try {
            auto call_start_time = std::chrono::high_resolution_clock::now();
            const auto& [instrument, type, amount, price, label] = orderParams[i];
            rapidjson::Document response = system.placeOrder(token, instrument, type, amount, price, label); 
            auto call_end_time = std::chrono::high_resolution_clock::now();
            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(call_end_time - call_start_time).count();

            outFile << "Sync Call " << i + 1 << ": Latency: " << static_cast<double>(latency) / 1000.0 << " ms\n"; 
            if (response.HasMember("success") && response["success"].GetBool()) {
                outFile << "Success: true\n";
            } else if (response.HasMember("error")) {
                outFile << "Error: " << response["error"].GetString() << "\n";
            }
            outFile << "------------------------\n";

        } catch (const std::exception& e) {
            outFile << "Sync Call " << i + 1 << ": Exception: " << e.what() << "\n";
        }
    }
    auto sync_end_time = std::chrono::high_resolution_clock::now();
    auto sync_total_time = std::chrono::duration_cast<std::chrono::microseconds>(sync_end_time - sync_start_time).count();
    outFile << "Total Synchronous Time: " << static_cast<double>(sync_total_time) / 1000.0 << " ms\n";

    // --- Asynchronous Test ---
    outFile << "\nAsynchronous Test:\n";
    outFile << "------------------\n";
    cout << "\nAsynchronous Test:\n";
    cout << "------------------\n";
    auto async_start_time = std::chrono::high_resolution_clock::now();
    std::vector<rapidjson::Document> responses = system.placeOrdersAsync(token, orderParams);
    for (size_t i = 0; i < responses.size(); ++i) {
        outFile << "Async Call " << i + 1 << ":\n";
        if (responses[i].HasMember("error")) { 
            // Handle error cases in asynchronous results
        } else if (responses[i].HasMember("success") && responses[i]["success"].GetBool()) {
            outFile << "Success: true\n";
        }
        outFile << "------------------------\n";
    }
    auto async_end_time = std::chrono::high_resolution_clock::now();
    auto async_total_time = std::chrono::duration_cast<std::chrono::microseconds>(async_end_time - async_start_time).count();
    outFile << "Total Asynchronous Time: " << static_cast<double>(async_total_time) / 1000.0 << " ms\n";

    // --- Time Difference ---
    outFile << "\nTime Difference:\n";
    outFile << "----------------\n";

    double timeDifference = static_cast<double>(sync_total_time - async_total_time) / 1000.0;
    outFile << "Difference (Sync - Async): " << timeDifference << " ms\n";
    if(async_total_time != 0)
        outFile << "Asynchronous was " << (static_cast<double>(sync_total_time)/async_total_time) << " times faster\n";
    else
        outFile << "Asynchronous was infinite times faster\n"; 

    outFile.close();
}