/**
 * Route for /client 
*/
const router = express.Router();
const userMiddleware = require("../../middlewares/UserMiddleware");

router.post("/login", (req, res) => {

    const token = cryptoHelper.createID();
    if (serverOptions.useAuthentication) {

        if (req.body.username == "ramb0" && req.body.password == "pass") {
            
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

// Retrieve all slaves.
router.get("/slaves", userMiddleware.hasAuthToken, (req, res) => {
    
    res.json(databaseHelper.getSlaves());
})

// Nickname an slave.
router.post("/slaves/nickname", userMiddleware.hasAuthToken, 
    userMiddleware.checkNickName, (req, res) => {
    
    databaseHelper.setNickname(req.body.slaveId, req.body.nickname);
    res.json({message: "Success"});
})

module.exports = router;