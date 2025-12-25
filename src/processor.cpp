#include "processor.h"

#include <vector>

#include "linux_parser.h"

float Processor::Utilization() {
  static long prev_idle = 0;
  static long prev_total = 0;

  long idle = LinuxParser::IdleJiffies();
  long total = LinuxParser::Jiffies();

  long idle_delta = idle - prev_idle;
  long total_delta = total - prev_total;

  prev_idle = idle;
  prev_total = total;

  if (total_delta == 0) return 0.0f;
  return 1.0f - static_cast<float>(idle_delta) / total_delta;
}