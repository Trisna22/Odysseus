
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

module.exports = {
    hasAuthToken,
    checkNickName
}