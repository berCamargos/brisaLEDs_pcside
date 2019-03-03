import socket
import pygame
import sys


def update_leds(screen, leds, N_LEDS, start_pos = (10,10), led_size = (10,10), led_dist = (1,0)):
    for led_idx in range(N_LEDS):
        try:
            color = (leds[led_idx*3 + 1], leds[led_idx*3], leds[led_idx*3 + 2])
        except Exception as e:
            print(e)
            break
        pygame.draw.rect(screen, color, (start_pos[0] + (led_size[0] + led_dist[0])*led_idx, start_pos[1], led_size[0], led_size[1]), 0)
    pygame.display.update()



UDP_IP = "127.0.0.1"
UDP_PORT = 2345

pygame.init()
screen = pygame.display.set_mode((640,100))
screen.fill((0,0,0))
start_pos = (10,10)
led_size = (10,10)
led_dist = (1,0)
N_LEDS = 30
END_CHAR = 223
SCAPE_CHAR = 222

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

leds = []
while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    scaped = False
    for d in data:
        if scaped:
            leds.append(d)
        elif d == END_CHAR:
            update_leds(screen, leds, N_LEDS)
            leds = []
        elif d == SCAPE_CHAR:
            scaped = True
            continue
        else:
            leds.append(d)



