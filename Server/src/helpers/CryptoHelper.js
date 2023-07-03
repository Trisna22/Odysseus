
const { randomUUID } = require("crypto");
    
const createID = () => {

    return randomUUID();
}

module.exports = {
    createID
}