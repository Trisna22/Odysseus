// Modals DB.
/**
 * Slaves: {
 *      id: $string,
 *      ip: $string,
 *      os: $string,
 *      computername: $string,
 *      nickname: $string,
 *      username: $string,
 *      info: $string
 * }
 */

/**
 * Jobs: {
 *      id: $string,
 *      slaveId: $string,
 *      payloadId: $string,
 *      createdAt: $string,
 *      finishedAt: $string,
 *      objectSize: $int,
 *      status: $string,
 *      variables: $array<$string>
 * }
 */

/**
 * Payloads: {
 *      name: $string,
 *      id:   $string,
 *      description: $string,
 *      location: $string,
 *      os: $string
 *      categories: $array<$string>
 *      variables: $array<$string>
 * }
 */


class DatabaseHelper {

    constructor() {
        // Init database...
        this.authTokens = [];
        this.slaves = [];
        this.payloads = [];
        this.jobs = [];

        this.payloads.push(
            {
                "id": "df53576a-faf1-4fda-944f-8fa7e4569a63",
                "name": "Echo",
                "variables": [
                    {
                        "varname": "TEXT",
                        "description": "String to echo.",
                        "vartype": "string"
                    }
                ],
                "description": "Echoes input var.",
                "os": "linux",
                "categories": [
                    "scary", "stealth"
                ],
                "location": "./payloads/df53576a-faf1-4fda-944f-8fa7e4569a63/",
                "source": "#include <iostream>\n#include <stdio.h>\n\n#define TO_ECHO TEXT\n\nextern \"C\" int payload_init() {\n\tprintf(\"Hello world! Message: %s\n\", TO_ECHO);\nreturn 22;\n}\n\n",
                "createdAt": "Wed, 12 Jul 2023 15:59:22 GMT"
            })

            this.payloads.push(
                {
                    "id": "8659be86-04b6-4f6b-93e2-77df4cf9bb69",
                    "name": "Reverse shell (TCP)",
                    "variables": [
                        {
                            "varname": "TARGET_PORT",
                            "description": "Target port number to use.",
                            "vartype": "number"
                        },
                        {
                            "varname": "TARGET_IP",
                            "description": "Sets the target IP address.",
                            "vartype": "string"
                        },
                    ],
                    "description": "Creates shell access trough reverse TCP connection.",
                    "os": "linux",
                    "categories": [
                        "networking", "reverse connection", "stealth", "shell", "command & control"
                    ],
                    "location": "./payloads/8659be86-04b6-4f6b-93e2-77df4cf9bb69/",
                    "source": "#include <iostream>\n\nconst char addrIP[] = TARGET_IP;\nint port = PORT_NUM;\n\nvoid hax() {\n\tprintf(\"Hacking the fuck out of it!\\n\");\n}\nextern \"C\" int payload_init()\n{\n\thax();\n}\n\n",
                    "createdAt": "Wed, 12 Jul 2023 15:59:22 GMT"
                })
    }

    addNewSlave(slave) {

        this.slaves.push({
            id: slave.id,
            ip: slave.ip,
            os: "Linux",
            computername: slave.computername,
            nickname: "No nickname",
            username: slave.username,
            info: slave.info,
        });
    }

    saveAuthToken(token, user) {
        this.authTokens.push({
            user: user,
            token: token
        })
    }

    addNewJob(job) {
        this.jobs.push({
            id: job.id,
            slaveId: job.slaveId,
            payloadId: job.payloadId,
            objectSize: job.objectSize,
            variables: job.variables,
            location: job.location,
            createdAt: new Date(Date.now()).toUTCString(),
            finishedAt: "",
            code: 0,
            status: "PENDING",
        })
    }

    setJobStatus(id, status, code = 0, finished = false) {
        this.jobs = this.jobs.map(job => {

            // Change the status of the job with id.
            if (job.id == id) {

                job.status = status;
                if (finished) {
                    job.finishedAt = new Date(Date.now()).toUTCString() 
                    job.code = code;
                }
                
                return job;
            }

            return job; // Leave the other jobs be.
        });
    }

    setNickname(slaveId, name) {

        this.slaves = this.slaves.map(slave => {
            if (slave.id == slaveId) {
                slave.nickname = name; // Only change name.
            }

            return slave;
        })
    }

    addPayload(payload) {
        this.payloads.push({
            id: payload.id,
            os: payload.os,
            name: payload.name,
            variables: payload.variables,
            location: payload.location,
            createdAt: new Date(Date.now()).toUTCString(),
            description: payload.description,
            categories: payload.categories
        })
    }

    getSlaveById(id) {
        return this.slaves.filter((entry => entry.id == id))[0];
    }

    checkAuthToken(token) {
        return this.authTokens.filter((entry => token == entry.token))[0];
    }

    getJobForSlave(id) {
        return this.jobs.filter(entry => entry.slaveId == id && entry.status == "PENDING")[0];
    }

    getJobWithId(id) {
        return this.jobs.filter(entry => entry.id == id)[0]
    }

    getPayloadById(id) {
        return this.payloads.filter(entry => entry.id == id)[0];
    }

    getSlaves() {
        return this.slaves;
    }

    getPayloads() {
        return this.payloads;
    }

    getJobs() {
        return this.jobs;
    }
}

module.exports = DatabaseHelper;