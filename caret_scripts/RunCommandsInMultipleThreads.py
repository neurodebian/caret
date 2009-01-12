import os
import subprocess
from threading import Thread

##-----------------------------------------------------------------------------
#
# Class that runs a command in a thread.
# If the command sent to the constructor is a list, each item in the list
# is executed as a command sequentially
#
class RunCommandInThread(Thread):
   #
   # Constructor
   # "commandIn" may be either a command or a list containing commands
   #
   def __init__ (self, commandIn):
      Thread.__init__(self)
      
      #
      # If input is a string, place it into a list
      # If input is a list, it's ok
      #
      if isinstance(commandIn, str):
         self.commandList = [commandIn]
      elif isinstance(commandIn, list):
         self.commandList = commandIn
      else:
         self.commandList = []
         
      #print "RunCommandInThread commandList:", self.commandList
      #print "RunCommandInThread commandIn:", commandIn
      #print "RunCommandInThread type(commandIn):", type(commandIn)
      
      
   def run(self):
      if len(self.commandList) == 0:
         print "ERROR COMMAND LIST EMPTY"
         return
         
      for cmd in self.commandList:
         p = subprocess.Popen(cmd, shell=True)
         result = os.waitpid(p.pid, 0)
         if (result <= 0) :
            print "ERROR COMMAND FAILED: "
            print "   ", cmd

###XX ##-----------------------------------------------------------------------------
###XX #
###XX # Run a a list of commands in multiple threads
###XX #
###XX class RunCommandsInMultipleThreads(Thread):
###XX    def __init__(self, commandList, numberOfThreads):
###XX       Thread.__init__(self)
###XX       self.commandList = commandList
###XX       self.maximumNumberOfThreads = numberOfThreads
###XX       
###XX    def run(self):
###XX       if len(self.commandList) == 0:
###XX          print "ERROR: List of commands is empty"
###XX          return
###XX       
###XX       numberOfCommandsToRun = len(self.commandList)
###XX       
###XX       i = 0
###XX       while i < numberOfCommandsToRun: 
###XX          #
###XX          # Determine how many threads to create
###XX          #
###XX          numThreadsToCreate = self.maximumNumberOfThreads
###XX          numCommandsLeft = numberOfCommandsToRun - i
###XX          if (numCommandsLeft < numThreadsToCreate) :
###XX             numThreadsToCreate = numCommandsLeft
###XX             
###XX          #
###XX          # Get directories that are to be executed
###XX          #
###XX          commandsToRunList = []
###XX          j = 0
###XX          while j < numThreadsToCreate:
###XX             commandsToRunList.append(self.commandList[i])
###XX             i = i + 1
###XX             j = j + 1
###XX          
###XX          #
###XX          # Execute the directories
###XX          #
###XX          threadList = []
###XX          for commandToRun in commandsToRunList :
###XX             #print "STARTING: ", commandToRun
###XX             cmdThread = RunCommandInThread(commandToRun)
###XX             threadList.append(cmdThread)
###XX             cmdThread.start()
###XX             
###XX          for thread in threadList:
###XX             thread.join()
      
##-----------------------------------------------------------------------------
#
# Run a a list of commands in multiple threads
#
def runCommands(commandList, maximumNumberOfThreads):
   if len(commandList) == 0:
      print "ERROR: List of commands is empty"
      return
   
   numberOfCommandsToRun = len(commandList)
   
   i = 0
   while i < numberOfCommandsToRun: 
      #
      # Determine how many threads to create
      #
      numThreadsToCreate = maximumNumberOfThreads
      numCommandsLeft = numberOfCommandsToRun - i
      if (numCommandsLeft < numThreadsToCreate) :
         numThreadsToCreate = numCommandsLeft
         
      #
      # Get directories that are to be executed
      #
      commandsToRunList = []
      j = 0
      while j < numThreadsToCreate:
         commandsToRunList.append(commandList[i])
         i = i + 1
         j = j + 1
      
      #
      # Execute the directories
      #
      threadList = []
      for commandToRun in commandsToRunList :
         #print "STARTING: ", commandToRun
         cmdThread = RunCommandInThread(commandToRun)
         threadList.append(cmdThread)
         cmdThread.start()
         
      for thread in threadList:
         thread.join()
      
