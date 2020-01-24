#include "common.h"
#include "common-cpp.h"

static std::mutex io_mutex;
#define LOG(x)                                                                                      \
    do {                                                                                            \
        io_mutex.lock();                                                                            \
        std::cout << std::this_thread::get_id() << " - " << __FUNCTION__ << ": " << x << std::endl; \
        io_mutex.unlock();                                                                          \
    } while (false)
#define SERVER_MAX_THREADS 2

struct ClientConnection {
    ClientConnection(sockpp::tcp_socket&& sock)
        : socket(std::move(sock)) {
        ASSERT(socket.is_open());
        LOG("connection opened: " << socket.peer_address() << ", native " << socket.handle());
    }
    ClientConnection(const ClientConnection&) = delete;
    ClientConnection(ClientConnection&&)      = default;

    sockpp::tcp_socket socket;
};

void connection_handler_thread(std::deque<ClientConnection>* connections, std::mutex* connections_mutex) {
    ASSERT(connections);
    ASSERT(connections_mutex);
    LOG("thread started");
    ClientConnection* conn    = nullptr;
    bool              running = true;
    while (running) {
        if (!conn) {
            // acquire mutex so the list doesn't get emptied in
            // between checking this and popping an element
            connections_mutex->lock();
            if (!connections->empty()) {
                conn = new ClientConnection(std::move(connections->front()));
                connections->pop_front();
                // we can release the mutex at this point as our
                // manipulation of connections is done.
                connections_mutex->unlock();
            } else {
                connections_mutex->unlock();
                LOG("waiting for connections...");
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } else {
            if (!conn->socket || !conn->socket.is_open()) {
                LOG("socket " << conn->socket.handle() << " closed");
                delete conn;
                conn = nullptr;
                continue;
            }

            LOG("handling connection " << conn->socket.peer_address() << " (native " << conn->socket.handle() << ")");

            ASSERT(conn);
            ASSERT(!!conn->socket);
            conn->socket.read_timeout(std::chrono::milliseconds(100));
            message_t msg;
            ssize_t   rc = conn->socket.read(static_cast<void*>(&msg), sizeof(message_t));

            if (rc == -1) {
                // some error occurred
                //LOG("socket.read: " << conn->socket.last_error_str());
                LOG("no new messages for connection " << conn->socket.peer_address());
                // we add the connection back to the list and continue
                connections_mutex->lock();
                connections->push_back(std::move(*conn));
                connections_mutex->unlock();
                delete conn;
                conn = nullptr;
                continue;
            }

            // c-style write to avoid SIGPIPE on broken connection
            {
                int native_socket_handle = conn->socket.handle();
                rc                       = send(native_socket_handle, "???", 4, MSG_NOSIGNAL);
                if (rc == -1) {
                    LOG("pipe likely broke on " << conn->socket.handle() << ", closing connection");
                    conn->socket.shutdown(SHUT_RDWR);
                    conn->socket.close();
                }
            }

            LOG("received from " << conn->socket.peer_address() << ", user_id=" << msg.user_id << ": " << msg.text);
        }
    }
}

int main(int, char**) {
    std::int16_t                 port = PORT;
    std::deque<ClientConnection> connections;
    std::mutex                   connections_mutex;
    std::vector<std::thread>     thread_pool;

    sockpp::tcp_acceptor acc(port);

    ASSERT(static_cast<bool>(acc));

    // create a few threads
    for (int i = 0; i < SERVER_MAX_THREADS; ++i) {
        thread_pool.push_back(std::thread(connection_handler_thread, &connections, &connections_mutex));
    }

    // listen & accept loop
    LOG("listening");
    bool is_running = true;
    while (is_running) {
        sockpp::tcp_socket sock = acc.accept();
        ClientConnection   conn(std::move(sock));
        connections_mutex.lock();
        connections.push_back(std::move(conn));
        connections_mutex.unlock();
    }
}
