#include <FastLED.h>

//Definering og initialisering av diverse makroer og variabler
#define farge   CRGB::Purple
#define led_port  8
#define ant_leds  15
#define sensor A1
#define terskel 40
#define animasjonsTid  600000

float forsinkelse = animasjonsTid/(ant_leds/2);
unsigned long startTid;
unsigned long forrigeOppdateringTid;
int indeks = 0;

CRGB leds[ant_leds];

void setup() {
  pinMode(sensor, INPUT);
  FastLED.addLeds<NEOPIXEL, led_port>(leds, ant_leds); 
}

void loop() {
  int lysVerdi = analogRead(sensor);

  //Vår på/av. Dersom fototransistoren leser en verdi under en forutsatt terskel starter lysene
  if(lysVerdi < terskel){

    //Vi har litt ekstra kode for når lysene starter opp
    if (indeks == 0){       
      startTid = millis();
      forrigeOppdateringTid = millis();

      skruPaaLys(indeks);
      skruPaaLys(ant_leds-1-indeks);
      indeks++;
    }
    
    //Hvis riktig tid har gått siden forrige lys ble aktivert, skrur vi på neste lys
    unsigned long p = forrigeOppdateringTid + forsinkelse;
    if(millis() > p){        
      skruPaaLys(indeks);
      skruPaaLys(ant_leds-1-indeks);
      indeks++;
      forrigeOppdateringTid = millis();
    }

    //Hvis riktig tid har gått siden flasken ble satt på blinker vi rødt.
    else if(millis() > startTid + animasjonsTid){     
      blinkRoedt();
    }
  }

  //Dersom fototransistoren leser for høy verdi skrus lysene av og indeksen nullstilles.
  else{
    lysAv();
    indeks = 0;
  }
}

//Denne metoden skrur på en led hvems indeks vi tar som parameter i metodekallet
void skruPaaLys(int indeks){
  leds[indeks] = farge;
  FastLED.show();
}

//Denne metoden skrur av alle lysene UNNTATT det første og siste lyset. 
//Disse skal lyse hele tiden for at fototransistoren skal kunne registrere en verdi selv i mørke rom
void lysAv(){
  FastLED.clear();
  leds[0] = CRGB::White;
  leds[14] = CRGB::White;
  FastLED.show();
}

//Denne metoden blinker alle lysene rødt. 
void blinkRoedt(){
  for (int n = 0; n < ant_leds; n++) {leds[n] = CRGB::Red;}
  FastLED.show();
  delay(200);

  FastLED.clear();
  FastLED.show();
  delay(800);
}
