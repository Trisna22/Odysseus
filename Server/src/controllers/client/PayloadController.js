/**
 * Route for /payload
 */
const router = express.Router();
const authMiddleware = require("../../middlewares/AuthMiddleware");

router.get("/", authMiddleware.hasAuthToken, (req, res) => {

    // Get all payloads.

    res.json([
        {payload: "bomb",id: 0},
        {payload: "nuke",id: 1}
    ])
})

router.get("/jobs", authMiddleware.hasAuthToken, (req, res) => {
    
    // Get all active jobs.

    res.json({/*allJobs*/})
})

module.exports = router;