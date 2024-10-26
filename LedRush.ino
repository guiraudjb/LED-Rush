#include <DFMiniMp3.h>

class Mp3Notify; 

typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3; 

DfMp3 dfmp3(Serial1);

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> 
#endif

#define PIN        3 // Numéro du PIn pour les LEDS
#define NUMPIXELS 150 // Popular NeoPixel ring size
int BRIGHTNESS = 10;

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#include <TM1637Display.h>

#define CLK 6
#define DIO 7

TM1637Display display = TM1637Display(CLK, DIO);

const uint8_t win[] = {
  SEG_F | SEG_E | SEG_D | SEG_C,
  SEG_B | SEG_C | SEG_D | SEG_E,
  SEG_B | SEG_C,
  SEG_E | SEG_G | SEG_C
};

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

class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
  {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  

    // start next track
    track += 1;
    // this example will just start back over with 1 after track 3
    if (track > 3) 
    {
      track = 1;
    }
    dfmp3.playMp3FolderTrack(track);  // sd:/mp3/0001.mp3, sd:/mp3/0002.mp3, sd:/mp3/0003.mp3
  }
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

void setup() {
  Serial.begin(9600);

  pinMode(LED5V, OUTPUT);  
  pinMode(9, OUTPUT);
  digitalWrite(LED5V, HIGH);
  digitalWrite(9, HIGH);

  display.setBrightness(5);
  display.showNumberDec(02, false, 2, 2);  
  display.showNumberDec(01, false, 2, 0);   

  dfmp3.begin();
  uint16_t volume = dfmp3.getVolume();
  dfmp3.setVolume(50);
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.playMp3FolderTrack(1);

  pinMode(boutonP1, INPUT_PULLUP);
  pinMode(boutonP2, INPUT_PULLUP);

  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
}

void loop() {
  if (!digitalRead(boutonP1) && winning == false) {
    velocityP1 += 1;
    //Temporisation();
  } else if (winning == true) {
    Reset();
  }
  if (!digitalRead(boutonP2) && winning == false) {
    if (clickP2 >= 3) {
      clickP2 = 0;
      velocityP2 += 3;
    }
    //Temporisation();
    clickP2 += 1;
  }

  if (velocityP1 > 150) { velocityP1 = 0; tourP1 = tourP1 + 1; }
  if (velocityP2 > 150) { velocityP2 = 0; tourP2 = tourP2 + 1; }

  if (tourP1 == NombreTours) { Win(1); }
  if (tourP2 == NombreTours) { Win(2); }

  MillisTimer=millis();
  if (MillisTimer - oldTimer > 1000) {
    /*if (velocityP1 > 1) {
      velocityP1 -= 1;
    }*/
    timer += 1;
    oldTimer = MillisTimer;
  }
  else if (MillisTimer > 250) {
    oldPosP1 = velocityP1;
    oldPosP2 = velocityP2;
  }

  if (winning == false) {
    display.showNumberDec(tourP1, false, 2, 0);
    display.showNumberDec(tourP2, false, 2, 2);

    pixels.clear();

    pixels.setPixelColor(oldPosP1 + velocityP1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));
    pixels.setPixelColor(oldPosP1 + velocityP1 + 1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));

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
void Temporisation(){
  int statusLeftButton = digitalRead(boutonP1);
  int statusRightButton = digitalRead(boutonP2);
  for (int T = 1; T <= 1000 ; T++){
    while(statusLeftButton==LOW || statusRightButton==LOW){
      delay(125);
    }
  }
}

void Win(int player) {
  winning = true;
  tourP1 = 0;
  tourP2 = 0;
  dfmp3.setVolume(50);
  dfmp3.playMp3FolderTrack(2);
  display.setSegments(win);
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
  display.showNumberDec(0, false);
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

void State() {
  if (!digitalRead(10)) {
    pixels.setBrightness(0);
    pixels.show();
  } 
  else if (digitalRead(10)) {
    pixels.setBrightness(50);
    pixels.show();
  }
}
