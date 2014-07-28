#include <SoftwareSerial.h>
#include <stdarg.h>
#include "credentials.h"

SoftwareSerial wifiSerial(10, 11); // RX, TX

int8_t sendATcommand(const char* expected_answer1, const char* expected_answer2, unsigned int timeout, unsigned int argc, ...);

void setup() {

  Serial.begin(9600);
  Serial.println("Begin");
  wifiSerial.begin(9600);

  int result;

  result = sendATcommand("+OK", "+ERR", 100, 1, "+++");
  result = sendATcommand("+OK", "+ERR", 100, 1,"AT+WPRT=!0");
  result = sendATcommand("+OK", "+ERR", 100, 1,"AT+ATPT=!100");
  result = sendATcommand("+OK", "+ERR", 100, 2,"AT+SSID=!", WIFI_SSID);
  result = sendATcommand("+OK", "+ERR", 100, 1,"AT+ENCRY=!6");
  result = sendATcommand("+OK", "+ERR", 100, 2,"AT+KEY=!1,1,", WIFI_PASSWORD);
  result = sendATcommand("+OK", "+ERR", 100, 1,"AT+Z");
  delay(10000); // wait for reset
  result = sendATcommand("+OK", "+ERR", 100, 1,"AT+LKSTT");
  result = sendATcommand("+OK", "+ERR", 100, 1,"AT+SKCLS=1");
}

void loop(){

  int result;

  result = sendATcommand("+OK", "+ERR", 100, 1, "AT+SKCT=0,0,api.thingspeak.com, 80");
  delay(2000);
  result = sendATcommand("+OK", "+ERR", 100, 1, "AT+SKSTT=1");
  result = sendATcommand("+OK", "+ERR", 100, 4, "AT+SKSND=1,POST /update HTTP/1.1\nX-THINGSPEAKAPIKEY:", THING_SPEAK_API_KEY, "\nContent-Type: application/x-www-form-urlencoded\nfield1=", "9");
  result = sendATcommand("+OK", "+ERR", 100, 1, "AT+SKCLS=1");

  delay(60000);  

}

//
// 
//
int8_t sendATcommand(const char* expected_answer1, const char* expected_answer2, unsigned int timeout, unsigned int argc, ...) {

  va_list args;
  va_start(args, timeout);

  char* command;

  const int responseSize = 100;

  Serial.print("SENDING ");
  for(int i=0; i < argc; i++) {
    Serial.print(va_arg(args, char* ));
  }
  Serial.println("");

  uint8_t x=0,  answer=0;
  char response[responseSize];
  unsigned long previous;

  delay(100);

  while( wifiSerial.available() > 0) {
    wifiSerial.read();    // Clean the input buffer
  }

  for(int i=0; i < argc; i++) {
    wifiSerial.print(va_arg(args, char* ));
  }
  wifiSerial.println("");


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if(wifiSerial.available() != 0) {    
      if(x < responseSize) { 
        // don't run off the end of the array!
        response[x] = wifiSerial.read();

        // check if the desired answer 1  is in the response of the module
        if (!strncmp(response, expected_answer1, x)) {
          answer = 1;
        }
        // check if the desired answer 2 is in the response of the module
        else if (!strncmp(response, expected_answer2, x)) {
          answer = 2;
        }

        x++;
      } 
      else {
        // if we do have too much - just flush it out
        wifiSerial.read();
      }
    }
  }
  // Waits for the asnwer with time out
  while((answer == 0) && ((millis() - previous) < timeout));    

  if(answer == 0) {
    Serial.println("TIMEOUT");
  }

  va_end(args); 

  Serial.print("ANSWER:");
  Serial.println(answer);  
  return answer;
}







