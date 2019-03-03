#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include "heatmap.h"
#include "configs.h"

static void HSLToRGB(double val, uint8_t *leds);
static void simple_7_colors(double val, uint8_t *leds);
static void convert_to_rgb(double val, uint8_t *leds);

void init_heatmap()
{
    return;
}

void run_heatmap(double *values, uint8_t *leds)
{
    for(uint8_t led_idx = 0; led_idx < N_LEDS; led_idx += 1)
    {
#if HEATMAP_7_COLORS
        simple_7_colors(values[led_idx], &leds[3*led_idx]);
#elif HEATMAP_3_COLORS
        convert_to_rgb(values[led_idx], &leds[3*led_idx]);
#endif
    }
}

/* A simple routine calling UNIX write() in a loop */
float HueToRGB(float v1, float v2, float vH)
{
	if (vH < 0)
		vH += 1;

	if (vH > 1)
		vH -= 1;

	if ((6 * vH) < 1)
		return (v1 + (v2 - v1) * 6 * vH);

	if ((2 * vH) < 1)
		return v2;

	if ((3 * vH) < 2)
		return (v1 + (v2 - v1) * ((2.0f / 3) - vH) * 6);

	return v1;
}

void HSLToRGB(double val, uint8_t *leds) {
    float H = (1 - val) * 100;
    float S = 100;
    float L = val * 50;

	if (S == 0)
	{
		leds[0] = leds[1] = leds[2] = (unsigned char)(L * 255);
	}
	else
	{
		float v1, v2;
		float hue = (float)H / 360;

		v2 = (L < 0.5) ? (L * (1 + S)) : ((L + S) - (L * S));
		v1 = 2 * L - v2;

        if (val > -0.95)
        {
		    leds[1] = (unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
		    leds[0] = (unsigned char)(255 * HueToRGB(v1, v2, hue));
		    leds[2] = (unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));
        }
        else
        {
		    leds[0] = (uint8_t)(255*val);(unsigned char)(255 * HueToRGB(v1, v2, hue + (1.0f / 3)));
		    leds[1] = 0;//(unsigned char)(255 * HueToRGB(v1, v2, hue));
		    leds[2] = 0;//(unsigned char)(255 * HueToRGB(v1, v2, hue - (1.0f / 3)));

        }
	}
}

void simple_7_colors(double val, uint8_t *leds)
{
    /* 
        Based on the image from http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients (http://www.andrewnoske.com/w/images/b/bf/Heatmap_gradient.png)
    */

    // When putting in different files this should be defines
    double max_val = 1;
    uint8_t N_COLORS = 7;
    // Binary representation of each color (red = 4, green = 2, blue = 1)
    uint8_t colors[] = {0, 4, 6, 2, 3, 1, 7};
    double step = (max_val/(N_COLORS-1));
    /*Get the base color it belongs*/
    uint8_t base = (uint8_t)floor(val/step);
    if (base == (N_COLORS-1))
    {
        base -= 1;
    }
    /*Get the variation between this base and the next*/
    double step_diff = (val - (base)*step)/step;

    uint8_t rgb_base = 1;
    for(uint8_t rgb_index = 0; rgb_index < 3; rgb_index += 1)
    {
        leds[rgb_index] = 255*(colors[base]&rgb_base)/rgb_base + 255*step_diff*(((colors[base + 1]&rgb_base) - (colors[base]&rgb_base))/rgb_base);
        rgb_base *= 2;
    }
}

void convert_to_rgb(double val, uint8_t *leds)
{
    double i_f = val*2;
    int i = floor(i_f);
    double f = i_f - i;
    memset(leds, 0, 3);
    if (f < 0.0000001)
    {
        leds[2-i] = 255;
    }
    else
    {
        leds[1-i] = 255*f;
        leds[2-i] = 255*(1-f);
    }
}


