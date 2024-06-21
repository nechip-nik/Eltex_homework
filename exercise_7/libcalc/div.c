#include "../include/libcalc.h"

int div(int param_1, int param_2) {
  if (param_2 == 0) {
    return 0;
  }
  return param_1 / param_2;
}
