# LED-Rush
LED Rush game for Arduino UNO, with LED and Buttons

## Dependencies

```h
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif
```

```h
#include <DFMiniMp3.h>

class Mp3Notify; 

typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 

DfMp3 dfmp3(Serial1);
```

## Small info about the code

You can change the colors of players here

```ino
int playerColors[4][3] = {
  {0, 255, 255},            // Cyan for Player1
  {0, 255, 0},              // Green for Player2
  {255, 255, 0},            // Yellow for Player3
  {255, 0, 255},            // Purple for Player4
};
```
