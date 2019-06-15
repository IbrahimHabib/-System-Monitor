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
  values = Util::DataVector(pstat,line);
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
    vector<string> stat;
    ifstream stream = Util::getStream((Path::basePath() + pid + "/" +  Path::statPath()));
    stat = Util::DataVector(stream,line);
    return to_string(float(stof(stat[13])/sysconf(_SC_CLK_TCK)));
 }
 /******************************************************************* */
 /*
 Implementation of getProcUser function
 Argumnets: Process Pid as a string
 Return: process user as a string
 */
string ProcessParser::getProcUser(string pid)
{
    string line;
    string name = "Uid:";
    string result ="";
    string param;
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    while (std::getline(stream,line))
    {
       if (line.compare(0, name.size(),name) == 0)
       {
         stringstream s(line);
         s>>param>>result;
         break;
       }
    }
     stream = Util::getStream("/etc/passwd");
      name =("x:" + result);
      while (std::getline(stream, line)) {
        if (line.find(name) != std::string::npos) {
            result = line.substr(0, line.find(":"));
            return result;
        }
    }
    return "";
}
/******************************************************************** */
/*
Implementation of getSysCpuPercent function
Arguments: Core Number as a string
Returm vector of all core data
 */
vector<string> ProcessParser::getSysCpuPercent(string coreNumber)
{
   // reading data for each core, by passing the core number the corresponding data of this core is returned 
    string line;
    string read;
    string name = "cpu" + coreNumber;
    vector<string> values;
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line))
    {
     if (line.compare(0, name.size(),name) == 0) 
     {
     stringstream s(line);
     while (s>>read)
     values.push_back(read);
     return values;
     }
    } 
  return (vector<string>());
}
/*
get_active_Cpu_time; this function uses the data comming from getSysCpuPercent fumction adn calculate active cpu time
Arguments: vector of data comming from getSysCpuPercent for a specific core bumber
Return: Active cpu time as a float
*/
float get_sys_active_cpu_time(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}
/*
 get_sys_idle_cpu_time; this function uses the data comming from getSysCpuPercent fumction adn calculate idle cpu time
 Arguments: vector of data comming from getSysCpuPercent for a specific core bumber
Return: Idle cpu time as a float
 */
float get_sys_idle_cpu_time(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}
/*************************************************** */
/*
Implementaion of getSysRamPercent function
Arguments: None
Return: RAM usage % as a float
 */
float ProcessParser::getSysRamPercent()
{
    string line;
    string name1 = "MemAvailable:";
    string name2 = "MemFree:";
    string name3 = "Buffers:";
    ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
    string param;
    string value;
    float total_mem = 0;
    float free_mem = 0;
    float buffers = 0;
    while (std::getline(stream, line)) {
        if (total_mem != 0 && free_mem != 0)
            break;
        if (line.compare(0, name1.size(), name1) == 0) 
        {
            stringstream s(line);
            s>>param>>value;
            total_mem = stof(value);
        }
        if (line.compare(0, name2.size(), name2) == 0) {
            stringstream s(line);
            s>>param>>value;
            free_mem = stof(value);
        }
        if (line.compare(0, name3.size(), name3) == 0) {
             stringstream s(line);
            s>>param>>value;
            buffers = stof(value);
        }
    }
    //calculating usage:
    return float(100.0*(1-(free_mem/(total_mem-buffers))));
}
/************************************************************** */
/*
Implementation of getSysKernelVersion function
Arguments: None
Return: Kernel version as a string
 */
string ProcessParser::getSysKernelVersion()
{
    string line;
    string name = "Linux version ";
    ifstream stream = Util::getStream((Path::basePath() + Path::versionPath()));
    string value;
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
          string stream s(line);
          s>>value>>value>>value;
          return value;
        }
    }
    return "";
}
/******************************************************************* */
/*
Implementation of getOsName function
Argumnets: None
Return: OS name as string
 */
string ProcessParser::getOsName()
{
    string line;
    string name = "PRETTY_NAME=";

    ifstream stream = Util::getStream(("/etc/os-release"));

    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
              std::size_t found = line.find("=");
              found++;
              string result = line.substr(found);
              result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
              return result;
        }
    }
    return "";

}
/****************************************************************** */
/*
Implementation of getTotalThreads
Arguments: None
Return: total number of processes threads as integer
 */
int ProcessParser::getTotalThreads()
{
    string line;
    int result = 0;
    string name = "Threads:";
    string value;
    vector<string>_list = ProcessParser::getPidList();
    for (int i=0 ; i<_list.size();i++) {
    string pid = _list[i];
    //getting every process and reading their number of their threads
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            stringstream s(line);
            s>>value>>value;
            result += stoi(value);
            break;
        }
    }
    return result;
}
/***************************************************************** */
/*
Implementation of getTotalNumberOfProcesses function
Arguments: None
Return: Total numner of processes as integer
 */
int ProcessParser::getTotalNumberOfProcesses()
{
    string line;
    int result = 0;
    string name = "processes";
    string value;
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            stringstream s(line);
            s>>value>>value;
            result += stoi(value);
            break;
        }
    }
    return result;
}
/******************************************************************* */
/*
Implementation of getNumberOfRunningProcesses function
Arguments: None
Return: Total numner o running processes as integer
 */
int ProcessParser::getNumberOfRunningProcesses()
{
    string line;
    int result = 0;
    string name = "procs_running";
    ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(), name) == 0) {
            stringstream s(line);
            s>>value>>value;
            result += stoi(value);
            break;
        }
    }
    return result;
}
/******************************************************************* */
/*
Imnplementation of getNumberOfCores
Arguments: None
Return: Number of cpu cores
 */
int ProcessParser::getNumberOfCores()
{
    // Get the number of host cpu cores
    string line;
    string value;
    string name = "cpu cores";
    ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
    while (std::getline(stream, line)) {
        if (line.compare(0, name.size(),name) == 0) {
            vector<string> values;
            stringstream s(line);
            while(s>>value)
            values.push_back(value);
            return stoi(values[3]);
        }
    }
    return 0;
}