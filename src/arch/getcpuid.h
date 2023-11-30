#if !defined(__GNUC__)
#error "GCC required"
#else
#ifndef _GETCPUID_H
#define _GETCPUID_H
//Check if we are using gcc compiler

/* Example: Get CPU's model number */
int get_model(void);
 
/* Example: Check for builtin local APIC. */
int check_apic(void);

#endif
#endif