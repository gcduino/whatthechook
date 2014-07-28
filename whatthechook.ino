#include <SoftwareSerial.h>
#include "credentials.h"

#define POST_STRING_START "POST /update HTTP/1.1\nX-THINGSPEAKAPIKEY: "
#define POST_STRING_MIDDLE "\nContent-Type: application/x-www-form-urlencoded\nfield1="

SoftwareSerial wifiSerial(10, 11); // RX, TX

void setup(){

  Serial.begin(9600);
  Serial.println("Begin");
  wifiSerial.begin(9600);

  return;

  int result;
  result = sendATcommand("+++", "",                  "+OK", "+ERR", 100);
  result = sendATcommand("AT+WPRT=!0", "",           "+OK", "+ERR", 100);
  result = sendATcommand("AT+ATPT=!100", "",         "+OK", "+ERR", 100);
  result = sendATcommand("AT+SSID=!", WIFI_SSID,   "+OK", "+ERR", 100);
  result = sendATcommand("AT+ENCRY=!6", "",        "+OK", "+ERR", 100);
  result = sendATcommand("AT+KEY=!1,1,", WIFI_PASSWORD, "+OK", "+ERR", 100);
  result = sendATcommand("AT+Z", "", "+OK", "+ERR", 100);
  delay(10000); // wait for reset
  result = sendATcommand("AT+LKSTT", "", "+OK", "+ERR", 100);
  result = sendATcommand("AT+SKCLS=1", "", "+OK", "+ERR", 100);
  Serial.println(result);  

}

void loop(){

  return;

  int result;

  result = sendATcommand("AT+SKCT=0,0,api.thingspeak.com, 80","", "+OK", "+ERR", 100);
  delay(2000);
  result = sendATcommand("AT+SKSTT=1","", "+OK", "+ERR", 100);
  result = sendATcommand("AT+SKSND=1,POST /update HTTP/1.1\nX-THINGSPEAKAPIKEY: XXX\nContent-Type: application/x-www-form-urlencoded\nfield1=9", "",  "+OK", "+ERR", 100); 
  result = sendATcommand("AT+SKCLS=1", "", "+OK", "+ERR", 100);

  delay(60000);  

}

//
// 
//
int8_t sendATcommand(const char* ATcommand1, const char* ATcommand2, const char* expected_answer1, const char* expected_answer2, unsigned int timeout) {

  const int responseSize = 100;

  Serial.print("SENDING ");
  Serial.print(ATcommand1);
  Serial.println(ATcommand2);

  uint8_t x=0,  answer=0;
  char response[responseSize];
  unsigned long previous;

  delay(100);

  while( wifiSerial.available() > 0) {
    wifiSerial.read();    // Clean the input buffer
  }

  wifiSerial.print(ATcommand1);    // Send the AT command 
  wifiSerial.println(ATcommand2);    // Send the AT command 

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

  return answer;
}





