import processing.sound.*;

import ddf.minim.*;
import ddf.minim.analysis.*;
import ddf.minim.effects.*;
import ddf.minim.signals.*;
import ddf.minim.spi.*;
import ddf.minim.ugens.*;
import processing.serial.*;

Serial myPort;  // Create object from Serial class
boolean firstContact = false; // Whether you've heard from the microcontroller

float input[]; //input array that will be populated by serial

//audio initializers to be passed to constructor
Minim myminim[] = new Minim[4];
AudioPlayer myaudio[] = new AudioPlayer[4];

Track tracks[] = new Track[4]; //array of track objects

class Track {
  //attributes
  Minim minim;
  AudioPlayer player;
  float leftlevel;
  float rightlevel;
  boolean playing;
  float volume;
  
  //constructor
  Track(Minim inminim, AudioPlayer inplayer, Boolean inplaying, float involume) {
    //initialize
    minim = inminim;
    player = inplayer;
    playing = inplaying;
    volume = involume;
  }
  
  //update volume
  void setValueOfVolume(float newvolume) {
     volume = newvolume;
  }
  
  void setLevels(float inleft, float inright) {
    leftlevel = inleft;
    rightlevel = inright;
  }
  
  //update if playing or not
  void setOnOrOff(float onOrOff) {
    if (onOrOff == 0.0) {
      playing = false;
    }
    else if (onOrOff == 1.0) {
      playing = true;
    }
  }
}

void setup() {
  size(1024, 200);
  noStroke();
  fill( 255, 128 );
  
  //listen to port
  String portName = Serial.list()[3]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600);
  
  //initialize audio arrays to be passed to constuctor
  //minim
  for (int i = 0; i < 4; i++) {
    myminim[i] = new Minim(this);
  }
  //player
  myaudio[0] = myminim[0].loadFile("drum.mp3");
  myaudio[1] = myminim[1].loadFile("voice.mp3");
  myaudio[2] = myminim[2].loadFile("piano.mp3");
  myaudio[3] = myminim[3].loadFile("guitar.mp3");
  
  //intialize track objects
  for (int i = 0; i < 4; i++) {
    tracks[i] = new Track(myminim[i], myaudio[i], false, 10.0);
  }
  
  //start every track at the same time
  for (int i = 0; i < 4; i ++) {
    tracks[i].player.loop(100);
    tracks[i].player.mute();
  }
  
}      

void serialEvent(Serial myPort) {
  // read the serial buffer:
  String myString = myPort.readStringUntil('\n');
  
   // if you got any bytes other than the linefeed:
  if (myString != null) {
 
    myString = trim(myString);
 
    // if you haven't heard from the microncontroller yet, listen:
    if (firstContact == false) {
      if (myString.equals("hello")) {
        myPort.clear();          // clear the serial port buffer
        firstContact = true;     // you've had first contact from the microcontroller
        myPort.write('A');       // ask for more
      }
    }
    
    // if you have heard from the microcontroller, proceed:
    else {
      // split the string at the commas
      // and convert the sections into integers:
      input = float(split(myString, ','));
    }
    
    // when you've parsed the data you have, ask for more:
    for (int i = 0; i < 4; i++) {
      myPort.write(String.valueOf(tracks[i].leftlevel));
    }
    
  }
}

void draw() {
  background(0);
  
  if (input != null) {
    
    //iterate through tracks
    for (int i = 0; i < 4; i++) {
          
      //set tracks[i].playing to true or false based on whether the input[i] is 0 or 1
      tracks[i].setOnOrOff(input[i]);
      
      //set tracks[i].volume based on the input
      tracks[i].setValueOfVolume(input[i+4]);
            
      println("track " + i + " is " + tracks[i].playing + " playing with a volume of " + tracks[i].volume);
            
      //if track[i].playing is true, play, otherwise, pause
      if (tracks[i].playing) {
        tracks[i].player.unmute();
        //actually change the volume
        tracks[i].player.setGain(tracks[i].volume);
        tracks[i].setLevels(tracks[i].player.left.level(), tracks[i].player.right.level());
      }
      else {
        tracks[i].player.mute();
        tracks[i].player.rewind();
        tracks[i].setLevels(0.0, 0.0);
      } 
      
      //set levels of each track
      println("left: " + tracks[i].leftlevel + " right: " + tracks[i].rightlevel);
      
      //visualize the RMS (root-mean-square) of each track
      rect(0, i*50, tracks[i].leftlevel*width, 50);
      rect(0, i*50, tracks[i].rightlevel*width, 50);
           
    }
    
  }
  
  
  
}