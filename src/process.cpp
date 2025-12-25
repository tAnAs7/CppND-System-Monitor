#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  long uptime = LinuxParser::UpTime();
  long proc_uptime = LinuxParser::UpTime(pid_);
  long total_time = LinuxParser::ActiveJiffies(pid_);
  if (proc_uptime <= 0) return 0.0f;
  return static_cast<float>(total_time) / sysconf(_SC_CLK_TCK) / proc_uptime;
}

string Process::Command() { return LinuxParser::Command(pid_); }

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() { return LinuxParser::User(pid_); }

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}