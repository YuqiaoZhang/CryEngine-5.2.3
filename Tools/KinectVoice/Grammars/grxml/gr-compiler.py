# -----------------------------------------------------------------------
# Grammar Compiler
# Author: Dario Sancho (dariop@crytek.com)
# Date: 31.07.2013
#
# Description: This script is meant to be placed in the directory 
# where the grammar xml files (.grxml) are stored. Executing
# the script will compile these grammar files into binary ones (.cfg)
# and output them in the same directory. You will have to move them (.cfg)
# manually to their appropriate directory. This is done purposefully 
# to avoid overwriting existing compiled files unintentionally.
# -----------------------------------------------------------------------

import os
import subprocess

XDK_GRAMMAR_COMPILER = 'xbpg'
XDK_GRAMMAR_COMPILER_FULL_PATH = ''

def setCompilerPath():
	xdkBinPath = os.getenv('DurangoXDK')
	
	if xdkBinPath == None:
		print "[Error]: I cannot find the XDK Environment Variable (DurangoXDK)"
		xdkBinPath = '.\\'

	xdkBinPath = xdkBinPath + 'bin\\'
	global XDK_GRAMMAR_COMPILER_FULL_PATH;
	XDK_GRAMMAR_COMPILER_FULL_PATH = xdkBinPath + XDK_GRAMMAR_COMPILER
	
	print '[XDK grammar compiler path]: '
	print XDK_GRAMMAR_COMPILER_FULL_PATH
	print '\n'

def getFiles(path, result):
	os.chdir(path)
	for files in os.listdir("."):
		if os.path.isdir(path + '/' + files):
			result.append(files)

def getGrxmlFiles(path, grammars):
	os.chdir(path)
	for files in os.listdir("."):
		if files.endswith(".grxml"):
			grammars.append(files)

def compileGrammar(grammars, pathOut):
	total = len(grammars)
	cnt = 0;
	for gr in grammars:
		cnt +=1
		print '['+str(cnt)+'/'+str(total)+'] : ' + gr
		print XDK_GRAMMAR_COMPILER_FULL_PATH
		subprocess.call([XDK_GRAMMAR_COMPILER_FULL_PATH, gr])

def print_signature():
	print "\n"
	print "                         ''~``"
	print "                        ( o o )"
	print "+------------------.oooO--(_)--Oooo.------------------+"
	print "|                                                     |"
	print "|     COMPILATION    .oooO            FINISHED        |"
	print "|                    (   )   Oooo.                    |"
	print "+---------------------\ (----(   )--------------------+"
	print "                       \_)    ) /"
	print "                             (_/"


def print_header():
	print "\n"
	print "         ______________________________________"
	print "________|          Grammar Compiler            |_______ "
	print "\       |               v0.1                   |      / "
	print " \      |       dariop@crytek.com (Dario)      |     /  "
	print " /      |______________________________________|     \  "
	print "/__________)                                (_________\ "
	print "\n"

if __name__ == '__main__':
	print_header()
	scriptDir = os.path.dirname(os.path.abspath(__file__));
	dirs = []
	setCompilerPath()
	getFiles(scriptDir, dirs)
	for d in dirs:
		os.chdir(scriptDir)
		print d
		grammars = []
		dirPath = "./" + d
		print dirPath
		getGrxmlFiles(dirPath, grammars)
		print "Number of grammars found: ", len(grammars)
		print "\nStarting Compilation...\n"
		compileGrammar(grammars, "./")
		print "\nCompilation Finished..."
	print_signature()
