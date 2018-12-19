#ifndef CSP_H
#define CSP_H

#define NUM_CSP_PINS 1

typedef struct {
  uint8_t pin; 
} CSP_Map;

static const CSP_Map csp_pin_map[NUM_CSP_PINS] = {
    [0] = { .pin=9 }, // CSP0 is controled with a number which is defined in ../lib/pin_num.h
};

extern void CurrSourICP(void);

#endif // CSP_H 
