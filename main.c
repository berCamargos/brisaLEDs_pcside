/***
  This file is part of PulseAudio.
  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.
  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
***/
#include "configs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#include "udp_client.h"
#include "heatmap.h"
#include "run_fft.h"
#include "led_map.h"

static double values[N_LEDS];
static double out[BUFSIZE];

void fix_led(uint8_t *leds)
{
    /* This is probabelly due to a bug on ESP8266 code */
    uint8_t temp_led;
    temp_led = leds[0];
    leds[0] = leds[1];
    leds[1] = temp_led;
}

uint16_t encode(uint8_t *leds, uint8_t *msg)
{
    uint16_t msg_idx = 0;
    for(uint16_t led_buf_idx = 0; led_buf_idx < N_LEDS*3; led_buf_idx += 1)
    {
        if ((leds[led_buf_idx] == SCAPE_CHAR) || (leds[led_buf_idx] == END_CHAR))
        {
            msg[msg_idx++] = SCAPE_CHAR;
        }
        msg[msg_idx++] = leds[led_buf_idx];
    }
    msg[msg_idx++] = END_CHAR;
    return msg_idx;
}

int main(int argc, char*argv[]) {
    /* The sample type to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S16LE,
        .rate = 44100,
        .channels = 2
    };
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    int16_t buf[BUFSIZE*2];
    uint8_t leds[N_LEDS*3];
    uint8_t msg[N_LEDS*3*2];
    memset(values, 0, N_LEDS*sizeof(double));
    connect_udp(argv[1], atoi(argv[2]));
    printf("%s, %d\n", argv[1], atoi(argv[2]));
    /* get a message from the user */

    init_fft();
    init_led_map();
    init_heatmap();

    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    for (;;) {
        /* Record some data ... */
        if (pa_simple_read(s, (uint8_t *)buf, sizeof(buf), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
        run_fft(buf, out);
        run_led_map(out, values);
        run_heatmap(values, leds);
        for(uint8_t led_idx = 0; led_idx < N_LEDS; led_idx += 1)
        {
            fix_led(&leds[3*led_idx]);
        }
        uint16_t msg_len = encode(leds, msg);
        send_udp(msg, msg_len);
    }
    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
    return ret;
}
