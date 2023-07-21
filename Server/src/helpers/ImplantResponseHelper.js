
const Response = {
    EXIT: 0x00,         // Exit malware
    CLOSE: 0x00,        // Exit malware
    INIT: 0x01,         // Init job
    NEW_OBJECT: 0x02,   // New job
    NOT_INIT: 0x03,     // Slave hasn't been init before.
    DESTROY: 0x666,     // Destroy implant
    LOITER: 0x022,      // No jobs currently (job finished)
    PONG: 0x022,        // Response to ping
    ERROR: 0x023        // Error code
}

module.exports = Response;