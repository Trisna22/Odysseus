/**
 * Route for /payload
 */
const router = express.Router();
const authMiddleware = require("../../middlewares/UserMiddleware");

router.get("/", authMiddleware.hasAuthToken, (req, res) => {

    // Get all payloads.

    res.json(databaseHelper.getPayloads())
})

router.get("/jobs", authMiddleware.hasAuthToken, (req, res) => {
    
    // Get all active jobs.

    res.json({/*allJobs*/})
})

module.exports = router;