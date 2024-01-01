
const uuid = require("uuid");
    
const createID = () => {

    return uuid.v4();
}

module.exports = {
    createID
}