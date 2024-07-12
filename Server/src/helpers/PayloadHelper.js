
const fs = require("fs")
const {exec, execFileSync, execSync} = require("child_process");

global.PAYLOAD_PATH = "./payloads/";
global.JOBS_PATH = "./jobs/"

const COMMAND_COMPILE = "g++ -c -o " ;
const TEMP_SOURCE = "/tmp/.source.cpp";

const compilePayloadTest = (id, variables, success, error) => {

    const path = PAYLOAD_PATH + id + "/"
    if (variables) {

        // Fill in the variables.
        var data = fs.readFileSync(path + "source.cpp").toString();
        var copyData = data;

        for (var v of variables) {
            if (v.vartype == "number") {
                data = data.replace(v.varname, "0");
            } else {
                data = data.replace(v.varname, "\"string\"");
            }
        }

        // Now compile temporary file.
        fs.writeFileSync(TEMP_SOURCE, data);
        // Replace the temp file with the target source (temporary).
        fs.renameSync(TEMP_SOURCE, path + "source.cpp"); 

        exec(COMMAND_COMPILE + 
            path + "object.so" + // Output path 
            " " +
            path + "source.cpp" // Source path
            , (err, stdout, stderr) => {
                
                if (err) {
                    console.error("Failed to compile code!");
                    console.error(stderr);
                    error(stderr, true);
                    return;
                }
            
            // Now change the original values to path.
            // We replaced the path with the file with filled in variables remember!
            fs.writeFileSync(path + "source.cpp", copyData)
            success(path);
        })
    }
    else {   
        exec(COMMAND_COMPILE + 
            path + "object.so" + // Output path 
            " " +
            path + "source.cpp" // Source path
            , (err, stdout, stderr) => {
                
            if (err) {
                console.error("Failed to compile code!");
                console.error(stderr);
                error(stderr, true);
                return;
            }
                
            success(path);
        })
    }
}

const compilePayload = (payload, jobId, variables, success, error) => {
    
    const path = PAYLOAD_PATH + payload.id + "/";
    let inputFile = path + "source.cpp";
    if (variables && payload.variables && payload.variables.length > 0) {

        // Fill in the variables.
        var data = fs.readFileSync(path + "source.cpp").toString();

        for (var i = 0; i < payload.variables.length; i++) {

            let v = payload.variables[i];
            if (v.varname == variables[i].name)
                v.value = variables[i].value;

            console.log("Changing variable " + v.varname + " to " + v.value)

            if (v.vartype == "number") {
                data = data.replace(v.varname, v.value);
            } else {
                data = data.replace(v.varname, "\"" + v.value +"\"");
            }

            // Now compile temporary file.
            fs.writeFileSync(TEMP_SOURCE, data);   
            fs.renameSync(TEMP_SOURCE, inputFile); // Replace the temp file with the target source.
        }
    }

    fs.mkdirSync(JOBS_PATH + jobId);
    exec(COMMAND_COMPILE + 
            JOBS_PATH + jobId + "/object.so" + // Output path 
            " " +
            inputFile // Source path
            , (err, stdout, stderr) => {
                
        if (err) {
            console.error("Failed to compile code!");
            console.error(stderr);
            error(stderr, true);
            return;
        }

        // Get file size for completion.
        var stats = fs.statSync(JOBS_PATH + jobId + "/object.so");
        success(JOBS_PATH + jobId + "/object.so", stats.size);
    })
}

const createNewPayload = (payloadObject, id, variables, success, error) => {

    const path = PAYLOAD_PATH + id;
    fs.mkdirSync(path);
    
    // First move the source to the path.
    payloadObject.mv(path + "/source.cpp", (err) => {
        if (err) {
            console.error("Failed to move the payload to the source!");
            error(err);
            return;
        }

        compilePayloadTest(id, variables, success, error);
    });
}

const getSources = (payloads) => {

    let newPayloads = [];

    payloads.forEach(payload => {

        var source = "";
        try {
            source = fs.readFileSync(payload.location + "source.cpp").toString()

        } catch(err) {
            source = "Failed to load source code!"
        }
            newPayloads.push({
                id: payload.id,
                name: payload.name,
                variables: payload.variables,
                description: payload.description,
                osPayloads: payload.osPayloads,
                categories: payload.categories ? payload.categories : "",
                location: payload.location,
                source: source,
                command: payload.command,
                createdAt: payload.createdAt
            })
    });
    
    return newPayloads;
}

const getCategories = () => {

    return [
        // Networking
        {name: "networking", color: "yellow"},
        {name: "server", color: "yellow"},
        {name: "reverse connection", color: "yellow"},

        // Payload content
        {name: "scary", color: "red"},
        {name: "command & control", color: "red"},
        {name: "destroyer", color: "red"},
        {name: "stealth", color: "red"},
        {name: "ransom", color: "red"},
        {name: "self-destruct", color: "red"},

        // Method
        {name: "filesystem",color: "gray"},
        {name: "boot", color: "gray"},
        {name: "crypto", color: "gray"},
        {name: "shell", color: "gray"},
        {name: "exec", color: "gray"},
    ]
}

const getJobs = () => {

    let jobs = databaseHelper.getJobs();

    // Add data from other modals.
    jobs.forEach((job) => {
        job.slave = databaseHelper.getSlaveById(job.slaveId);
        job.payload = databaseHelper.getPayloadById(job.payloadId);
    })

    return jobs;
}

module.exports = {
    compilePayload,
    createNewPayload,
    getSources,
    getCategories,
    getJobs
}