#include <Wire.h>
#include <Lycidcrystal_I2C.h>


int LED5V = 2;
int boutonP1 = 4; // Numero du PIN du bouton 1
int boutonP2 = 5; // Numero du PIN du bouton 2

int clickP1, clickP2; // Definir le nombre de cliques de chaque joueurs
int tourP1, tourP2;
int NombreTours = 5;
bool winning = false;

int MillisTimer = millis();
int oldTimer = 0;
int timer;

int velocityP1, velocityP2;
int oldPosP1, oldPosP2;

int playerColors[2][3] = {
  {0, 0, 255},
  {0, 255, 0}
};

void setup() {
  Serial.begin(9600);

  pinMode(LED5V, OUTPUT);  
  pinMode(9, OUTPUT);
  digitalWrite(LED5V, HIGH);
  digitalWrite(9, HIGH);

  lcd.init();
  lcd.init();
  lcd.setBri

  pinMode(boutonP1, INPUT_PULLUP);
  pinMode(boutonP2, INPUT_PULLUP);

}

void loop() {
  if (!digitalRead(boutonP1) && winning == false) {
    velocityP1 += 1;
  } else if (winning == true) {
    Reset();
  }
  if (!digitalRead(boutonP2) && winning == false) {
    if (clickP2 >= 3) {
      clickP2 = 0;
      velocityP2 += 3;
    }
    clickP2 += 1;
  }

  if (velocityP1 > 150) { velocityP1 = 0; tourP1 = tourP1 + 1; }
  if (velocityP2 > 150) { velocityP2 = 0; tourP2 = tourP2 + 1; }

  if (tourP1 == NombreTours) { EndGame(); }
  if (tourP2 == NombreTours) { EndGame(); }

  MillisTimer=millis();
  if (MillisTimer - oldTimer > 1000) {
    /*if (velocityP1 > 1) {
      velocityP1 -= 1;
    }*/
    timer += 1;
    oldTimer = MillisTimer;
  }

  if (winning == false) {
    AfficherTimer();
    printBigNum(tourP1, 1, 1);
    printBigNum(tourP2, 16, 1);

    pixels.clear();

    pixels.setPixelColor(velocityP1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));
    pixels.setPixelColor(velocityP1 + 1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));

    pixels.setPixelColor(velocityP2, pixels.Color(playerColors[1][0], playerColors[1][1], playerColors[1][2]));
    pixels.setPixelColor(velocityP2 + 1, pixels.Color(playerColors[1][0], playerColors[1][1], playerColors[1][2]));

    pixels.show();
  } else {
    rainbowCycle(1);
    delay(5000);
    Reset();
  }

  debug();

  delay(100);
}

void AfficherTimer(){
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("Temps:");
  String scorestring = String(timer);
  if (scorestring.length()==3){
    String centaine = String(scorestring.charAt(0));
    printBigNum(centaine.toInt(), 5, 1);
    String decimale = String(scorestring.charAt(1));
    printBigNum(decimale.toInt(), 8, 1);
    String unite = String(scorestring.charAt(2));
    printBigNum(unite.toInt(), 11, 1);
  }
  else if (scorestring.length()==2){
    String decimale = String(scorestring.charAt(0));
    printBigNum(decimale.toInt(), 7, 1);
    String unite = String(scorestring.charAt(1));
    printBigNum(unite.toInt(), 10, 1);
  }
  else{
    String unite = String(scorestring.charAt(0));
    printBigNum(0, 8, 1);
    printBigNum(unite.toInt(), 11, 1);
  }
  
}

void EndGame() {
  winning = true;
  tourP1 = 0;
  tourP2 = 0;
}

void debug() {
  Serial.print("Bouton P1: ");
  Serial.print(digitalRead(boutonP1));
  Serial.print(" - Bouton P2: ");
  Serial.print(digitalRead(boutonP2));
  Serial.print(" - Acceleration: ");
  Serial.print(oldPosP1 + velocityP1);
  Serial.println("");
}

void Reset() {
  pixels.clear();
  pixels.show();
  winning = false;
  tourP1 = 0;
  tourP2 = 0;
  velocityP1 = 0;
  velocityP2 = 0;
  clickP1 = 0;
  clickP2 = 0;
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}


uint32_t Wheel(byte WheelPos) {
  State();
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
