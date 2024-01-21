# Remote-Controlled-RGB-LED
RGB Led controll with remote controller

## Components and Schematic Drawings

### - Arduino UNO R3
### - Phototransistor (BPW17N):
![photot_trans](https://user-images.githubusercontent.com/102668658/204975980-66391020-5431-43ea-bccb-4f112810191b.png)
### - RGB LED:
![rgb_led](https://user-images.githubusercontent.com/102668658/204975955-276201ef-b1eb-4de9-a65c-d536cec97a6c.png)

## Requirements
1. Use the red button on the TV remote control to adjust the red channel of the LED.
2. Use the green button on the TV remote control to adjust the green channel of the LED.
3. Use the blue button on the TV remote control to adjust the blue channel of the LED.
4. The brightness of the selected colour channel can be adjusted by using the channel scroll buttons (CH+ and CH-).
5. Change the brightness of the selected colour channel in steps of 10 on a scale of 0-250.
(The 251-255 range cannot be used. 0: the channel is not illuminated. 250: the
channel is lit at the highest brightness used).
6. When changing the strength of the selected colour channel, overflow and underflow
is not allowed. Once the limits are reached, further strengthening or weakening of the channel
channel will be ineffective.
7. After pressing the [SAVE_BUTTON] button, the adjustment of channel strength is suspended.
8. After pressing the [SAVE_BUTTON] button, the SW will receive a press of button 1, 2 or 3
and stores the current setting of the RGB LED in EEPROM.
9. Press the [SAVE_BUTTON] button followed by any other button (except [SAVE_BUTTON], the
[LOAD_BUTTON] and the ON/OFF button), the channel adjustability is reactivated.
10. After pressing the [LOAD_BUTTON] button, you can not adjust the strength of the channels.
11. After pressing the [LOAD_BUTTON] button, the SW will receive a press of button 1, 2 or 3
and reads and displays the saved setting of the RGB LED from the EEPROM. 
12. Press the [LOAD_BUTTON] button followed by any other button (except [SAVE_BUTTON], the
[LOAD_BUTTON] and the ON/OFF button), the channel adjustability is reactivated.
13. Pressing the [LOAD_BUTTON] button and [SAVE_BUTTON] in succession always results in the later button.
14. Press the ON/OFF button to turn the RGB LED on and off.
15. After switching on the RGB LED with the ON/OFF button, the brightness of the channels
adjustment will be active.
16. After a reset, the SW will start with the saved LED setting for button 1, and the saved
setting is displayed on the LED.
17. Data stored in EEPROM must be protected.
18. If SW detects data corruption after reset, it overwrites the corrupted data with the default settings and
it also takes care of data protection.
19. Default value of the three saved settings:

|Button| R | G | B |
|------|---|---|---|
|  1   |200| 0 | 0 |
|  2   | 0 |200| 0 |
|  3   | 0 | 0 |200|

## State-Transition Graph
![graf](https://user-images.githubusercontent.com/102668658/204976744-3ed193a8-f176-4de3-b58b-69303927abc9.jpg)

## Demo Video
https://user-images.githubusercontent.com/102668658/204981867-01642a2f-8e14-48aa-876e-f65e64095594.mp4



