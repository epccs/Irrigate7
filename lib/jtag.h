#ifndef jtag_h
#define jtag_h

#if defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
#define HASJTAG 1
#endif

#ifndef HASJTAG
#   error your mcu is not supported
#endif

extern void disable_jtag(void);
extern void enable_jtag(void);

#endif // jtag_h
