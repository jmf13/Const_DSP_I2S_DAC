/*
 * DSP.c
 *
 *  Created on: 19 juil. 2016
 *      Author: JMF
 */

#include "dsp.h"

// arm c library includes
#include "stdbool.h"

// arm cmsis library includes
#define ARM_MATH_CM4 // To set compiler for CM4 cores in arm_math
#define __USED_FPU 1

#include "stm32f4xx.h"
#include "arm_math.h"
// #include "math_helper.h" //## check usage ???

//#define NUM_FIR_TAPS 56 // ## Linked to Lauer example
#define BLOCKSIZE    256

//Number of 2nd order Biquad stages per filter
#define NUMSTAGES 2

// allocate the buffer signals and the filter coefficients on the heap

//arm_fir_instance_q15 FIR;
//q15_t outSignal[BLOCKSIZE];

arm_biquad_casd_df1_inst_f32 S1;

/* ----------------------------------------------------------------------
** f32 input and output buffers
** ------------------------------------------------------------------- */
float32_t inputf32[BLOCKSIZE];
float32_t outputf32[BLOCKSIZE];
/* ----------------------------------------------------------------------
** Q31 state buffers for biquad cascade
** ------------------------------------------------------------------- */
static float32_t biquadStatef32[4 * NUMSTAGES];

float32_t coeffTable[5*NUMSTAGES] = {
  /* Band 1, -9 dB gain */
		0.914247351285939, -1.828494702571880, 0.914247351285939, 1.824651307057290, -0.832338098086468,
		1.124231319754870, -1.824426994977370, 0.740179934839365, 1.841269711734180, -0.847568537837422};

/* q15_t fir_coeffs_lp[NUM_FIR_TAPS] = { -217,   40,  120,  237,  366,  475,  527,  490,  346,
                                       100, -217, -548, -818, -947, -864, -522,   86,  922,
                                      1904, 2918, 3835, 4529, 4903, 4903, 4529, 3835, 2918,
                                      1904,  922,   86, -522, -864, -947, -818, -548, -217,
                                       100,  346,  490,  527,  475,  366,  237,  120,   40,
                                      -217,    0,    0,    0,    0,    0,    0,    0,    0,
                                         0,    0};  // low pass at 1KHz with 40dB at 1.5KHz for SR=16KHz */

/*q15_t fir_coeffs_hp[NUM_FIR_TAPS] = { -654,  483,  393,  321,  222,   76, -108, -299, -447,
                                      -501, -422, -200,  136,  520,  855, 1032,  953,  558,
                                      -160,-1148,-2290,-3432,-4406,-5060,27477,-5060,-4406,
                                     -3432,-2290,-1148, -160,  558,  953, 1032,  855,  520,
                                       136, -200, -422, -501, -447, -299, -108,   76,  222,
                                       321,  393,  483, -654,    0,    0,    0,    0,    0,
0, 0,}; // high pass at 1.5KHz with 40dB at 1KHz for SR=16KHz */

// q15_t fir_state[NUM_FIR_TAPS + BLOCKSIZE];  //Lauer example

bool firstStart = false;

// the core dsp function
void dsp(int16_t* buffer, int length)
{

	  // we initiate the filter only if needed to prevent clitches at the beginning of new buffers
		if (firstStart == false)
		{
			initFilter();
			firstStart = true;
		}

  	// process with FIR
	//  arm_fir_fast_q15(&FIR, buffer, outSignal, BLOCKSIZE);

  	// copy the result
	//  arm_copy_q15(outSignal, buffer, length);

	/* ----------------------------------------------------------------------
	** Convert block of input data from float to Q31
	** ------------------------------------------------------------------- */
	arm_q15_to_float(buffer, inputf32, BLOCKSIZE);
	/* ----------------------------------------------------------------------
	** Scale down by 1/8.  This provides additional headroom so that the
	** graphic EQ can apply gain.
	** ------------------------------------------------------------------- */
	//arm_scale_q31(inputQ31, 0x7FFFFFFF, -3, inputQ31, BLOCKSIZE);

	/* ----------------------------------------------------------------------
	** Call the Q31 Biquad Cascade DF1 32x64 process function for band1, band2
	** ------------------------------------------------------------------- */
	arm_biquad_cascade_df1_f32(&S1, inputf32, outputf32, BLOCKSIZE);

	/* ----------------------------------------------------------------------
	** Convert Q31 result back to float
	** ------------------------------------------------------------------- */
	arm_float_to_q15(outputf32, buffer, BLOCKSIZE);
	/* ----------------------------------------------------------------------
	** Scale back up
	** ------------------------------------------------------------------- */
	//arm_scale_f32(outputF32 + (i * BLOCKSIZE), 8.0f, outputF32 + (i * BLOCKSIZE), BLOCKSIZE);

}

// we initialize and switch the filter here
void initFilter()
{
  // apply the low pass filter
  //arm_fir_init_q15(&FIR, NUM_FIR_TAPS, fir_coeffs_hp, fir_state, BLOCKSIZE);
  /* Initialize the state and coefficient buffers for all Biquad sections */
	arm_biquad_cascade_df1_init_f32(&S1, NUMSTAGES,
            &coeffTable[0],
            &biquadStatef32[0]);
}
