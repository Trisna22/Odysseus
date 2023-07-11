
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
    if (payload && payload.name && payload.description && payload.os ) {
        // && payload.categories && payload.os) {
            
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

const checkPayloadExists = (req, res, next) => {

    // Add code that checks if payload exists.
    next();
}

const checkPayloadValid = (req, res, next) => {

    // Add code that compiles payload.
    next();
}

module.exports = {
    hasAuthToken,
    checkNickName,
    checkNewPayload,
    checkPayloadValid,
    checkPayloadExists
}