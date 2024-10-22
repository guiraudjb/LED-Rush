#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif

#define PIN        5 // Numéro du PIn pour les LEDS
#define NUMPIXELS 25 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int boutonP1 = 10; // Numero du PIN du bouton 1
int boutonP2 = 11; // Numero du PIN du bouton 2
int boutonP3 = 12; // Numero du PIN du bouton 3
int boutonP4 = 13; // Numero du PIN du bouton 4

int clickP1, clickP2, clickP3, clickP4; // Definir le nombre de cliques de chaque joueurs

int forwardP1 = 0;

int playerColors[4][3] = {
  {0, 255, 255},
  {0, 255, 0},
  {255, 255, 0},
  {255, 0, 255},
};

void setup() {
  pinMode(boutonP1, INPUT_PULLUP);
  Serial.begin(9600);
  pixels.begin();
}

void loop() {
  Serial.println(forwardP1);
  if (!digitalRead(boutonP1)) {
    pixels.clear();
    clickP1 += 1;
    if (clickP1 >= 5) {
      clickP1 = 0;
      forwardP1 += 1;
    }
  }
  if (forwardP1 > 24) {
    forwardP1 = 0;
  }
  pixels.setPixelColor(forwardP1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));
  pixels.setPixelColor(forwardP1 - 1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));

  pixels.setPixelColor(forwardP1 + 2, pixels.Color(playerColors[1][0], playerColors[1][1], playerColors[1][2]));
  pixels.setPixelColor(forwardP1 + 3, pixels.Color(playerColors[1][0], playerColors[1][1], playerColors[1][2]));

  pixels.setPixelColor(forwardP1 + 5, pixels.Color(playerColors[2][0], playerColors[2][1], playerColors[2][2]));
  pixels.setPixelColor(forwardP1 + 6, pixels.Color(playerColors[2][0], playerColors[2][1], playerColors[2][2]));

  pixels.setPixelColor(forwardP1 + 8, pixels.Color(playerColors[3][0], playerColors[3][1], playerColors[3][2]));
  pixels.setPixelColor(forwardP1 + 9, pixels.Color(playerColors[3][0], playerColors[3][1], playerColors[3][2]));

  pixels.show();
}
