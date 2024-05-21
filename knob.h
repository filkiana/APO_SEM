#ifndef KNOB_H
#define KNOB_H

#include <stdint.h>
#include "mzapo_regs.h"

#define BLUE_KNOB SPILED_REG_KNOBS_8BIT_o
#define GREEN_KNOB SPILED_REG_KNOBS_8BIT_o + 1
#define RED_KNOB SPILED_REG_KNOBS_8BIT_o + 2

int8_t get_knob_value(unsigned char *spiled_base,const uint8_t knob,  const int current);


#endif // KNOB_H