#include "common.h"
#include "common-cpp.h"

static std::mutex io_mutex;
#define LOG(x) std::cout << std::this_thread::get_id() << " - " << __FUNCTION__ << ": " << x << std::endl
#define SERVER_MAX_THREADS 2

struct ClientConnection {
    ClientConnection(sockpp::tcp_socket&& sock)
        : socket(std::move(sock)) {
        ASSERT(socket.is_open());
        LOG("connection opened: " << socket.peer_address());
    }
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection(ClientConnection&&)      = default;

    sockpp::tcp_socket socket;
};

void connection_handler_thread(std::deque<ClientConnection>* connections) {
    LOG("thread started");
    bool running = true;
    while (running) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int, char**) {
    std::int16_t                 port = PORT;
    std::deque<ClientConnection> connections;
    std::vector<std::thread>     thread_pool;

    sockpp::tcp_acceptor acc(port);

    ASSERT(static_cast<bool>(acc));

    // create a few threads
    for (int i = 0; i < SERVER_MAX_THREADS; ++i) {
        thread_pool.push_back(std::thread(connection_handler_thread, &connections));
    }

    // listen & accept loop
    LOG("listening");
    bool is_running = true;
    while (is_running) {
        sockpp::tcp_socket sock = acc.accept();
        ClientConnection   conn(std::move(sock));
        connections.push_back(std::move(conn));
    }
}
