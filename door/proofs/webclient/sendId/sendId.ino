#include <SPI.h>
#include <Ethernet.h>

// rfid reader variables
int RFIDResetPin = 9;

// webclient variables
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char server[] = "192.168.1.117";
int port = 8888;
unsigned long lastConnectionTime = 0;  // last time you connected to the server, in milliseconds
boolean lastConnected = false; // state of the connection last time through the main loop
const unsigned long postingInterval = 15*1000; // delay between updates, in milliseconds
EthernetClient client; 


void setup() {
  Serial.begin(9600);
  
  // rfid reader setup
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  // ONLY NEEDED IF CONTROLING THESE PINS - EG. LEDs
  pinMode(2, OUTPUT);
  pinMode(9, OUTPUT);
  
  // webclient setup
  delay(1000);  // give the ethernet module time to boot up
  Ethernet.begin(mac);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  char tagString[13];
  int index = 0;
  boolean reading = false;
  
  // if there's incoming data on net, send it out the serial port
  while(client.available()){
    char c = client.read();
    Serial.print(c);
  }
  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
  // if there's incoming data on serial, send it out the serial port
  while(Serial.available()){
    int readByte = Serial.read(); //read next available byte

    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      tagString[index] = readByte;
      index ++;
    }
  }

  if(index > 0) {
    httpRequest(tagString);
	clearTag(tagString); // clear the char of all value
    resetReader(); // reset the RFID reader
  }
  
  // store the state of the connection for next time through the loop
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void httpRequest(char tag[]) {
  // if there's a successful connection:
  if (client.connect(server, port)) {
    Serial.println("connecting...");
    // send the HTTP GET
	String request = "GET /open?q=" + tag + " HTTP/1.1":
    client.println(request);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

void resetReader(){
///////////////////////////////////
//Reset the RFID reader to read again.
///////////////////////////////////
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char one[]){
///////////////////////////////////
//clear the char array by filling with null - ASCII 0
//Will think same tag has been read otherwise
///////////////////////////////////
  for(int i = 0; i < strlen(one); i++){
    one[i] = 0;
  }
}




