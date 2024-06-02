#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/tcp.h>

#include "../OutputFormatter.h" // Required!

#ifdef DEBUG_BOF
#define INPUT_PORT 9080
#endif

#define INPUT_PORT HOST_PORT

typedef void(*output_func)(int, const char* fmt, ...); // For printing output on C2 server.
/**
 * @brief Features to add
 * 
 * - proxy all data through socket
 * - custom comand TAP to retrieve network map (ping sweep)?
 */

/**
 * Socks protocol Version 5 BOF
 * RFC: https://datatracker.ietf.org/doc/html/rfc1928
 * C2: https://github.com/Trisna22/Odysseus
 */
#ifndef SOCK5_PROXY_H
#define SOCK5_PROXY_H

#define NULL                            0x00
#define SOCKET_ERROR                    -1
#define ARRAY_INIT                      {0}
#define ARRAY_SIZE (x)                  (sizeof(x) / sizeof(x[0]))
#define MAX_BUFFER_SIZE                 65536

typedef uint8_t uint128_t[16];  // For IPv6 addresses.

// SOCKS versions
enum ver : uint8_t {
    SOCKS4 =                0x04,
    SOCKS5 =                0x05
};        

// SOCK5 authentication methods
enum AuthMethods : uint8_t {
    NOAUTH =                0x00,
    GSSAPI =                0x01,
    USER_PASSWORD =         0x02,
    NO_ACCEPTABLE =         0xFF
};

// SOCK5 commands
enum Command : uint8_t {
    CONNECT =               0x01,
    BIND =                  0x02,
    UDP_ASSOCIATE =         0x03
};

// SOCK5 address type.
enum AddressType : uint8_t {
    IPV4 =                  0x01,
    FQDN =                  0x03,
    IPV6 =                  0x04,
};

// SOCK5 command reply
enum CommandReply : uint8_t {
    OK =                    0x00,
    ERROR =                 0x01,
    RULE_BLOCK =            0x02,
    NETWORK_UNREACHABLE =   0x03,
    HOST_UNREACHABLE =      0x04,
    CONNECTION_REFUSED =    0x05,
    TTL_EXPIRED =           0x06,
    CMD_NOT_SUPPORTED =     0x07,
    ADDR_NOT_SUPPORTED =    0x08,
    UNASSIGNED =            0xFF
};

/**
 * All structs for invitation handshake (authentication methods)
 */
struct InvitationRequest {

    uint8_t version;       // Proxy version to use.
    uint8_t nmethods;      // Number of methods.
    
} SOCK5_INV_REQUEST;
struct InvitationResponse {
    uint8_t version;       // Proxy version to use.
    uint8_t method;        // Method to use.
} SOCK5_INV_RESPONSE;

/**
 * All structs for proxy commands.
 */

// IP version 4.
struct IPv4 {
    uint32_t ip;
    uint16_t port;
} ipv4;

// IP version 6.
struct IPv6 {
    uint128_t ip;
    uint16_t port;
} ipv6;

// Domain name.
struct FQDN {
    uint8_t bytes[4];
} fqdn;

struct CommandRequest {
    uint8_t version;       // Proxy version to use.
    uint8_t command;       // The proxy command.
    uint8_t reserved;
    uint8_t addrType;      // The internet address type.
} SOCK5_COMMAND;

struct CommandResponse {
    uint8_t version;
    uint8_t reply;
    uint8_t reserved;
    uint8_t addrType;
    // union BND {

    //     struct IPv4 ipv4;
    //     struct IPv6 ipv6;
    //     struct FQDN fqdn;
        
    // } BindAddress;

} SOCK5_COMMAND_REPLY;

#pragma pack(push, 1)

class Sock5Proxy {
private:
    int clientSocket;
    char* proxyUsername;
    char* proxyPassword;
    AuthMethods authMethod;

    /**
     * Exit thread gracefully.
     */
    void exitThread() {
        
        close(this->clientSocket);
        pthread_exit(0);
    }

    /**
     * Receive any object from client socket.
     */
    template<typename T>
    T receiveObject(int* red) {

        T obj;
        if ((*red = recv(this->clientSocket, &obj, sizeof(T), NULL)) <= 0) {
            printf("[!] Failed to receive proxy object! Error code: %d\n", errno);
            this->exitThread();
        }

        return obj;
    }

    /**
     * Send any object to client socket.
     */
    template<typename T>
    void sendObject(T obj, int size) {

        if (send(this->clientSocket, &obj, size, NULL) == SOCKET_ERROR) {
            printf("[!] Failed to send proxy object! Error code: %d\n", errno);
        }
    }

    /**
     * Send IP & PORT response.
     */
    void sendIPResponse(char* ip, unsigned short int port) {

        send(this->clientSocket, (void*)ip, 4, 0);
        send(this->clientSocket, (void*)&port, sizeof(port), 0);
        
    }

    /**
     * Handle SOCK5 authentication.
     */
    void handleSOCK5Authentication() {
        switch (this->authMethod)
        {
            case AuthMethods::NOAUTH: {
                
                // Send response its fine.
                InvitationResponse response = {
                    ver::SOCKS5,
                    AuthMethods::NOAUTH
                };

                sendObject<InvitationResponse>(response, sizeof(InvitationResponse));
                break;
            }
            
            case AuthMethods::USER_PASSWORD: {

                printf("[!] USERNAME PASSWORD authentication not yet implemented!\n");
                this->exitThread();
                break;
            }
        }
    }

    /**
     * Convert uint32 to IP address.
     */
    static char* parseIPv4(uint32_t value) {
        
        // uint32_t networkByteOrder = htonl(value);
        char *IP = new char[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET, &value, IP, INET_ADDRSTRLEN) == NULL) {
            printf("[-] Failed to parse the IPv4 address! Error code: %d\n", errno);
            return NULL;
        }

        return IP;
    }

    /**
     * Proxy to an app with IP and port.
     */
    int proxyConnect(int addrType, char* ip, int port) {

        int red, appSock = SOCKET_ERROR;

        // Construct struct for connecting.
        struct sockaddr_in remote;
        memset(&remote, 0, sizeof(sockaddr_in));
        remote.sin_addr.s_addr = inet_addr(ip);
        remote.sin_port = htons(port);
        remote.sin_family = AF_INET;

        /**
         *  Connect to remote address.
         */
        printf("[#] Connecting to app %s:%d\n", ip, port);

        appSock = socket(AF_INET, SOCK_STREAM, 0);
        if (appSock == SOCKET_ERROR) {

            printf("[-] Failed to create app socket! Error code: %d\n", errno);
            return SOCKET_ERROR;
        }

        if (connect(appSock, (sockaddr*)&remote, sizeof(remote)) == SOCKET_ERROR) {

            printf("[-] Failed to connect to the app! Error code: %d\n", errno);
            return SOCKET_ERROR;
        }

        return appSock;
    }

    /**
     * Proxy to bind to an ip and port for an app. 
     */
    int proxyBind(char* ip, int port) {

        return SOCKET_ERROR;
    }


    /**
     * Proxy pipe the connection between app and the client.
     */
    void pipeSocketApp(int clientSock, int appSocket) {

        // The biggest file descriptor is what we are looking for.
        int maxFD = (clientSock > appSocket) ? clientSock : appSocket;

        // Setup read/write variables.
        fd_set readSet;
        char buffer[MAX_BUFFER_SIZE];

        for (;;) {
            FD_ZERO(&readSet);
            FD_SET(clientSock, &readSet);
            FD_SET(appSocket, &readSet);
            int ret = select(maxFD + 1, &readSet, NULL, NULL, NULL);

            if (ret < 0 && errno == EINTR) {
                continue;
            }

            // When app socket sends data.
            if (FD_ISSET(appSocket, &readSet)) {
                
                int nread = recv(appSocket, buffer, MAX_BUFFER_SIZE, 0);
                if (nread <= 0) {
                    if (nread == 0) {
                        printf("[-] App closed connection!\n");
                    } else {
                        printf("[-] Error reading from appSocket! Error code: %d\n", errno);
                    }
                    break;
                }

                // printf("[*] %d bytes appSocket => clientSocket\n", nread);

                // Send to cient socket.
                if (send(clientSock, (const void*)buffer, nread, NULL) == SOCKET_ERROR) {

                    printf("[-] Error sending data client socket! Error code: %d\n", errno);
                    break;
                }

                continue;
            }
            
            // When client socket sends data.
            if (FD_ISSET(clientSock, &readSet)) {
                
                int nread = recv(clientSock, buffer, MAX_BUFFER_SIZE, NULL);
                if (nread <= 0) {
                    if (nread == 0) {
                        printf("[-] Client closed connection!\n");
                    } else {
                        printf("[-] Error reading from client! Error code: %d\n", errno);
                    }
                    break;
                }

                // printf("[*] %d bytes clientSocket => appSocket\n", nread);

                // Send to app socket.
                if (send(appSocket, (const void*)buffer, nread, NULL) == SOCKET_ERROR) {

                    printf("[-] Error sending data to app socket! Error code: %d\n", errno);
                    break;
                }
            }
        }

        this->exitThread();
    }

public:

    Sock5Proxy(int sock, AuthMethods method) : clientSocket(sock), authMethod(method) {}
    Sock5Proxy(int sock, AuthMethods method, char* username, char* password) : authMethod(method),
        clientSocket(sock),proxyUsername(username),proxyPassword(password) {}

    ~Sock5Proxy() {
        close(this->clientSocket);
    }

    /**
     * @brief Handles the initiation requests.
     * RFC: https://datatracker.ietf.org/doc/html/rfc1928#section-3
     */
    void invitation() {

        // Client sends us the version, number of methods and all methods.
        int sizeRead = 0;
        InvitationRequest inv = this->receiveObject<InvitationRequest>(&sizeRead);
        
        // Check if version compatible.
        if (sizeRead == 2 && inv.version != ver::SOCKS5 && inv.version != ver::SOCKS4) {

            printf("[#] They sent us {%hhX, %hhX}\n", inv.version, inv.nmethods);
            printf("[-] Incompatible with our version!\n");
            exitThread();
        }

        if (inv.version == ver::SOCKS5) {

            bool supported = false;

            // Check if auth method is supported by us.
            for (int i = 0; i < inv.nmethods; i++) {

                int red = 0;
                char type = this->receiveObject<char>(&red);

                // Check if client supports our chosen authentication method.
                if (type == this->authMethod) {
                    supported = true;
                }
            }

            // If none of the methods are supported.
            if (!supported) {

                InvitationResponse response = {
                    ver::SOCKS5,
                    AuthMethods::NO_ACCEPTABLE
                };
                sendObject<InvitationResponse>(response, sizeof(InvitationResponse));
            }
            else {
                this->handleSOCK5Authentication();
                return; // Exit function instead of suicide.
            }

        }
        else if (inv.version == ver::SOCKS4) {

            printf("[-] Version not yet supported...\n");

        } else {
            printf("[-] Unsupported version!\n");
        }

        this->exitThread();
    }  
    
    /**
     * @brief Handles the incoming proxy request.
     * RFC: https://datatracker.ietf.org/doc/html/rfc1928#section-4
     */
    void handleCommands() {
        
        // Get the command from the client.
        char* IP;
        int red, port;
        CommandRequest request = this->receiveObject<CommandRequest>(&red);

        int appSocket = SOCKET_ERROR;

        // Based on command connect or bind.
        switch (request.command)
        {
        case Command::CONNECT: {

            // Check if an domain is given instread of IP.
            if (request.addrType == AddressType::FQDN) {
                // Parse the domain.


            } else {
                // Parse the addresses.
                IPv4 ipv4 = this->receiveObject<IPv4>(&red);
                IP = Sock5Proxy::parseIPv4(ipv4.ip);
                port = htons(ipv4.port);
            }

            appSocket = this->proxyConnect(request.addrType, IP ,port);

            break;
        }
        case Command::BIND: {

            printf("[-] Proxy method BIND not supported yet!\n");
            
            CommandResponse response = {
                ver::SOCKS5,
                CommandReply::CMD_NOT_SUPPORTED,
                0x00, // Reserved
                request.addrType
            };
            
            this->sendObject<CommandResponse>(response, sizeof(response));
            this->sendIPResponse("0.0.0.0", 666);
            this->exitThread();
            
            break;
        }
        case Command::UDP_ASSOCIATE: {
            printf("[-] Proxy method UDP ASSOCIATE not supported yet!\n");

            CommandResponse response = {
                ver::SOCKS5,
                CommandReply::CMD_NOT_SUPPORTED,
                0x00, // Reserved
                request.addrType
            };
            
            this->sendObject<CommandResponse>(response, sizeof(response));
            this->sendIPResponse("0.0.0.0", 666);
            this->exitThread();
            break;
        }
        default: {
            printf("[-] Unknown proxy command: %hhx\n", request.command);

            CommandResponse response = {
                ver::SOCKS5,
                CommandReply::CMD_NOT_SUPPORTED,
                0x00, // Reserved
                request.addrType,
            };
            
            this->sendObject<CommandResponse>(response, sizeof(response));
            this->sendIPResponse("0.0.0.0", 666);
            this->exitThread();
            break;
        }
        }

        // Check if connection has been established.
        if (appSocket != SOCKET_ERROR) {

            // Send response OK.
            CommandResponse response = {
                ver::SOCKS5,
                CommandReply::OK,
                0x00, // Reserved
                request.addrType
            };
            
            this->sendObject<CommandResponse>(response, sizeof(response));
            this->sendIPResponse("0.0.0.0", 666);

            // Pipe connection.
            pipeSocketApp(this->clientSocket, appSocket);
        }
        else {

            // Send response error.
            CommandResponse response = {
                ver::SOCKS5,
                CommandReply::ERROR,
                0x00, // Reserved
                request.addrType
            };

            // Based on socket error.
            switch (errno) {
                case ENETDOWN:
                case ENETUNREACH: {
                    response.reply = CommandReply::NETWORK_UNREACHABLE;
                    break;
                }
                
                case ECONNREFUSED: {
                    response.reply = CommandReply::CONNECTION_REFUSED;
                    break;
                }

                case ECONNRESET: {
                    response.reply = CommandReply::ERROR;
                    break;
                }

                default: {
                    response.reply = CommandReply::ERROR;
                    break;
                }
            }

            this->sendObject<CommandResponse>(response, sizeof(response));
            this->sendIPResponse("0.0.0.0", 666);
        }

        this->exitThread(); // If finished close thread.
    }

    /**
     * Start proxy server.
     */
    static int startProxy(int PORT = 1080) {

        // Usually on port 1080
        int serverSocket;

        sockaddr_in address;
        memset(&address, 0, sizeof(sockaddr_in));
        address.sin_port = htons(PORT);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;

        // Create socket that with ADDR_REUSE
        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {

            printf("[-] Failed to create socket! Error code: %d\n", errno);
            return SOCKET_ERROR;
        }

        // Set port and host as reuseable.
        int option = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))) {

            printf("[-] Failed to set socket options to reuse addr and port! Error code: %d\n", errno);
            return SOCKET_ERROR;
        }

        // Bind sock address and port.
        if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {

            printf("[-] Failed to bind the socket! Error code: %d\n", errno);
            return SOCKET_ERROR;
        }

        // Now listen for clients.
        if (listen(serverSocket, 25) < 0) {

            printf("[-] Failed to listen for connections! Error code: %d\n", errno);
            return SOCKET_ERROR;
        }

        return serverSocket;
    }

    /**
     * App loop 
     */
    static int handleClients(int serverSocket) {

        int clientSocket;
        pthread_t worker;
        struct sockaddr_in remote;
        memset(&remote, 0, sizeof(remote));
        socklen_t remoteLen;
    
        for (;;) {

            // Accept client.
            if ((clientSocket = accept(serverSocket, (sockaddr*)&remote, &remoteLen)) < 0) {
                printf("[-] Failed to accept proxy client! Error code: %d\n", errno);
                return errno;    
            }

            printf("[#] Accepted new client connection \n");

            // Create worker thread for the proxy client.
            if (pthread_create(&worker, 
                NULL, Sock5Proxy::proxyClientHandler, 
                (void*)&clientSocket) != 0) {
                
                printf("[-] Failed to create new thread for client! Error code: %d\n", errno);    
                return errno;
            } else {

                // Unhook thread entirely
                pthread_detach(worker);
            }
        }

        return 0;
    }

    /**
     * Client thread handler. 
     * 
     * @param arg -> int clientSocket;
     */
    static void *proxyClientHandler(void* arg) {

        int clientSocket = *(int *)arg;

        // Initialize SOCK5 object.
        Sock5Proxy proxy(clientSocket, AuthMethods::NOAUTH);

        // First handle invitation proces.
        proxy.invitation();

        // Now handle commanding.
        proxy.handleCommands();
        return 0;
    }
};
#endif // !~ SOCK5_PROXY_H

#ifdef DEBUG_BOF
    int main(int argc, char * argv[]) {

        int serverSocket;
        if ((serverSocket = Sock5Proxy::startProxy(INPUT_PORT)) == SOCKET_ERROR) {
            return 1;
        }
        
        printf("[!] Listening for connections on port %d...\n", INPUT_PORT);

        return Sock5Proxy::handleClients(serverSocket);
    }

#else
int payload_init(int id, output_func output) {

    // Only output when ended or killed. 
    // output(id, )

    int serverSocket;
    if ((serverSocket = Sock5Proxy::startProxy(INPUT_PORT)) == SOCKET_ERROR) {
        return 1;
    }
    
    printf("[!] Listening for connections on port %d...\n", INPUT_PORT);

    return Sock5Proxy::handleClients(serverSocket);
}
#endif // DEBUG_BOF
