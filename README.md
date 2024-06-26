# IN1060-Override
_Vi er en gruppe på 5 studenter som tok emnet IN1060 ved UiO våren 2024_

Svært kort forklart er det vi har skapt en "dockingstasjon" for vannflaskene til gamere. Denne er utstyrt med en led-stripe for å kommunisere med brukeren og en fototransistor for å registrere hvorvidt flasken er i flaskeholderen. Poenget er at lysene skal minne brukeren på å huske å drikke vann.

Her har vi skrevet en forklaring av hele koden :) Koden er delt opp for å gjøre forklaringen enklere. Den fullstendige koden helt fri for kommentarer finnes i repo'en og heter _"fullstendigLoesningv1.ino"_.

### Definering og initalisering 
Helt i starten i koden setter vi opp diverse makroer og variabler, samt inkluderer biblioteket "FastLed" som brukes til å drive ledstripen. Alle makroene / variablene har beskrivende navn, så jeg kommer ikke til å gå inn på hva hver enkelt er. Den eneste som kan være vært å nevne er "terskel" dette er verdien som sensoren må synke under for at "systemet" skal starte. Bruker fortalte oss også at standard lysstyrke (255) var litt sterkt, så vi definerer en lavere lysstyrke.
```c++
#include <FastLED.h>
#define farge   CRGB::Purple
#define led_port  8
#define ant_leds  15
#define lysstyrke  150
#define sensor A1
#define terskel 40
#define animasjonsTid  600000

float forsinkelse = animasjonsTid/(ant_leds/2);
unsigned long startTid;
unsigned long forrigeOppdateringTid;
int indeks = 0;

CRGB leds[ant_leds];
```
### Egenskrevede metoder for bruk av led-stripen
For å gjøre loop()-funksjonen kortere, ryddigere og enklere har vi skrevet tre metoder. Alle metodene er ganske simple, men også veldig nyttige.

Metoden _skruPaaLys(int indeks)_ tar indeksen til en enkelt led-diode på led-stripen som parameter og setter den til fargen som vi definerte i starten av programmet

Metoden _lysAv()_ gjør nesten akkurat det navnet tilsier: den skrur av lys. Den skrur av alle lysene unntatt det første og siste lyset i stripen. Disse settes til en hvit farge, slik at sensoren har en verdi å lese selv i mørkbelagte rom.  

Metoden _blinkRoedt()_ sier seg selv: den blinker alle lysene i stripen rødt. Denne metoden bruker delay(), og ikke millis() da metoden kun brukes på et sted hvor dette ikke kommer i konflikt med lesing av sensorverdien.
```c++
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

```
### setup() og loop()
Som man kan se er setup()-funksjonen ganske kort. Alt vi gjør er å sette opp pinen til sensoren og setter opp led-stripen.

<sub>For å forklare hva som skjer i loop()-funksjonen har vi utelatt litt av koden. Forklaring av denne delen finnes litt lengere ned. </sub>

Enkelt forklart fungerer loopen vår slik: Vi kontinuerlig leser verdien til sensoren, og kjører kode utifra om den er over eller under den definerte terskelen. Hvis den er under terskelen starter systemet, og er den over så stopper det. 
```c++
void setup() {
  pinMode(sensor, INPUT);
  FastLED.addLeds<NEOPIXEL, led_port>(leds, ant_leds);
  FastLED.setBrightness(lysstyrke); 
}

void loop() {
  int lysVerdi = analogRead(sensor);

  if(lysVerdi < terskel){
    kode kode kode ... 
  }
  else{
    kode kode kode ...
  }
}
```
### if(flasken er satt ned){
Det er i denne if-setningen hvor mesteparten skjer. If-setningen oppfylles som nevnt når verdien på fototransistoren er under terskelen (flasken er satt ned). Koden i if-setningen består av en if-setning og en if-elseif-setning. 

Den første if-setningen oppfylles kun en gang, og det er når flasken først settes ned. Denne aktiverer de første lysene og setter noen millis() variabler som vi trenger for å aktivere de neste lysene på riktig tidspunkt.

Den neste if-setningen har som hensikt å aktivere neste lys på riktig tidspunkt. Den sjekker om tiden som har gått er større enn hva tiden var ved aktivering av forrige lys + forsinkelsen mellom aktivering. Koden skrur på lysene og oppdaterer variablene slik at de er korrekt for neste loop.

Den siste else-if-setningen er svært simpel. Den oppfylles når den tiden hele animasjonen skal ta har gått siden vi startet. Alt som skjer når dette oppfylles er at vi kaller på blinkRoedt()-metoden vi forklarte tidligere. 

```c++
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
```

### else(flasken er ikke satt ned / er løftet opp){
Else-setningen vil oppfylles når sensoren leser en verdi over terskelen. Dette oppstår når fototransistoren ikke er tildekket og registrerer for mye lys. Dette er den avsluttende delen av koden, og alt den gjør er å kalle på lysAv()-metoden (forklart lengere opp) og nullstille den eneste nødvendige variablen. Man vil da kunne sette flasken i dockingsstasjonen på nytt uten å måtte starte programmet helt på nytt. 
```c++
else{
    lysAv();
    indeks = 0;
  }
```
