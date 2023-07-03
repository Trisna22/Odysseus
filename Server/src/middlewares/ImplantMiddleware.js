
const responseHelper = require("../helpers/ImplantResponseHelper");

const checkInitRequest = (req, res, next) => {

    if (!req.body || !req.body.username || !req.body.computername,
            !req.body.info || !req.body.id) {

        res.status(404).json({code: responseHelper.ERROR});
        return;
    }   
    
    next();
}

module.exports = {
    checkInitRequest
}