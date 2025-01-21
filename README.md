# Deribit Order Management System

This project is a C++ implementation of an order management system designed to interact with the Deribit cryptocurrency exchange. It facilitates functionalities such as placing, editing, and canceling orders, as well as managing real-time updates through a WebSocket server for low-latency trading needs.

## Features

- **Authentication**: Securely access Deribit APIs using OAuth2.
- **Order Placement**: Supports placing buy and sell orders.
- **Order Management**: Capabilities to edit and cancel orders.
- **WebSocket Server**: Allows clients to subscribe to specific instruments and receive real-time order book updates.

## Project Structure

The project is organized as follows:

```
Deribit_Order_Management_System/
│
├── include/
│   └── nlohmann/
│       └── json.hpp          # Single-header JSON library for C++
│
├── src/
│   ├── main.cpp              # Entry point of the application
│   ├── OrderManager.cpp      # Manages order actions (buy, sell, edit, cancel)
│   ├── OrderManager.h        # Header file for OrderManager
│   ├── WebSocketServer.cpp   # WebSocket server implementation for real-time updates
│   ├── WebSocketServer.h     # Header file for WebSocketServer
│   ├── DeribitAPI.cpp        # Implementation of Deribit API interactions
│   ├── DeribitAPI.h          # Header file for DeribitAPI
│   └── CurlUtils.h           # Utility functions for making HTTP requests
│
├── CMakeLists.txt            # Build configuration for CMake
└── README.md                 # Project documentation
```

## Prerequisites

- C++17 or later
- CMake
- nlohmann/json (included as a single header in the `include` folder)
- libcurl for HTTP requests
- OpenSSL (for secure connections)

## Installation Steps

1. **Clone the repository**:

   ```bash
   git clone https://github.com/MrSuperfluous/Deribit_Order_Management_System.git
   cd Deribit_Order_Management_System
   ```

2. **Install dependencies**:

   Ensure you have `libcurl` and `OpenSSL` installed on your system. For macOS, you can use Homebrew:

   ```bash
   brew install curl openssl
   ```

3. **Build the project**:

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

4. **Run the application**:

   ```bash
   ./DeribitOrderSystem
   ```

## Configuration

Create a `config.json` file in the root directory of the project and include your API keys and secrets in the following format:

```json
{
    "client_id": "your_client_id",
    "client_secret": "your_client_secret"
}
```

## Usage

The application will authenticate, place a buy and a sell order, edit an existing order, and start the WebSocket server to handle subscriptions for order book updates.

## API Methods Used

1. **Authentication**:

   - Method: `orderManager.authenticate()`
   - Purpose: Authenticates the user using the provided `client_id` and `client_secret`.

2. **Get Order Book**:

   - Method: `orderManager.getOrderbook("BTC-PERPETUAL")`
   - API Endpoint: Typically `public/get_order_book`
   - Purpose: Retrieves the current order book for the specified instrument (`BTC-PERPETUAL`).

3. **Get Current Positions**:

   - Method: `orderManager.getCurrentPositions()`
   - API Endpoint: Typically `private/get_positions`
   - Purpose: Fetches the current positions held by the user.

4. **Place Buy Order**:

   - Method: `orderManager.placeBuyOrder("BTC-PERPETUAL", 1000, 50000)`
   - API Endpoint: `private/buy`
   - Parameters:
     - Symbol: `"BTC-PERPETUAL"`
     - Amount: `1000`
     - Price: `50000`
   - Purpose: Places a buy order for the specified amount and price.

5. **Place Sell Order**:

   - Method: `orderManager.placeSellOrder("BTC-PERPETUAL", 10000, 66000)`
   - API Endpoint: `private/sell`
   - Parameters:
     - Symbol: `"BTC-PERPETUAL"`
     - Amount: `10000`
     - Price: `66000`
   - Purpose: Places a sell order for the specified amount and price.

6. **Edit Order**:

   - Method: `orderManager.editOrder(buyOrderId, 100000, 51000)`
   - API Endpoint: `private/edit`
   - Parameters:
     - Order ID: `buyOrderId`
     - New Amount: `100000` 
