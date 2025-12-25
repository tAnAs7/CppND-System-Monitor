#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

#include "linux_parser.h"

using std::ifstream;
using std::istringstream;
using std::string;
using std::to_string;
using std::vector;

namespace LinuxParser {

float MemoryUtilization() {
  string key;
  long total = 0, free = 0;
  ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (filestream >> key) {
      if (key == "MemTotal:")
        filestream >> total;
      else if (key == "MemFree:")
        filestream >> free;
    }
  }
  return total == 0 ? 0.0f : static_cast<float>(total - free) / total;
}

long UpTime() {
  ifstream filestream(kProcDirectory + kUptimeFilename);
  long uptime = 0;
  if (filestream.is_open()) filestream >> uptime;
  return uptime;
}

vector<int> Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), ::isdigit))
        pids.push_back(stoi(filename));
    }
  }
  closedir(directory);
  return pids;
}

int TotalProcesses() {
  string key;
  int value;
  ifstream filestream(kProcDirectory + kStatFilename);
  while (filestream >> key >> value)
    if (key == "processes") return value;
  return 0;
}

int RunningProcesses() {
  string key;
  int value;
  ifstream filestream(kProcDirectory + kStatFilename);
  while (filestream >> key >> value)
    if (key == "procs_running") return value;
  return 0;
}

string OperatingSystem() {
  string line, key, value;
  ifstream filestream(kOSPath);
  while (getline(filestream, line)) {
    std::replace(line.begin(), line.end(), '=', ' ');
    std::replace(line.begin(), line.end(), '"', ' ');
    istringstream linestream(line);
    linestream >> key;
    if (key == "PRETTY_NAME") {
      getline(linestream, value);
      return value.substr(1);
    }
  }
  return "";
}

string Kernel() {
  string os, version, kernel;
  ifstream filestream(kProcDirectory + kVersionFilename);
  filestream >> os >> version >> kernel;
  return kernel;
}

vector<string> CpuUtilization() {
  string line, value;
  vector<string> values;
  ifstream filestream(kProcDirectory + kStatFilename);
  getline(filestream, line);
  istringstream linestream(line);
  linestream >> value;
  while (linestream >> value) values.push_back(value);
  return values;
}

long Jiffies() { return ActiveJiffies() + IdleJiffies(); }

long ActiveJiffies() {
  auto values = CpuUtilization();
  long sum = 0;
  for (int i : {kUser_, kNice_, kSystem_, kIRQ_, kSoftIRQ_, kSteal_})
    sum += stol(values[i]);
  return sum;
}

long IdleJiffies() {
  auto values = CpuUtilization();
  return stol(values[kIdle_]) + stol(values[kIOwait_]);
}

long ActiveJiffies(int pid) {
  string line, value;
  ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  getline(filestream, line);
  istringstream linestream(line);
  vector<string> values;
  while (linestream >> value) values.push_back(value);
  return stol(values[13]) + stol(values[14]) + stol(values[15]) +
         stol(values[16]);
}

string Command(int pid) {
  string line;
  ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  getline(filestream, line);
  return line;
}

string Ram(int pid) {
  string key, value;
  ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (filestream >> key >> value)
    if (key == "VmSize:") return to_string(stol(value) / 1024);
  return "0";
}

string Uid(int pid) {
  string key, value;
  ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  while (filestream >> key >> value)
    if (key == "Uid:") return value;
  return "";
}

string User(int pid) {
  string uid = Uid(pid);
  string line, name, x, id;
  ifstream filestream(kPasswordPath);
  while (getline(filestream, line)) {
    std::replace(line.begin(), line.end(), ':', ' ');
    istringstream linestream(line);
    linestream >> name >> x >> id;
    if (id == uid) return name;
  }
  return "";
}

long int UpTime(int pid) {
  string line, value;
  ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  getline(filestream, line);
  istringstream linestream(line);
  vector<string> values;
  while (linestream >> value) values.push_back(value);
  long starttime = stol(values[21]);
  return UpTime() - starttime / sysconf(_SC_CLK_TCK);
}

}  // namespace LinuxParser