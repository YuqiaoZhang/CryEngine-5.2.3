# subprocess - A .NET implementation of the Python subprocess module
# 
# Copyright (c) Jeff Hardy 2007.
#
# This source code is subject to terms and conditions of the Microsoft Public License. A 
# copy of the license can be found at 
# http://www.microsoft.com/resources/sharedsource/licensingbasics/publiclicense.mspx. If 
# you cannot locate the Microsoft Public License, please send an email to 
# jdhardy@gmail.com. By using this source code in any fashion, you are agreeing to be bound 
# by the terms of the Microsoft Public License.
#
# You must not remove this notice, or any other, from this software.
# 
# list2cmdline is taken from the Python 2.5 distribution, so the Python license applies.
# See http://www.python.org/2.4/license for details.

import sys, os
from System.Diagnostics import Process
from System.IO import MemoryStream

PIPE = -1
STDOUT = -2

class Popen(object):
	""" Need documentation - copy from Python subprocess.py? """
	def __init__(self, args, bufsize=0, executable=None, stdin=None, stdout=None, stderr=None, preexec_fn=None, close_fds=False, shell=False, cwd=None, env=None, universal_newlines=False, startupinfo=None, creationflags=0):
		if preexec_fn:
			raise ValueError("preexec_fn is not supported on Windows platforms")
		
		if close_fds:
			raise ValueError("close_fds is not supported on Windows platforms")
		
		if universal_newlines:
			raise ValueError("universal_newlines is not supported on .NET platforms")
		
		if startupinfo:
			raise ValueError("startupinfo is not supported on .NET platforms")
		
		if creationflags:
			raise ValueError("creationflags is not supported on .NET platforms")
		
		if not isinstance(bufsize, (int, long)):
			raise TypeError("bufsize must be an integer")
		
		if stdin is not None and stdin != PIPE:
			raise NotImplementedError("Cannont redirect stdin yet.")
		
		if stderr == STDOUT:
			raise NotImplementedError("Cannont redirect stderr to stdout yet.")
		
		args = args if not isinstance(args, str) else args.split()
		self.process = Process()
		self.process.StartInfo.UseShellExecute = False
		
		if not shell:
			self.process.StartInfo.FileName = executable or args[0]
			self.process.StartInfo.Arguments = list2cmdline(args[1:])
			self.process.StartInfo.CreateNoWindow = True
		else:
			self.process.StartInfo.FileName = executable or os.environ['COMSPEC']
			self.process.StartInfo.Arguments = list2cmdline(['/C'] + args)
			self.process.StartInfo.CreateNoWindow = False
		
		if env:
			self.process.StartInfo.EnvironmentVariables.Clear()
			for k, v in env.items():
				self.process.StartInfo.EnvironmentVariables.Add(k, v)
		
		self.process.StartInfo.WorkingDirectory = cwd or os.getcwd()
		
		self.process.StartInfo.RedirectStandardInput = stdin is not None
		self.process.StartInfo.RedirectStandardOutput = stdout is not None
		self.process.StartInfo.RedirectStandardError = stderr is not None
		
		combinedOutput = file(MemoryStream()) if stdout == PIPE and stderr == STDOUT else None
		#~ print combinedOutput
		
		if (stdout is not None and stdout != PIPE) or (stdout == PIPE and stderr == STDOUT):
			self.process.OutputDataReceived += Redirector(combinedOutput or stdout, "stdout")
		
		if stderr is not None and stderr != PIPE:
			self.process.ErrorDataReceived += Redirector(combinedOutput or stderr, "stderr")
		
		self.process.Start()
		self.pid = self.process.Id
		
		if (stdout is not None and stdout != PIPE) or (stdout == PIPE and stderr == STDOUT):
			#~ print "stdout: start async"
			self.process.BeginOutputReadLine()
		
		if stderr is not None and stderr != PIPE:
			#~ print "stderr: start async"
			self.process.BeginErrorReadLine()
		
		self.stdin = file(self.process.StandardInput.BaseStream) if stdin == PIPE else None
		self.stdout = combinedOutput or (file(self.process.StandardOutput.BaseStream) if stdout == PIPE else None)
		self.stderr = combinedOutput or (file(self.process.StandardError.BaseStream) if stderr == PIPE else None)
	
	def get_returncode(self):
		return self.poll()
	
	returncode = property(get_returncode)
	
	def poll(self):
		return self.process.ExitCode if self.process.HasExited else None
	
	def wait(self):
		self.process.WaitForExit()
		return self.process.ExitCode

	def kill(self):
		self.process.Kill()
		self.process.WaitForExit()
	
	def communicate(self, input=None):
		raise NotImplementedError("Popen.communicate")
		
		if input and self.stdin:
			self.stdin.write(input)
			self.stdin.flush()
		
		#  return (self.stdout.read() if self.stdout else None, self.stderr.read() if self.stderr else None)
		#  return (self.stdout.read() if self.stdout else None, "")
		return ("" if self.stdout else None, "" if self.stderr else None)

def call(*popenargs, **kwargs):
	p = Popen(*popenargs, **kwargs)
	return p.wait()
	
def check_call(*popenargs, **kwargs):
	return call(*popenargs, **kwargs)

# Taken from Python 2.5 subprocess.py
def list2cmdline(seq):
	"""
	Translate a sequence of arguments into a command line
	string, using the same rules as the MS C runtime:

	1) Arguments are delimited by white space, which is either a
	   space or a tab.

	2) A string surrounded by double quotation marks is
	   interpreted as a single argument, regardless of white space
	   contained within.  A quoted string can be embedded in an
	   argument.

	3) A double quotation mark preceded by a backslash is
	   interpreted as a literal double quotation mark.

	4) Backslashes are interpreted literally, unless they
	   immediately precede a double quotation mark.

	5) If backslashes immediately precede a double quotation mark,
	   every pair of backslashes is interpreted as a literal
	   backslash.  If the number of backslashes is odd, the last
	   backslash escapes the next double quotation mark as
	   described in rule 3.
	"""

	# See
	# http://msdn.microsoft.com/library/en-us/vccelng/htm/progs_12.asp
	result = []
	needquote = False
	for arg in seq:
		bs_buf = []

		# Add a space to separate this argument from the others
		if result:
			result.append(' ')

		needquote = (" " in arg) or ("\t" in arg)
		if needquote:
			result.append('"')

		for c in arg:
			if c == '\\':
				# Don't know if we need to double yet.
				bs_buf.append(c)
			elif c == '"':
				# Double backspaces.
				result.append('\\' * len(bs_buf)*2)
				bs_buf = []
				result.append('\\"')
			else:
				# Normal char
				if bs_buf:
					result.extend(bs_buf)
					bs_buf = []
				result.append(c)

		# Add remaining backspaces, if any.
		if bs_buf:
			result.extend(bs_buf)

		if needquote:
			result.extend(bs_buf)
			result.append('"')

	return ''.join(result)

class _InputStream(object):
	def __init__(self, streamReader):
		self.reader = streamReader
	
	def read(self, size=0):
		if size < 1:
			return self.reader.ReadToEnd()
		else:
			buffer = System.Array.CreateInstance(System.Char, size)
			self.reader.ReadBlock(buffer, 0, size)
			return "".join(buffer)
	
	def flush(self):
		self.reader.Flush()
	
	def close(self):
		self.reader.Close()

class _OutputStream(object):
	def __init__(self, streamWriter):
		self.writer = streamWriter
	
	def write(self, data):
		self.writer.Write(data)	
	
	def flush(self):
		self.writer.Flush()
	
	def close(self):
		self.writer.Close()

class Redirector(object):
	def __init__(self, to, name):
		if isinstance(to, (int, long)):
			raise NotImplementedError("Cannot redirect to a file descriptor")
		
		self.to = to
		self.name = name
		
		#~ print "redirection:", self.to, "for", self.name
	
	def __call__(self, sender, e):
		if e.Data:
			#~ print "%s: %s" % (self.name, e.Data)
			self.to.write(e.Data)
			self.to.flush()
