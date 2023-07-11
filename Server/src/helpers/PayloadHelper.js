
const fs = require("fs")
const {exec, execFileSync, execSync} = require("child_process");

global.PAYLOAD_PATH = "./payloads/";

const COMMAND_COMPILE = "g++ -shared -fPIC -o " ;

const compilePayload = (id, variables, success, error) => {

    const path = PAYLOAD_PATH + id + "/"
    if (variables) {

        // Fill in the variables.
        var data = fs.readFileSync(path + "source.cpp").toString();

        for (var v of variables) {
            if (v.vartype == "number") {
                data = data.replace(v.varname, "0");
            } else {
                data = data.replace(v.varname, "\"string\"");
            }
        }

        // Now compile temporary file.
        fs.writeFileSync("/tmp/source.cpp", data);

        exec(COMMAND_COMPILE + 
            path + "object.so" + // Output path 
            " " +
            "/tmp/source.cpp" // Source path
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

        compilePayload(id, variables, success, error);
    });
}

const getSources = (payloads) => {

    let newPayloads = [];

    payloads.forEach(payload => {

        var source = "";
        try {
            source = fs.readFileSync(payload.location + "source.cpp").toString()

        } catch(err) {

            console.error("Failed to load payload " + payload);
            console.error(err);
            source = "Failed to load source code!"
        }
            console.log(source);
            newPayloads.push({
                id: payload.id,
                name: payload.name,
                variables: payload.variables,
                description: payload.description,
                os: payload.os,
                categories: payload.categories ? payload.categories : "",
                location: payload.location,
                source: source 
            })
    });
    
    return newPayloads;
}

module.exports = {
    createNewPayload,
    compilePayload,
    getSources
}