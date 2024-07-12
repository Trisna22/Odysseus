
#define RESPONSE_EXIT               0x00  // Exit malware
#define RESPONSE_INIT               0x01  // Init job
#define RESPONSE_NEW_OBJECT         0x02  // New job
#define RESPONSE_NOT_INIT           0x03  // Implant hasn't been init yet.
#define RESPONSE_KILL_JOB           0x04  // Kill a running job.
#define RESPONSE_NO_ENTRY           0x222 // Object has no entry point
#define RESPONSE_NO_RES             0x333 // Server doesn't response to implant.
#define RESPONSE_DESTROY            0x666 // Destroy implant
#define RESPONSE_PONG               0x022 // Response to ping
#define RESPONSE_LOITER             0x022 // Response to ping
#define RESPONSE_ERROR              0x023 // Error code
