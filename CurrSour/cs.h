#ifndef CS_H
#define CS_H

#define NUM_CS_PINS 4

typedef struct {
  uint8_t pin; 
} CS_Map;

static const CS_Map cs_pin_map[NUM_CS_PINS] = {
    [0] = { .pin=4 }, // CS0 is controled with a number which is defined in ../lib/pin_num.h
    [1] = { .pin=5 },
    [2] = { .pin=6 },
    [3] = { .pin=7 }
};

extern void CurrSour(void);

#endif // CS_H 
