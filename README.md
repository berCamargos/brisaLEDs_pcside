# Brisa LEDs - PC Side

This is the first version of brisa LEDs made and mainteined by Bernardo Camargois (ber.camargos@gmail.com).

This application read audio inputs from PulseAudio, runs a FFT and sends it to a ESP through UDP.

It really need some commentary and mor funciontios, but it is a first working version


## Build

To build simply run `gcc parec-simple.c udp_client.c heatmap.c run_fft.c led_map.c -lpulse -lpulse-simple -lfftw3 -lm -o parec-simpl`

## Running

To run execute from terminal `./parec-simple 192.168.1.168 2345` where 192.168.1.168 is your ESP8266 IP

You can also use the LED_Emulator writen in python, if this is the case, simply run it and set the IP to 127.0.0.1
