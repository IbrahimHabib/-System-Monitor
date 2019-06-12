#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};


/********************************************************************/
/*
Implementation of getCmd functtion
Arguments: Process PID
Return: String of the Cmd used to start the process
 */

string ProcessParser::getCmd(string pid)
{ 
    string path = Path::basePath() + pid + Path::cmdPath();
   // open the cmdline file 
    cmdline = Util::getStream(path);
    string cmd;
    // get the process cmd and return it
    getline(cmdline, cmd);   
    return cmd;
}
/*********************************************************************/
/*
Implementation of getPidList()
Arguments: None
Return: A vector of string of all active processes' Pid
 */
vector<string> ProcessParser::getPidList()
{
   DIR* dir;
   struct dirent *ent;
   vector<string> PIDs;
   bool isps = true;
   //check if the dir exists
   if (!(dir = opendir(Path::basePath)))
   throw std::runtime_error(std::strerror(errno));
    //loop on the items in the dir
   while (ent = readdir(dir))
   {
       // if the item is not dir skip the loop until you get a dir
      if (ent->d_type != DT_DIR)
            continue;
    // check if the dir name is a process PID and it consistsonly of digits
      for (int i=0; i<strlen(ent->d_name); i++)
      {
           if (isdigit(ent->d_name[i]))
           isps = true;
           else
           isps =false;
     }
    // if the dir name is aprocess PID push it back to the vector of PIDs
     if(isps)
       PIDs.push_back(ent->d_name); 
     } 
    // check if the dir is closed save and return the vector of PIDS
      if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));
    return container;
}