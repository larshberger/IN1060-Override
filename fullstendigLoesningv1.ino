#include <FastLED.h>
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

  if(lysVerdi < terskel){

    if (indeks == 0){       
      startTid = millis();
      forrigeOppdateringTid = millis();

      skruPaaLys(indeks);
      skruPaaLys(ant_leds-1-indeks);
      indeks++;
    }
    
    unsigned long p = forrigeOppdateringTid + forsinkelse;
    if(millis() > p){        
      skruPaaLys(indeks);
      skruPaaLys(ant_leds-1-indeks);
      indeks++;
      forrigeOppdateringTid = millis();
    }

    else if(millis() > startTid + animasjonsTid){     
      blinkRoedt();
    }
  }

  else{
    lysAv();
    indeks = 0;
  }
}

void skruPaaLys(int indeks){
  leds[indeks] = farge;
  FastLED.show();
}

void lysAv(){
  FastLED.clear();
  leds[0] = CRGB::White;
  leds[14] = CRGB::White;
  FastLED.show();
}

void blinkRoedt(){
  for (int n = 0; n < ant_leds; n++) {leds[n] = CRGB::Red;}
  FastLED.show();
  delay(200);

  FastLED.clear();
  FastLED.show();
  delay(800);
}
