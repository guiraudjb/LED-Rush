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

int LED5V = 2;
int boutonP1 = 4; // Numero du PIN du bouton 1
int boutonP2 = 5; // Numero du PIN du bouton 2

int clickP1, clickP2; // Definir le nombre de cliques de chaque joueurs
int tourP1, tourP2;

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
  digitalWrite(LED5V, HIGH);

  dfmp3.begin();
  uint16_t volume = dfmp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.setVolume(50);
  dfmp3.playMp3FolderTrack(1);

  pinMode(boutonP1, INPUT_PULLUP);
  pinMode(boutonP2, INPUT_PULLUP);

  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
}

void loop() {
  if (!digitalRead(boutonP1)) {
    velocityP1 += 1;
    //Temporisation();
  }
  else if (!digitalRead(boutonP2)) {
    if (clickP2 >= 3) {
      clickP2 = 0;
      velocityP2 += 1;
    }
    //Temporisation();
    clickP2 += 1;
  }

  if (velocityP1 > 150) { velocityP1 = 0; tourP2 = tourP1 + 1; }
  if (velocityP2 > 150) { velocityP2 = 0; tourP2 = tourP2 + 1; }

  MillisTimer=millis();
  if (MillisTimer - oldTimer > 1000) {
    if (velocityP1 > 1) {
      velocityP1 -= 1;
    }
    timer += 1;
    oldTimer = MillisTimer;
  }
  else if (MillisTimer > 250) {
    oldPosP1 = velocityP1;
    oldPosP2 = velocityP2;
  }

  pixels.clear();

  pixels.setPixelColor(oldPosP1 + velocityP1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));
  pixels.setPixelColor(oldPosP1 + velocityP1 + 1, pixels.Color(playerColors[0][0], playerColors[0][1], playerColors[0][2]));

  pixels.setPixelColor(velocityP2, pixels.Color(playerColors[1][0], playerColors[1][1], playerColors[1][2]));
  pixels.setPixelColor(velocityP2 + 1, pixels.Color(playerColors[1][0], playerColors[1][1], playerColors[1][2]));

  pixels.show();

  debug();

  delay(250);
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

void debug() {
  Serial.print("Bouton P1: ");
  Serial.print(digitalRead(boutonP1));
  Serial.print(" - Bouton P2: ");
  Serial.print(digitalRead(boutonP2));
  Serial.print(" - Acceleration: ");
  Serial.print(oldPosP1 + velocityP1);
  Serial.println("");
}
