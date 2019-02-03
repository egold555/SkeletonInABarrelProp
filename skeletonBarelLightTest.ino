#include <FastLED.h>
#include "AudioPlayer.h"
#include "Bounce2.h"
#include "Ticker.h"

#define NUM_LEDS 50

#define PIN_TRIGGER_BUTTON 4
#define PIN_LEDS 6
#define PIN_RELAY_CYLINDER_MAN 8
#define PIN_AUDIO_RX 10
#define PIN_AUDIO_TX 12

#define AUDIO_BOOT 1
#define AUDIO_SCREAM 2

CRGB leds[NUM_LEDS];

Bounce inputTrigger;

bool isTriggered = false;

void changeRedWhiteLights();
void changeGreenLights();
void animation_standby();

Ticker tickerUpdateRedWhiteLights(changeRedWhiteLights, 50);
Ticker tickerUpdateGreenLights(changeGreenLights, 20);
Ticker tickerResetToStandby(animation_standby, 13000);

void delayWithTicker(int mils)
{
  long start = millis();
  while (millis() < start + mils) {
    tickerUpdateRedWhiteLights.update();
    tickerUpdateGreenLights.update();
    tickerResetToStandby.update();
  }
}

void setup() {
  tickerUpdateRedWhiteLights.stop();
  tickerUpdateGreenLights.start();
  tickerResetToStandby.stop();

  setupAudio(PIN_AUDIO_RX, PIN_AUDIO_TX);
  LEDS.addLeds<WS2811, PIN_LEDS>(leds, NUM_LEDS);
  randomSeed(analogRead(0) + 37 * analogRead(1)); //Set the seed to a floating point number because random() gives same set of numbers on every bootup cause the arduino has no clock


  pinMode(PIN_RELAY_CYLINDER_MAN, OUTPUT);
  // Turn relays OFF (HIGH)
  digitalWrite(PIN_RELAY_CYLINDER_MAN, HIGH);

  pinMode(PIN_TRIGGER_BUTTON, INPUT_PULLUP);
  inputTrigger.attach(PIN_TRIGGER_BUTTON);

  Serial.begin(9600);
  Serial.println("Hello World");



  playAudio(AUDIO_BOOT);
}


void loop () {

  while (Serial.available() > 0) {
    int got = Serial.read();
    if (got == 'x') {
      isTriggered = true;
    }
  }

  //inputTrigger.update();

  //if (inputTrigger.fell()) {
  if (isTriggered) {
    // The button is pressed.
    animation_popup();
    isTriggered = false;
  }

  tickerUpdateRedWhiteLights.update();
  tickerUpdateGreenLights.update();
  tickerResetToStandby.update();
}

void animation_standby() {
  tickerUpdateRedWhiteLights.stop();
  tickerUpdateGreenLights.start();
  tickerResetToStandby.stop();

  digitalWrite(PIN_RELAY_CYLINDER_MAN, HIGH);
}

int redState = false;
void animation_popup() {
  tickerUpdateRedWhiteLights.start();
  tickerUpdateGreenLights.stop();
  tickerResetToStandby.start();

  playAudio(AUDIO_SCREAM);

  digitalWrite(PIN_RELAY_CYLINDER_MAN, LOW);
  delayWithTicker(2000);


  for (int i = 0; i < 10 ; i++) {

    digitalWrite(PIN_RELAY_CYLINDER_MAN, HIGH);
    delayWithTicker(500);

    digitalWrite(PIN_RELAY_CYLINDER_MAN, LOW);
    delayWithTicker(500);
  }

  digitalWrite(PIN_RELAY_CYLINDER_MAN, HIGH);
}

void changeRedWhiteLights() {
  if (redState) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
    }
  }
  else {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
    }
  }

  redState = !redState;

  LEDS.show();
}

void changeGreenLights()
{
  leds[random(NUM_LEDS)] = getRandColor(80, 100, 128, 255);

  //Fade all leds every loop
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(randomNumberBetween(0, 20)); //8
  }

  LEDS.show();
}

CHSV getRandColor(int minColor, int maxColor, int minValue, int maxValue) {
  return CHSV(randomNumberBetween(minColor, maxColor), 255, randomNumberBetween(minValue, maxValue));
}

int randomNumberBetween(int min, int max) {
  return random((max - min) + 1) + min;
}

