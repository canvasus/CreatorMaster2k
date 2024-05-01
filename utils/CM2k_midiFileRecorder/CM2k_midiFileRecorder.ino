#include <MIDI.h>
#include <USBHost_t36.h>

USBHost myusb;
USBHub hub1(myusb);
MIDIDevice_BigBuffer midi1(myusb);

void setup() {
  
  delay(100);
  myusb.begin();
  delay(200);
  midi1.setHandleNoteOn(myNoteOn)
  midi1.setHandleNoteOff(myNoteOff)
}

void loop() {
  myusb.Task();
  midi1.read()
}

void myNoteOn {

}

void myNoteOff {

}