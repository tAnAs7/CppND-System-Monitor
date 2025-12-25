#include "process.h"

#include <unistd.h>

#include <string>

#include "linux_parser.h"

using std::string;

Process::Process(int pid) : pid_(pid) {}

int Process::Pid() const { return pid_; }

float Process::CpuUtilization() const {
  long proc_uptime = LinuxParser::UpTime(pid_);
  long total_time = LinuxParser::ActiveJiffies(pid_);
  if (proc_uptime <= 0) return 0.0f;
  return static_cast<float>(total_time) / sysconf(_SC_CLK_TCK) / proc_uptime;
}

string Process::Command() const { return LinuxParser::Command(pid_); }

string Process::Ram() const { return LinuxParser::Ram(pid_); }

string Process::User() const { return LinuxParser::User(pid_); }

long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  return CpuUtilization() > a.CpuUtilization();
}