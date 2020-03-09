#include <FastLED.h>
#include "AudioPlayer.h"
#include "Bounce2.h"
#include "Ticker.h"

#define NUM_LEDS 50

#define PIN_TRIGGER_BUTTON 4
#define PIN_LEDS 6
#define PIN_RELAY_CYLINDER_MAN 9
#define PIN_RELAY_SMOKE 8
#define PIN_AUDIO_RX 10
#define PIN_AUDIO_TX 12

#define AUDIO_BOOT 1
#define AUDIO_SCREAM 3

#define SKELETON_UP LOW
#define SKELETON_DOWN HIGH

#define SMOKE_ON_MS 6000
#define SMOKE_OFF_MS 22000
#define SMOKE_ON_VALUE LOW
#define SMOKE_OFF_VALUE HIGH

CRGB leds[NUM_LEDS];

Bounce inputTrigger;

bool isTriggered = false;

void changeRedWhiteLights();
void changeGreenLights();
void animation_standby();
void turn_smoke_on();
void turn_smoke_off();

Ticker tickerUpdateRedWhiteLights(changeRedWhiteLights, 100);
Ticker tickerUpdateGreenLights(changeGreenLights, 20);
Ticker tickerResetToStandby(animation_standby, 3500);
Ticker tickerSmokeOff(turn_smoke_on, SMOKE_OFF_MS);
Ticker tickerSmokeOn(turn_smoke_off, SMOKE_ON_MS);

void delayWithTicker(int mils)
{
  long start = millis();
  while (millis() < start + mils) {
    tickerUpdateRedWhiteLights.update();
    tickerUpdateGreenLights.update();
    tickerResetToStandby.update();
    tickerSmokeOn.update();
    tickerSmokeOff.update();
  }
}

void setup() {
  tickerUpdateRedWhiteLights.stop();
  tickerUpdateGreenLights.start();
  tickerResetToStandby.stop();
  
  setupAudio(PIN_AUDIO_RX, PIN_AUDIO_TX);
  LEDS.addLeds<WS2811, PIN_LEDS>(leds, NUM_LEDS);
  randomSeed(analogRead(A0) + 37 * analogRead(A1)); //Set the seed to a floating point number because random() gives same set of numbers on every bootup cause the arduino has no clock


  pinMode(PIN_RELAY_CYLINDER_MAN, OUTPUT);
  pinMode(PIN_RELAY_SMOKE, OUTPUT);
  turn_smoke_off();

  // Turn relays OFF (HIGH)
  digitalWrite(PIN_RELAY_CYLINDER_MAN, SKELETON_DOWN);

  pinMode(PIN_TRIGGER_BUTTON, INPUT_PULLUP);
  inputTrigger.attach(PIN_TRIGGER_BUTTON);

  Serial.begin(9600);
  Serial.println(F("Hello World"));



  playAudio(AUDIO_BOOT);
}


void loop () {

  while (Serial.available() > 0) {
    int got = Serial.read();
    if (got == 'x') {
      //Serial.println(F("Running..."_);
      isTriggered = true;
    }
  }

  inputTrigger.update();

  if (inputTrigger.fell()) {
      isTriggered = true;
      //Serial.println(F("Running..."_);
  }
  if (isTriggered) {
    // The button is pressed.
    animation_popup();
    isTriggered = false;
  }

  tickerUpdateRedWhiteLights.update();
  tickerUpdateGreenLights.update();
  tickerResetToStandby.update();
  tickerSmokeOn.update();
  tickerSmokeOff.update();
}

void animation_standby() {
  tickerUpdateRedWhiteLights.stop();
  tickerUpdateGreenLights.start();
  tickerResetToStandby.stop();

  digitalWrite(PIN_RELAY_CYLINDER_MAN, SKELETON_DOWN);
}

int redState = false;
void animation_popup() {
  
  digitalWrite(PIN_RELAY_CYLINDER_MAN, SKELETON_UP);

  delayWithTicker(500);

  playAudio(AUDIO_SCREAM);

  tickerUpdateRedWhiteLights.start();
  tickerUpdateGreenLights.stop();
  tickerResetToStandby.start();

  delayWithTicker(3000);

  digitalWrite(PIN_RELAY_CYLINDER_MAN, SKELETON_DOWN);
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
  leds[random(NUM_LEDS)] = getRandColor(60, 150, 128, 255);

  //Fade all leds every loop
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(randomNumberBetween(0, 20)); //8
  }

  LEDS.show();
}

void turn_smoke_on()
{
  digitalWrite(PIN_RELAY_SMOKE, SMOKE_ON_VALUE);
  tickerSmokeOff.stop();
  tickerSmokeOn.start();
}

void turn_smoke_off()
{
  digitalWrite(PIN_RELAY_SMOKE, SMOKE_OFF_VALUE);
  tickerSmokeOn.stop();
  tickerSmokeOff.start();
}

CHSV getRandColor(int minColor, int maxColor, int minValue, int maxValue) {
  return CHSV(randomNumberBetween(minColor, maxColor), 255, randomNumberBetween(minValue, maxValue));
}

int randomNumberBetween(int min, int max) {
  return random((max - min) + 1) + min;
}

