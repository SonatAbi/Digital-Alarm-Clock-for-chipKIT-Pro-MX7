/* 
 * File:   Digilent_config.h
 * Author: WTB
 *
 * Created on January 22, 2016, 10:50 AM
 */

#ifndef DIGILENT_CONFIG_H
#define	DIGILENT_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

// Digilent board configuration
#pragma config ICESEL       = ICS_PGx1  // ICE/ICD Comm Channel Select
#pragma config DEBUG        = OFF       // Debugger Disabled for Starter Kit
#pragma config FNOSC        = PRIPLL	// Oscillator selection
#pragma config POSCMOD      = XT	// Primary oscillator mode
#pragma config FPLLIDIV     = DIV_2	// PLL input divider
#pragma config FPLLMUL      = MUL_20	// PLL multiplier
#pragma config FPLLODIV     = DIV_1	// PLL output divider
#pragma config FPBDIV       = DIV_8	// Peripheral bus clock divider
#pragma config FSOSCEN      = OFF	// Secondary oscillator enable


#ifdef	__cplusplus
}
#endif

#endif	/* DIGILENT_CONFIG_H */

