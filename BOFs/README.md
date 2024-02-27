# Beacon Object Files (BOFs)
All beacon object files given with the framework, the rest you have to create on your own.

## OutputFormatter.h
If your BOF wants to output a string, you can do this by adding the OutputFormatter.h to your BOF and call the setOutputData() function. After finishing the BOF, the main program will call getOutputData() and get the output string.
