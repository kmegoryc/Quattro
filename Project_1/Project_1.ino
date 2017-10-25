#include <Adafruit_MPR121.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

//neopixel variables
#define N_LEDS 8

//noepixel object
Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(N_LEDS, 3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS, 5, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS, 9, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(N_LEDS, 10, NEO_GRB + NEO_KHZ800);


//potentiometer variables
const int potPin0 = A0;
const int potPin1 = A1;
const int potPin2 = A2;
const int potPin3 = A3;
int potValue[4];
float volume[4];
int led_val[4];

//capacitive touch shield object
Adafruit_MPR121 cap = Adafruit_MPR121();

//capacitive touch shield variables
bool playing[4];
//keeps track of the last pins touched so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void establishContact() {
  while (Serial.available() <= 0) {
      Serial.println("hello");   // send a starting message
      delay(300);
  }
}

void setup() {
  strip0.begin();
  strip1.begin();
  strip2.begin();
  strip3.begin();
  strip0.show();
  strip1.show();
  strip2.show();
  strip3.show();
  
  
  pinMode(potPin0, INPUT);
  pinMode(potPin1, INPUT);
  pinMode(potPin2, INPUT);
  pinMode(potPin3, INPUT);

  for (int i = 0; i < 4; i++) {
    volume[i] = 0;
  }

  //intialize serial communication
  Serial.begin(9600);
  
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");

  establishContact(); 
}

void loop() {

  //POTENTIOMETER

  potValue[0] = analogRead(potPin0);
  potValue[1] = analogRead(potPin1);
  potValue[2] = analogRead(potPin2);
  potValue[3] = analogRead(potPin3);

  for (int i = 0; i < 4; i++) {
    //Serial.print(potValue[i]); Serial.print(",");
    volume[i] = map(potValue[i], 0, 1023, -10, 10);
  }

  //POTENTIOMETER
  
  //CAPACITIVE TOUCH
  
  // get the currently touched pads
  currtouched = cap.touched();
  
  for (uint8_t i = 0; i < 12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {

      for (int j = 0; j < 4; j++) {
        if (!playing[j] && (i == j)) {
          playing[j] = true;
        } 
        else if (playing[j] && (i == j)) {
          playing[j] = false;
        }
      }
      
    }
  }

  // reset our state
  lasttouched = currtouched;

  //CAPACITIVE TOUCH

  //NEOPIXEL

  for (int i = 0; i < 4; i++) {
    led_val[i] = map(potValue[i], 0, 1023, 0, 8);
  }

  //clears all the pixels
  for (int i = 0; i < 8; i++) {
    strip0.setPixelColor(i, strip0.Color(0, 0, 0)); //white
    strip1.setPixelColor(i, strip1.Color(0, 0, 0)); //white
    strip2.setPixelColor(i, strip2.Color(0, 0, 0)); //white
    strip3.setPixelColor(i, strip3.Color(0, 0, 0)); //white
  }

  if (playing[0]) {
    for (int i = 0; i < led_val[0]; i++) {
      strip0.setPixelColor(i, strip0.Color(255, 0, 0)); //red
      strip0.show(); //this sends the updated pixel color to the hardware
    }
  }
  
  if (playing[1]) {
    for (int i = 0; i < led_val[1]; i++) {
      strip1.setPixelColor(i, strip1.Color(255, 255, 0)); //yellow
      strip1.show(); //this sends the updated pixel color to the hardware
    }
  }
  
  if (playing[2]) {
    for (int i = 0; i < led_val[2]; i++) {
      strip2.setPixelColor(i, strip2.Color(0, 150, 0)); //green
      strip2.show(); //this sends the updated pixel color to the hardware
    }
  }
  
  if (playing[3]) {
    for (int i = 0; i < led_val[3]; i++) {
      strip3.setPixelColor(i, strip3.Color(0, 0, 255)); //blue
      strip3.show(); //this sends the updated pixel color to the hardware
    }
  }

  //NEOPIXEL

  if (Serial.available() > 0) {
    // read the incoming byte:
    int inByte = Serial.read();
    
    //print playing (0 or 1) values to serial port
    for (int i = 0; i < 4; i++) {
      Serial.print(playing[i]); Serial.print(","); 
    } 

    //print volume (0 to 1) values to serial port
    for (int i = 0; i < 3; i++) {
      Serial.print(volume[i]); Serial.print(",");
    }
    Serial.println(volume[3]);
    
  }

   

}

