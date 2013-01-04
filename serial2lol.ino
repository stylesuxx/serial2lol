/*
Display a serially received string on the LoL shield.
As soon as it is possible to display new text, the arduino will serially 
print READY every second until it receives a new String to process.

In the application sending to the arduino set the baudrate to 115200.
Also let the arduino some time to setup properly - sometimes the first 
bytes arrive somehow crippled, in this case a 1 sec delay before sending
the first string helps.
*/

#include "Charliplexing.h"
#include "Myfont.h"

String inputString = ""; 
boolean stringComplete = false;
boolean readingString = false;

void setup () {  
  cli(); // Disable interrupts
  
  //set timer1 interrupt at 1Hz
  TCCR1A = 0;                          // set entire TCCR1A register to 0
  TCCR1B = 0;                          // same for TCCR1B
  TCNT1 = 0;                           //initialize counter value to 0
  
  // set compare match register for 1hz increments
  OCR1A = 15624;                       // = (16*10^6) / (1*1024) - 1 (must be <65536)
  TCCR1B |= (1 << WGM12);              // turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set CS12 and CS10 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A);             // enable timer compare interrupt
  
  sei(); // Enable interrupts
}

void loop () {
  // Display the string to the lol shield when a newline arrives
  if(stringComplete) {
    char text[inputString.length()];
    inputString.toCharArray(text, inputString.length());
    
    LedSign::Init();
    Myfont::Banner(inputString.length(), (unsigned char *) text);
       
    // Reset flags and String to be ready for new input
    readingString = false;
    stringComplete = false;
    inputString = "";
  }
}

ISR (TIMER1_COMPA_vect) {
 if(!readingString) {
    Serial.begin(115200);
    Serial.print("READY");
 }
}

void serialEvent () {
  while(Serial.available()) {
    readingString = true;
    char inChar = (char) Serial.read();
    inputString += inChar;
    if(inChar == '\n') {
      stringComplete = true;
    } 
  }
}
