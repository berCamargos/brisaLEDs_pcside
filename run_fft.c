#include <fftw3.h>
#include "configs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "run_fft.h"

static double in_d[BUFSIZE];
static fftw_complex out_c[BUFSIZE];
static fftw_plan fftw_p = NULL; 

static void get_frequencies_fftwr(int16_t *in, double *out);
static void get_frequencies_fake(uint16_t *in, double *out);

void init_fft()
{
#if RUN_FAKE_FFT
    return;
#else
    fftw_p = fftw_plan_dft_r2c_1d(BUFSIZE, in_d, out_c, FFTW_ESTIMATE);
#endif
}

void run_fft(int16_t *in, double *out)
{
#if RUN_FAKE_FFT
    get_frequencies_fake(in, out);
#else
    get_frequencies_fftwr(in, out);
#endif
}

void get_frequencies_fftwr(int16_t *in, double *out)
{
    for (uint16_t i = 0; i < BUFSIZE; i+= 1)
    {
        in_d[i] = (double)in[i*2];
    }
    fftw_execute(fftw_p);
    for (uint16_t i = 0; i < BUFSIZE; i += 1)
    {
        out[i] = ((double *)(out_c[i]))[0];
    }
    out[0] = 0;
}

void get_frequencies_fake(uint16_t *in, double *out)
{
    for (uint16_t i = 0; i < BUFSIZE; i += 1)
    {
        out[i] = (float)i/(float)BUFSIZE;
    }
}


