/**
 * @file socket.c
 * @brief Implementation of simple wrapper for C socket API
 */

#include "socket.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

/**
 * Create a success result
 */
static socket_result_t socket_success(void) {
    return (socket_result_t){
        .success = true,
        .error_msg = NULL,
        .error_code = 0
    };
}

/**
 * Create an error result
 */
static socket_result_t socket_error(const char *error_msg) {
    return (socket_result_t){
        .success = false,
        .error_msg = error_msg,
        .error_code = errno
    };
}

socket_result_t socket_create(socket_t *sock, socket_type_t type) {
    if (!sock) {
        return socket_error("Invalid socket pointer");
    }

    int sock_type = (type == SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;
    int protocol = (type == SOCKET_TCP) ? IPPROTO_TCP : IPPROTO_UDP;
    
    int fd = socket(AF_INET, sock_type, protocol);
    if (fd < 0) {
        return socket_error("Failed to create socket");
    }

    // Initialize socket structure
    memset(sock, 0, sizeof(socket_t));
    sock->fd = fd;
    sock->type = type;
    sock->is_server = false;
    sock->is_connected = false;
    sock->is_nonblocking = false;

    // Set socket options
    int option = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0) {
        close(fd);
        return socket_error("Failed to set socket options");
    }

    return socket_success();
}

socket_result_t socket_set_nonblocking(socket_t *sock, bool nonblocking) {
    if (!sock || sock->fd < 0) {
        return socket_error("Invalid socket");
    }

    int flags = fcntl(sock->fd, F_GETFL, 0);
    if (flags < 0) {
        return socket_error("Failed to get socket flags");
    }

    if (nonblocking) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(sock->fd, F_SETFL, flags) < 0) {
        return socket_error("Failed to set socket flags");
    }

    sock->is_nonblocking = nonblocking;
    return socket_success();
}

socket_result_t socket_bind(socket_t *sock, int port) {
    if (!sock || sock->fd < 0) {
        return socket_error("Invalid socket");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(sock->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        return socket_error("Failed to bind socket");
    }

    sock->addr = addr;
    sock->is_server = true;
    return socket_success();
}

socket_result_t socket_listen(socket_t *sock, int backlog) {
    if (!sock || sock->fd < 0) {
        return socket_error("Invalid socket");
    }

    if (!sock->is_server) {
        return socket_error("Socket is not bound to an address");
    }

    if (listen(sock->fd, backlog) < 0) {
        return socket_error("Failed to listen on socket");
    }

    return socket_success();
}

socket_result_t socket_accept(socket_t *server_sock, socket_t *client_sock) {
    if (!server_sock || server_sock->fd < 0 || !client_sock) {
        return socket_error("Invalid socket");
    }

    if (!server_sock->is_server) {
        return socket_error("Not a server socket");
    }

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    
    int client_fd = accept(server_sock->fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return socket_error("No pending connections");
        }
        return socket_error("Failed to accept connection");
    }

    // Initialize client socket
    memset(client_sock, 0, sizeof(socket_t));
    client_sock->fd = client_fd;
    client_sock->type = server_sock->type;
    client_sock->addr = client_addr;
    client_sock->is_server = false;
    client_sock->is_connected = true;
    client_sock->is_nonblocking = false;

    return socket_success();
}

socket_result_t socket_connect(socket_t *sock, const char *hostname, int port) {
    if (!sock || sock->fd < 0 || !hostname) {
        return socket_error("Invalid socket or hostname");
    }

    if (sock->is_server) {
        return socket_error("Cannot connect with a server socket");
    }

    struct hostent *host = gethostbyname(hostname);
    if (!host) {
        return socket_error("Failed to resolve hostname");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr *)host->h_addr);

    if (connect(sock->fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if (sock->is_nonblocking && (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK)) {
            // This is expected for non-blocking sockets and not an error
            sock->addr = addr;
            return socket_success();
        }
        return socket_error("Failed to connect to server");
    }

    sock->addr = addr;
    sock->is_connected = true;
    return socket_success();
}

socket_result_t socket_send(socket_t *sock, const void *data, size_t size, size_t *bytes_sent) {
    if (!sock || sock->fd < 0 || !data) {
        return socket_error("Invalid socket or data");
    }

    if (!sock->is_connected && sock->type == SOCKET_TCP) {
        return socket_error("Socket is not connected");
    }

    ssize_t sent = send(sock->fd, data, size, 0);
    if (sent < 0) {
        if (sock->is_nonblocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            if (bytes_sent) {
                *bytes_sent = 0;
            }
            return socket_success();
        }
        return socket_error("Failed to send data");
    }

    if (bytes_sent) {
        *bytes_sent = (size_t)sent;
    }
    return socket_success();
}

socket_result_t socket_receive(socket_t *sock, void *buffer, size_t size, size_t *bytes_received) {
    if (!sock || sock->fd < 0 || !buffer) {
        return socket_error("Invalid socket or buffer");
    }

    if (!sock->is_connected && sock->type == SOCKET_TCP) {
        return socket_error("Socket is not connected");
    }

    ssize_t received = recv(sock->fd, buffer, size, 0);
    if (received < 0) {
        if (sock->is_nonblocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            if (bytes_received) {
                *bytes_received = 0;
            }
            return socket_success();
        }
        return socket_error("Failed to receive data");
    }

    if (bytes_received) {
        *bytes_received = (size_t)received;
    }
    return socket_success();
}

socket_result_t socket_close(socket_t *sock) {
    if (!sock || sock->fd < 0) {
        return socket_error("Invalid socket");
    }

    if (close(sock->fd) < 0) {
        return socket_error("Failed to close socket");
    }

    sock->fd = -1;
    sock->is_connected = false;
    sock->is_server = false;
    return socket_success();
}

const char *socket_get_error_string(socket_result_t result) {
    if (result.success) {
        return "Success";
    }
    
    if (result.error_msg) {
        return result.error_msg;
    }
    
    return strerror(result.error_code);
}
