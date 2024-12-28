#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define PIN        6
#define NUMPIXELS  150
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 350

int LED5V = 2;
int boutonP1 = 6; // Numero du PIN du bouton 1
int boutonP2 = 5; // Numero du PIN du bouton 2
int Buzzer = 4;   // Numero du PIN du Buzzer

int clickP1, clickP2; // Definir le nombre de cliques de chaque joueurs
int tourP1 = 0;
int tourP2 = 0;
int NombreTours = 5;
bool winning = false;
bool start = false;

int MillisTimer = millis();
int oldTimer = 0;
int timer;

int velocityP1, velocityP2;
int oldPosP1, oldPosP2;


uint8_t blank[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

uint8_t custChar[8][8] = {
{
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
},// Small top line - 0

{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
},// Small bottom line - 1


{
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111
},// Small lines top and bottom -2


{
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
}, // -3



{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111
},// Left bottom chamfer full - 4

{
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
},// Right top chamfer full -5


{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100
},// Right bottom chamfer full -6

{
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
},// Left top chamfer full -7
 
};

// Define our numbers 0 thru 9
// 254 is blank and 255 is the "Full Block"
uint8_t bigNums[38][6] = {
  {7, 0, 5, 4, 1, 6}, //0
  {0, 5, 254, 1, 255, 1},     //1
  {0, 0, 5, 4, 2, 2},         //2
  {3, 3, 5, 2, 2, 6},         //3
  {4, 1, 5, 254, 254, 255},   //4
  {255, 3, 3, 2, 2, 6},       //5 
  {7, 3, 3, 4, 2, 6},         //6 
  {0, 0, 5, 254, 7, 254},     //7 
  {7, 3, 5, 4, 2, 6},         //8 
  {7, 3, 5, 254, 254, 6},     //9
  {0, 3, 5, 254, 2, 254},//? 10
  {254, 255, 254, 254, 2, 254},//! 11 
 {7, 3, 5, 255, 254, 255}, //A 12
 {255, 3, 6, 255, 2, 5}, //B 13
 {7, 0, 0, 4, 1, 1}, //C 14
 {255, 0, 5, 255, 1, 6}, //D 15
 {255, 3, 3, 255, 2, 2}, //E 16
 {255, 3, 3, 255, 254, 254}, //F 17
 {7, 0, 0, 4, 1, 5}, //G 18
{255, 1, 255, 255, 254, 255},  //H 19
 {0, 255, 0, 1, 255, 1}, //I 20
 {254, 254, 255,1,1, 6}, //J 21
 {255, 1, 6, 255, 254, 5}, //K 22
 {255, 254, 254, 255, 1, 1}, //L 23
 {5,1,7,255,254,255}, //M 24
 {5,1,255,255,0,4}, //N 25
 {7, 0, 5, 4, 1, 6}, //O 26
 {255, 3, 5, 255, 254, 254}, //P 27
 {7,0,5,4,1,255}, //Q 28
 {255, 3, 5, 255, 254,5}, //R 29
 {7, 3, 3, 2, 2,6}, //S 30
 {0, 255, 0, 254, 255, 254}, //T 31
 {255, 254, 255, 4, 1, 6}, //U 32
 {5, 254, 7, 4, 1, 6}, //V 33
 {5, 1, 7, 4,255, 6}, //W 34
 {4, 1, 6, 7, 254, 5}, //X 35
 {4,1,6,254,255,254}, //Y 36
 {0,3,6,7,2,1}, //Z 37
  
};

struct RGB {
    int r;
    int g;
    int b;
};

RGB playerColor1 = {0, 0, 255};
RGB playerColor2 = {0, 255, 0};

RGB mixColors(const RGB& color1, const RGB& color2) {
    RGB mixedColor;
    mixedColor.r = (color1.r + color2.r) / 2;
    mixedColor.g = (color1.g + color2.g) / 2;
    mixedColor.b = (color1.b + color2.b) / 2;
    return mixedColor;
}

RGB mixedColor = mixColors(playerColor1, playerColor2);

void setup() {
  Serial.begin(9600);

  pinMode(LED5V, OUTPUT);  
  pinMode(9, OUTPUT);
  digitalWrite(LED5V, HIGH);
  digitalWrite(9, HIGH);

  lcd.init();
  lcd.init();
  lcd.backlight();

  pixels.begin();

  pinMode(boutonP1, INPUT_PULLUP);
  pinMode(boutonP2, INPUT_PULLUP);

}

void loop() {
  if (winning == false && start == false) {
    if (!digitalRead(boutonP1)) {
      EcranGo();
    }
  }
  if (winning == false && start == true) {
    if (!digitalRead(boutonP1) && winning == false) {
      velocityP1 += 1;
    }
    if (!digitalRead(boutonP2) && winning == false) {
        velocityP2 += 1;
    }

    if (velocityP1 > 150) { velocityP1 = 0; tourP1 = tourP1 + 1; }
    if (velocityP2 > 150) { velocityP2 = 0; tourP2 = tourP2 + 1; }

    if (tourP1 == NombreTours) { EndGame(); }
    if (tourP2 == NombreTours) { EndGame(); }

    MillisTimer=millis();
    if (MillisTimer - oldTimer > 1000) {
      timer += 1;
      oldTimer = MillisTimer;
    }

    if (timer < 60) {
      AfficherTimer();
      printBigNum(tourP1, 1, 1);
      printBigNum(tourP2, 16, 1);
    } else {
      EndGame();
    }

    pixels.clear();

    pixels.setPixelColor(velocityP1, pixels.Color(playerColor1.r, playerColor1.g, playerColor1.b));
    pixels.setPixelColor(velocityP1 + 1, pixels.Color(playerColor1.r, playerColor1.b, playerColor1.g));

    pixels.setPixelColor(velocityP2, pixels.Color(playerColor2.r, playerColor2.g, playerColor2.b));
    pixels.setPixelColor(velocityP2 + 1, pixels.Color(playerColor2.r, playerColor2.g, playerColor2.b));

    if (velocityP1 == velocityP2) {
      pixels.setPixelColor(velocityP1, pixels.Color(mixedColor.r, mixedColor.g, mixedColor.b));
      pixels.setPixelColor(velocityP1 + 1, pixels.Color(mixedColor.r, mixedColor.g, mixedColor.b));
    } else if (velocityP1 == velocityP2 - 1) {
      pixels.setPixelColor(velocityP1 + 1, pixels.Color(mixedColor.r, mixedColor.g, mixedColor.b));
    } else if (velocityP1 == velocityP2 - 1) {
      pixels.setPixelColor(velocityP1, pixels.Color(mixedColor.r, mixedColor.g, mixedColor.b));
    }

    pixels.show();
  } else {
    rainbowCycle(1);
    delay(5000);
    Reset();
  }

  debug();

  delay(100);
}

void printBigNum(int number, int startCol, int startRow) {
  // Position cursor to requested position (each char takes 3 cols plus a space col)
  lcd.setCursor(startCol, startRow);
  // Each number split over two lines, 3 chars per line. Retrieve character
  // from the main array to make working with it here a bit easier.
  uint8_t thisNumber[6];
  for (int cnt = 0; cnt < 6; cnt++) {
    thisNumber[cnt] = bigNums[number][cnt];
  }
  // First line (top half) of digit
  for (int cnt = 0; cnt < 3; cnt++) {
    lcd.print((char)thisNumber[cnt]);
  }
  // Now position cursor to next line at same start column for digit
  lcd.setCursor(startCol, startRow + 1);
  // 2nd line (bottom half)
  for (int cnt = 3; cnt < 6; cnt++) {
    lcd.print((char)thisNumber[cnt]);
  }
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

void EcranGo()
{
  if (!start) {
    lcd.clear();
    tone(Buzzer,523,1000);
    delay(1000);  
    tone(Buzzer,523,1000);
    delay(1000);
    printBigNum(18, 7, 1);
    printBigNum(26, 11, 1);
    tone(Buzzer,1000,1000); 
    delay(1000);
    start = true;
  } 
}


void EndGame() {
  lcd.clear();
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
