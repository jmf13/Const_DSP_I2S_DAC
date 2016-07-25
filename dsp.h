/**
 * @file         dsp.h
 * @version      1.0
 * @date         2015
 * @author       Christoph Lauer
 * @compiler     armcc
 * @copyright    Christoph Lauer engineering
 */
 
#ifndef __DSP_H
#define __DSP_H

#include <stdint.h>

void dsp(int16_t* buffer, int length);
void initFilter(void);

#endif