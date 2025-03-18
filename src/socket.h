/**
 * @file socket.h
 * @brief Simple wrapper for C socket API
 */

#ifndef ZENO_SOCKET_H
#define ZENO_SOCKET_H

#include <netinet/in.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>

/**
 * Socket type enumeration
 */
typedef enum {
    SOCKET_TCP, /**< TCP socket */
    SOCKET_UDP  /**< UDP socket */
} socket_type_t;

/**
 * Socket handle structure
 */
typedef struct {
    int fd;                 /**< Socket file descriptor */
    socket_type_t type;     /**< Socket type (TCP/UDP) */
    struct sockaddr_in addr; /**< Socket address */
    bool is_server;         /**< Is this a server socket? */
    bool is_connected;      /**< Is the socket connected? */
    bool is_nonblocking;    /**< Is the socket non-blocking? */
} socket_t;

/**
 * Socket result structure
 */
typedef struct {
    bool success;           /**< Operation success flag */
    const char *error_msg;  /**< Error message if success is false */
    int error_code;         /**< Error code if success is false */
} socket_result_t;

/**
 * Create a new socket
 * 
 * @param socket Pointer to socket_t structure to initialize
 * @param type Socket type (SOCKET_TCP or SOCKET_UDP)
 * @return Result of the operation
 */
socket_result_t socket_create(socket_t *socket, socket_type_t type);

/**
 * Set a socket to non-blocking mode
 * 
 * @param socket Socket to set as non-blocking
 * @param nonblocking true to set as non-blocking, false for blocking
 * @return Result of the operation
 */
socket_result_t socket_set_nonblocking(socket_t *socket, bool nonblocking);

/**
 * Bind a socket to a specific port on all interfaces
 * 
 * @param socket Socket to bind
 * @param port Port number to bind to
 * @return Result of the operation
 */
socket_result_t socket_bind(socket_t *socket, int port);

/**
 * Start listening for connections on a socket
 * 
 * @param socket Socket to listen on (must be bound first)
 * @param backlog Maximum queue length for pending connections
 * @return Result of the operation
 */
socket_result_t socket_listen(socket_t *socket, int backlog);

/**
 * Accept a new connection on a listening socket
 * 
 * @param server_socket Socket that is listening
 * @param client_socket Pointer to socket_t structure to initialize with the new connection
 * @return Result of the operation
 */
socket_result_t socket_accept(socket_t *server_socket, socket_t *client_socket);

/**
 * Connect to a server
 * 
 * @param socket Socket to use for the connection
 * @param hostname Hostname or IP address to connect to
 * @param port Port number to connect to
 * @return Result of the operation
 */
socket_result_t socket_connect(socket_t *socket, const char *hostname, int port);

/**
 * Send data over a socket
 * 
 * @param socket Socket to send data on
 * @param data Pointer to data to send
 * @param size Size of data to send in bytes
 * @param bytes_sent Pointer to store number of bytes actually sent (can be NULL)
 * @return Result of the operation
 */
socket_result_t socket_send(socket_t *socket, const void *data, size_t size, size_t *bytes_sent);

/**
 * Receive data from a socket
 * 
 * @param socket Socket to receive data from
 * @param buffer Pointer to buffer to store received data
 * @param size Size of buffer in bytes
 * @param bytes_received Pointer to store number of bytes actually received (can be NULL)
 * @return Result of the operation
 */
socket_result_t socket_receive(socket_t *socket, void *buffer, size_t size, size_t *bytes_received);

/**
 * Close a socket
 * 
 * @param socket Socket to close
 * @return Result of the operation
 */
socket_result_t socket_close(socket_t *socket);

/**
 * Get string representation of a socket error
 * 
 * @param result Socket result containing the error
 * @return String representation of the error
 */
const char *socket_get_error_string(socket_result_t result);

#endif /* ZENO_SOCKET_H */
