/**
 * Route for /payload
 */
const router = express.Router();
const userMiddleware = require("../../middlewares/UserMiddleware");

router.get("/", userMiddleware.hasAuthToken, (req, res) => {

    // Get all payloads.

    res.json(databaseHelper.getPayloads())
})

router.post("/", userMiddleware.hasAuthToken, userMiddleware.checkNewPayload, 
    userMiddleware.checkPayloadExists, userMiddleware.checkPayloadValid, (req, res) => {



})

router.get("/jobs", userMiddleware.hasAuthToken, (req, res) => {
    
    res.json(databaseHelper.getJobs())
})

module.exports = router;