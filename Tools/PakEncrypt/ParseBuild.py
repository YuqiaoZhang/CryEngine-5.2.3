import argparse
import os
import shutil
import subprocess
import sys

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Performs an encryption operation on each pak file in a directory")
    parser.add_argument("operation", help="The cryptographic operation to perform.", choices=["encrypt", "decrypt", "sign"])
    parser.add_argument("input_dir", help="The directory to search for pak files as inputs to the operation.")
    parser.add_argument("output_dir", help="The directory in which to write the outputs of the operation.")
    parser.add_argument("-n", "--dryrun", help="Print out the files that will be processed without processing them.", action="store_true")
    parser.add_argument("-e", "--excludefiles", nargs="*", help="A list of files inside archives to not encrypt. Must be the full path inside the archive and use forward slashes e.g. sounds/interface/olis1.fsb")
    options = parser.parse_args(sys.argv[1:])

    if options.operation=="encrypt":
        exeName="PakEncrypt.exe"
    if options.operation=="decrypt":
        exeName="PakDecrypt.exe"
    if options.operation=="sign":
        exeName="PakSign.exe"

    if options.excludefiles:
        print("Excluding the following files from the build:")
        for excludeFile in options.excludefiles:
            print("\t%s"%excludeFile)

    inplace=False
    if os.path.abspath(options.input_dir).lower() == os.path.abspath(options.output_dir).lower():
        inplace=True

    numFilesToProcess=0
    print("Counting files to process.")
    for root, dirs, files in os.walk(options.input_dir):
        numFilesToProcess+=len(files)

    numFilesProcessed=0
    completedTXTExists=False
    for root, dirs, files in os.walk(options.input_dir):
        for file in files:
            numFilesProcessed+=1
            if root == options.input_dir and file.lower() == "completed.txt":
                completedTXTExists=True
                print("Found a completed.txt in the source directory. This will be copied at the end of the transfer.")
            else:
                relPath = os.path.relpath(os.path.join(root,file),options.input_dir)
                inputPath = os.path.join(options.input_dir,relPath)
                outputPath = os.path.join(options.output_dir,relPath)
                if not options.dryrun:
                    try:
                        os.makedirs(os.path.dirname(outputPath))
                    except:
                        pass
                if file[-4:] == ".pak":
                    print("%d/%d: %s -> (%s) -> %s"%(numFilesProcessed, numFilesToProcess, inputPath, options.operation, outputPath))
                    if not options.dryrun:
                        processInput = [exeName, inputPath, outputPath]
                        if options.excludefiles:
                            for excludeFile in options.excludefiles:
                                processInput.append(excludeFile)
                        subprocess.check_call(processInput)
                elif not inplace:
                    print("%d/%d: %s -> %s"%(numFilesProcessed, numFilesToProcess, inputPath, outputPath))
                    if not options.dryrun:
                        if os.path.isfile(outputPath):
                            os.remove(outputPath)
                        shutil.copy(inputPath, outputPath)

    if completedTXTExists==True and not inplace and not options.dryrun:
        shutil.copy(os.path.join(options.input_dir,"completed.txt"),os.path.join(options.output_dir,"completed.txt"))
