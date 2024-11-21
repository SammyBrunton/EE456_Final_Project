#include <iostream>
#include <RadioLib.h>
#include "PiHal.h"
#include <cstdio>
#include <sys/socket.h> // For socket functions
#include <arpa/inet.h> // For inet_pton()
#include <unistd.h> // For close()
#include <cstring>
using namespace std;

#define PORT 54321


// Create a new instance of the HAL class
PiHal* hal = new PiHal(0); // 0 for SPI 0 , set to 1 if using SPI 1(this will change NSS pinout)

// Create the radio module instance/////////////////////////
// Pinout *****MBED SHIELD****************PI HAT************
// NSS pin:  WPI# 10 (GPIO 8)  WPI # 29 (GPIO 21) for Pi hat
// DIO1 pin: WPI# 2  (GPIO 27) WPI # 27 (GPIO 16) for Pi hat
// NRST pin: WPI# 21 (GPIO 5)  WPI # 1  (GPIO 18) for Pi hat
// BUSY pin: WPI# 0  (GPIO 17) WPI # 28 (GPIO 20) for Pi hat
////////////////////////////////////////////////////////////

// Radio initialization based on Pi Hat wiring
// change for MBED Shield use
// According to SX1262 radio = new Module(hal, NSS,DI01,NRST,BUSY)
Module* module = new Module(hal, 29, 27, 1, 28);
SX1262 radio(module);

// Flag to indicate packet has been received
volatile bool receivedFlag = false;

// ISR function to set received flag
void setFlag(void) {
  receivedFlag = true;
}

int main(int argc, char** argv) {
  hal->init(); 
  radio.XTAL = true;

  //////
 int sock = socket(AF_INET, SOCK_STREAM, 0);
 if (sock < 0) {
 perror("Socket creation error");
 return -1;
 }
 cout << "Socket created successfully." << endl;

// Part 3 : Define Server Address and Connect
struct sockaddr_in serv_addr;
 serv_addr.sin_family = AF_INET;
 serv_addr.sin_port = htons(PORT);
 // Convert IPv4 address from text to binary
 if (inet_pton(AF_INET, "10.40.120.82", &serv_addr.sin_addr) <= 0) {
 perror("Invalid address / Address not supported");
 return -1;
 }
 if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
{perror("Connection failed");
 return -1;
 }
 //cout << "Connected to the server." << endl;
  //////

  // Initialize the radio module
  printf("[SX1262] Initializing ... ");
  int state = radio.begin(915.0, 125.0, 7, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0.0, false);
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  // Set the ISR for packet received
  radio.setPacketReceivedAction(setFlag);

  // Start receiving packets
  printf("[SX1262] Starting to listen ... ");
  state = radio.startReceive();
  if (state != RADIOLIB_ERR_NONE) {
    printf("failed, code %d\n", state);
    return(1);
  }
  printf("success!\n");

  // Main loop
  uint8_t buff[256] = { 0 };
 // for(;;) {
  while(!receivedFlag) {
    ;
  }
   // if(receivedFlag) {
      // Reset flag
     // receivedFlag = false;
      
      // Get the length of the received packet
      size_t len = radio.getPacketLength();
      
      // Read the packet
      int state = radio.readData(buff, len);
      if (state != RADIOLIB_ERR_NONE) {
        printf("Read failed, code %d\n", state);
      } 
      //else {
      //  printf("Data: %s\n", (char*)buff);
      //}


//////

 
 // Part 4 : Send and Receive Data
 //const char *hello = ;
 send(sock, (char*)buff, strlen( len ), 0);
 cout << "Received GPS Data Sent to Server." << endl;
 char buffer[1024] = {0};
 read(sock, buffer, 1024);
 cout << "Message from server: " << buffer << endl;
 close(sock);
//////
  


  
   // }
  //}

  return(0);
}