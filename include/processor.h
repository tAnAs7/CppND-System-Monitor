#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();

 private:
  long prev_idle_{0};
  long prev_total_{0};
};

#endif
