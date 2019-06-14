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
    ifstream cmdline;
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
    return PIDs;
}
/****************************************************** */
/*
Implement getVmSize function
Arguments: process Pid
Return: Virtual memory in GB used by rhe process
 */
std::string ProcessParser::getVmSize(string pid)
{
  string path = Path::basePath() + pid + Path::statusPath();
  string param = "VmData";
  ifstream status =Util::getStream(path);
  string line;
  string value;
  string VmKb;
  //loop on all the lines in the file
  while (std::getline(status,line))
  {
    //check if this line contain the required parameter
    if (line.compare(0,param.size(),param) == 0)
    {
      stringstream s(line);
      //set the parameter name in value and the Vm used in VmKb
      s>>value>>VmKb;
      break;
    } 
    
  }
  // convert the Vm used from KB to GB and return it as string 
   return to_string((stof(VmKb)/float(1024*1024)));
}
/************************************************************************ */
/*
Implementation of getCpuPercent function
Arguments: Process Pid
Return: CPU usage % as a string
 */

std::string ProcessParser::getCpuPercent(string pid)
{
  string path = Path::basePath() + pid + "/" + Path::statPath;
  ifstream pstat =Util::getStream(path);
  string line;
  vector<string> values;
  float res;
  string read;
  std::getline(stream,line);
  stringstream s(line);
  while (s>>read)
    values.push_back(read);
  float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return to_string(result);
}
/***************************************************************** */
/*
Implementation of getSysUpTime function
Arguments: None
Return: SysUpTime as an integer
 */
 long int ProcessParser::getSysUpTime()
 {
  string path = Path::basePath() + Path::upTimePath();
  ifstream stream =Util::getStream(path);
  string line;
  getline(stream,line);
  stringstream s(line);
  string sysuptime;
  s>>sysuptime;
  return stoi(sysuptime);
 }
 /************************************************************ */
 /*
 Implementaion of getProcUpTime function
 Argumnets: process Pid
 Return: Process running tima as a string
  */
 string ProcessParser::getProcUpTime(string pid)
 {
    string line;
    string read;
    vector<string> stat;
    ifstream stream = Util::getStream((Path::basePath() + pid + "/" +  Path::statPath()));
    getline(stream, line);
    stringstream s(line);
    while(s>>read)
    stat.push_back(read);
    return to_string(float(stof(stat[13])/sysconf(_SC_CLK_TCK)));


 }