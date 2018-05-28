#include <Tone.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

/* Initialise with default values (int time = 2.4ms, gain = 1x) */
// Adafruit_TCS34725 tcs = Adafruit_TCS34725();

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);

Tone tone1;
Tone tone2;

/* Variables used to hold color values */
uint16_t r, g, b, c, colorTemp, lux;

//Knobs
const int KNOB1_PIN = 0;
const int KNOB2_PIN = 1;
const int KNOB3_PIN = 2;
const int BUTTON_PIN = 3;
const int BUTTON_LED_PIN = 8;

const int SPEAKER1_PIN = 9;
const int SPEAKER2_PIN = 10;

// min and max values of synth parameters to map AutoRanged analog inputs to
const int MIN_SPEED = 50;
const int MAX_SPEED = 500;

const int MIN_DECAY = 10;
const int MAX_DECAY = 100;

const int MIN_STEPS = 3;
const int MAX_STEPS = 8;

const float all_tones[45] {16.35, 18.35, 20.6, 24.5, 27.5, 32.7, 36.71, 41.2, 49, 55, 65.41, 73.42, 82.41, 98, 110, 130.81, 146.83, 164.81, 196, 220, 261.63, 293.66, 329.63, 392, 440, 523.25, 587.33, 659.26, 783.99, 880, 1046.5, 1174.66, 1318.51, 1567.98, 1760, 2093, 2349.32, 2637.02, 3135.96, 3520, 4186.01, 4698.64, 5274.04, 6271.93, 7040};

const int arp_steps[12] = {0,1,2,1,0,1,2,1,0,1,2,1};

void setup(void) {
  Serial.begin(9600);

  tone1.begin(SPEAKER1_PIN);
  tone2.begin(SPEAKER2_PIN);

  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }
}

void loop(void) {

  readColorValues();

  if (analogRead(BUTTON_PIN) > 1020) {
    digitalWrite(BUTTON_LED_PIN, HIGH);
    int steps = getSteps();
    int vs[3] = {r,g,b};
    for(int i = 0; i < steps; i++) {
      if(i == steps-1) { 
        playTwoNotes(vs[arp_steps[i]], vs[(arp_steps[i]+1)%3], true); 
      }
      else {
        playTwoNotes(vs[arp_steps[i]], vs[(arp_steps[i]+1)%3], false); 
      }
    }
  }
  else {
    digitalWrite(BUTTON_LED_PIN, LOW);
  }
}

void readColorValues() {
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature(r, g, b);
  lux = tcs.calculateLux(r, g, b);

  if (false) {
    Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
    Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
    Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
    Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
    Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
    Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
    Serial.println(" ");
  }
}

void playNote(int note, boolean last_note) {
    int l = getLength();
    int note_l = getDecay() * l;
    if(false) {
      Serial.println(note_l);
    }
    tone1.play(getNote(note), note_l);
    tone2.play(getNote(note), note_l);
    if (last_note) { delay(l-50); }
    else { delay(l); }
}

void playTwoNotes(int note1, int note2, boolean last_note) {
    int l = getLength();
    int note_l = getDecay() * l;
    if(false) {
      Serial.println(note_l);
    }
    tone1.play(getNote(note1), note_l);
    tone2.play(getNote(note2), note_l);
    if (last_note) { delay(l-50); }
    else { delay(l); }
}

float getDecay() {
    return (analogRead(KNOB3_PIN) / 1023.0 * MAX_DECAY + MIN_DECAY) / 100.0;
}

int getSteps() {
    int v = analogRead(KNOB2_PIN);
    if(v < 100) {
      return 1;
    }
    if(v < 200) {
      return 2;
    }
    if(v < 300) {
      return 3;
    }
    if(v < 400) {
      return 4;
    }
    if(v < 500) {
      return 5;
    }
    if(v < 600) {
      return 6;
    }
    if(v < 700) {
      return 7;
    }
    if(v < 800) {
      return 8;
    }
    if(v < 900) {
      return 9;
    }
    return 10;
}

int getLength() {
  float l = analogRead(KNOB1_PIN) / 1023.0 * MAX_SPEED + MIN_SPEED;
  if(false) {
    Serial.print(l);
    Serial.print(" - ");
  }
  return l;
}

int getNote(int sensor_value) {
  int v = sensor_value/2;
  int num_notes = sizeof(all_tones) / sizeof(float);
  for (int i = 1; i < num_notes; i++) {
    if (v >= all_tones[i - 1] && v < all_tones[i]) {
      if ((v - all_tones[i - 1]) <= (all_tones[i] - v)) {
        v = all_tones[i - 1];
      }
      else {
        v = all_tones[i];
      }
    }
  }
  return v;
}

