#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/utsname.h>
#include <time.h>

#include "../OutputFormatter.h" // Required!

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

// All SOCK5 methods
#define METHOD_NO_AUTH_REQUIRED         0x00
#define METHOD_GSSAPI                   0x01
#define METHOD_USER_PASS                0x02
#define METHOD_NONE_ACCEPTABLE          0xFF

// All SOCK5 commands
#define CMD_CONNECT                     0x01
#define CMD_BIND                        0x02
#define CMD_UDP_ASSOICIATE              0x03


struct Auth_Negotiation_Request {

    enum ver : uint8_t {
        SOCKS4 =                0x04,
        SOCKS5 =                0x05
    };        
    uint8_t nomethods;
    enum AuthMethods : uint8_t {
        NOAUTH =                0x00,
        GSSAPI =                0x01,
        PASSWORD =              0x02,
        NO_ACCEPTABLE =         0xff
    };
    
} SOCK5_NEG_REQUEST;

struct Auth_Negotiation_Response {
    enum ver : uint8_t {
        SOCKS4 =                0x04,
        SOCKS5 =                0x05
    };  
    enum AuthMethods : uint8_t {
        NOAUTH =                0x00,
        GSSAPI =                0x01,
        PASSWORD =              0x02,
        NO_ACCEPTABLE =         0xff
    };
};



#pragma pack(push, 1)

class Sock5Proxy {
    private:
        int serverSocket, clientSocket;
        int PORT;
    public:

    Sock5Proxy(int PORT = 1080) {
        this->PORT = PORT;
    }

    bool startProxy() {

        // Usually on port 1080

        // Create socket that with ADDR_REUSE
        
    }

    void handleClients() {

        // First receive all methods from client.

        // Send back the auth method that are acceptable.


    }
    

    /**
     * @brief Handles the incoming proxy request.
     * RFC: https://datatracker.ietf.org/doc/html/rfc1928#section-4
     *  
     */
    void handleRequest() {
        
    }

    /**
     * @brief Handles the outgoing reply to client.
     * RFC: https://datatracker.ietf.org/doc/html/rfc1928#section-6
     */
    void sendReply() {

    }
};
#endif // !~ SOCK5_PROXY_H

#ifdef DEBUG_BOF
    int main(int argc, char * argv[]) {

        Sock5Proxy proxy(9050);
        if (!proxy.startProxy()) {

            printf("Failed to start proxy!\n");
            return 1;
        }

        proxy.handleClients();
        return 0;
    }

#else
int payload_init(int id, output_func output) {

    output(id, "Starting proxy sock5 server...\n");

    return 0;
}
#endif // DEBUG_BOF
