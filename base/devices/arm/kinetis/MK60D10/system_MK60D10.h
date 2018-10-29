#ifndef SYSTEM_MK60D10_H
#define SYSTEM_MK60D10_H

extern uint32_t SystemCoreClock;

#ifdef __cplusplus
extern "C"
{
#endif

extern void disable_watchdog();

extern void system_init();

#ifdef __cplusplus
}
#endif


#endif
