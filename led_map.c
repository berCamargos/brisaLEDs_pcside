#include "configs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#include "led_map.h"

#define FREQ_DIVIDE 4
#define LED_DECAY   0.80

static void to_led_values_normalized_fixed_max_mean(double *out, double *values, int freq_divide);
static void to_led_values_normalized_mean_decay(double *out, double *values, int freq_divide, double keep);
static void to_led_values_normalized_mean(double *out, double *values, int freq_divide);

void init_led_map()
{
    return;
}

void run_led_map(double *out, double *values)
{
#if LED_MAP_DECAY
    to_led_values_normalized_mean_decay(out, values, FREQ_DIVIDE, LED_DECAY);
#elif LED_MAP_NORMALIZED
    to_led_values_normalized_mean(out, values, FREQ_DIVIDE, LED_DECAY);
#elif LED_MAP_FIXED_MAX
    to_led_values_normalized_fixed_max_mean(out, values, FREQ_DIVIDE, LED_DECAY);
#else
    printf("LED map not defined");
#endif
}

void to_led_values_normalized_fixed_max_mean(double *out, double *values, int freq_divide)
{
    uint16_t step = floor(((float)BUFSIZE/(2*freq_divide))/(float)(N_LEDS));
    uint16_t top;
    double max = 100000;
    double min = 0;
    double val;
    for(uint8_t led_idx = 0; led_idx < (N_LEDS); led_idx += 1)
    {
        top = step*(led_idx + 1);
        val = 0;
        for (uint16_t out_idx = led_idx*step; out_idx < top; out_idx += 1)
        {
            val += fabs(out[out_idx]);
        }
        val /= (top - led_idx*step);
        if (val > max)
        {
            val = max;
        }
        if (val < min)
        {
            val = min;
        }
        values[led_idx] = val;
    }
    if ((max-min) != 0)
    {
        for(uint8_t led_idx = 0; led_idx < N_LEDS; led_idx += 1)
        {
            values[led_idx] = (values[led_idx] - min)/(max - min);
        }
    }
}


void to_led_values_normalized_mean_decay(double *out, double *values, int freq_divide, double keep)
{
    uint16_t step = floor(((float)BUFSIZE/(2*freq_divide))/(float)(N_LEDS));
    uint16_t top;
    double max = 0;
    double min = 1000000000000;
    double val;
    double vals[N_LEDS];
    for(uint8_t led_idx = 0; led_idx < (N_LEDS); led_idx += 1)
    {
        top = step*(led_idx + 1);
        val = 0;
        for (uint16_t out_idx = led_idx*step; out_idx < top; out_idx += 1)
        {
            val += fabs(out[out_idx]);
        }
        val /= (top - led_idx*step);
        if (val > max)
        {
            max = val;
        }
        if (val < min)
        {
            min = val;
        }
        vals[led_idx] = val;
    }

    for(uint8_t led_idx = 0; led_idx < N_LEDS; led_idx += 1)
    {
        if ((max-min) != 0)
        {
            vals[led_idx] = (vals[led_idx] - min)/(max - min);
        }
        else
        {
            vals[led_idx] = 0;
        }
        values[led_idx] = (1-keep)*vals[led_idx] + keep*values[led_idx];
    }
}

void to_led_values_normalized_mean(double *out, double *values, int freq_divide)
{
    to_led_values_normalized_mean_decay(out, values, freq_divide, 0);
}


