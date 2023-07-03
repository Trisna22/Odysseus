/**
 * Route for /client 
*/
const router = express.Router();

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
})



module.exports = router;