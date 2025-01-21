#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>
#include <functional>

class WebSocketClient {
public:
    // Type definitions for WebSocket client
    using Client = websocketpp::client<websocketpp::config::asio_tls_client>;
    using MessagePtr = websocketpp::config::asio_client::message_type::ptr;
    using MessageHandler = std::function<void(const std::string&)>;

    WebSocketClient();
    ~WebSocketClient();

    // Connection management
    bool connect(const std::string& host, const std::string& port);
    bool subscribe(const std::string& channel, const std::string& token);
    void listen();
    void close();
    void startWebSocketSession(const std::string& token);

    // Callback setters
    void setMessageHandler(MessageHandler handler) { messageHandler = handler; }
    
    // Status checks
    bool isConnected() const { return connected; }
    bool isRunning() const { return m_isRunning; }

private:
    // WebSocket callbacks
    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, MessagePtr msg);
    void on_error(websocketpp::connection_hdl hdl);

    // Helper methods
    bool reconnect();
    void processMessage(const std::string& message);
    std::string constructSubscriptionMessage(const std::string& channel, const std::string& token);

    // WebSocket client and connection
    Client client;
    websocketpp::connection_hdl connection;
    
    // Thread management
    std::thread m_listenerThread;
    std::mutex mutex_;
    
    // Status flags
    std::atomic<bool> connected;
    std::atomic<bool> should_run;
    std::atomic<bool> m_isRunning;

    // Connection details
    std::string m_host;
    std::string m_port;
    
    // Message handling
    MessageHandler messageHandler;
    std::queue<std::string> messageQueue;
    std::mutex queueMutex;
    
    // Constants
    static constexpr int RECONNECT_DELAY_MS = 5000;
    static constexpr int MAX_RECONNECT_ATTEMPTS = 5;
};