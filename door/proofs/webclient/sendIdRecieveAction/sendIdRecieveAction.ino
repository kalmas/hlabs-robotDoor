#include <SPI.h>
#include <Ethernet.h>

// rfid reader variables
int RFIDResetPin = 9;

// webclient variables
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
char server[] = "192.168.1.108";
int port = 8888;
unsigned long lastConnectionTime = 0;  // last time you connected to the server, in milliseconds
boolean lastConnected = false; // state of the connection last time through the main loop
const unsigned long postingInterval = 15*1000; // delay between updates, in milliseconds
EthernetClient client; 

// lock open variables
int lockOpenPin = 2;

void setup() {
  Serial.begin(9600);
  
  // rfid reader setup
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  
  // webclient setup
  delay(1000);  // give the ethernet module time to boot up
  Ethernet.begin(mac);
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  // lock open setup
  pinMode(lockOpenPin, OUTPUT);
}

void loop() {
  String tagString = "";
  boolean reading = false;
  boolean lockOpen = false;
  String responseStr = "";
  
  // check incoming net data
  while(client.available()){
    char c = client.read();
    Serial.print(c);
    
    if (c != '\n'){
      responseStr += c;
    } else {
      if (responseStr == "Action: OpenDoorLock"){
        lockOpen = true;
        // break;
      }
      responseStr = "";      
    }
  }

  if (lockOpen){
    Serial.print("Door Open");
    digitalWrite(lockOpenPin, HIGH);
    delay(2000);
    Serial.print("Door Close");
    digitalWrite(lockOpenPin, LOW);
    lockOpen = false;
  }
  
  // if there's no net connection but there was one last time, stop the client
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("CLOSING CONNECTION");
    client.stop();
  }
  
  // if there's incoming data on serial, read in tagString
  while(Serial.available()){
    int readByte = Serial.read();
    if(readByte == 2) reading = true; //begining of tag
    if(readByte == 3) reading = false; //end of tag

    if(reading && readByte != 2 && readByte != 10 && readByte != 13){
      //store the tag
      Serial.println(readByte);
      tagString += readByte;
    }
  }
  
  // seems like a delay here is required
  delay(1000);
  
  httpRequest(tagString); // send http request
  resetReader(); // reset the RFID reader
  
  // store the state of the connection for next time through the loop
  lastConnected = client.connected();
}

void httpRequest(String tagString) {
  String requestStr = "GET /open?q=";
  requestStr += tagString;
  requestStr += " HTTP/1.1";
  
  delay(1000);
  if (client.connect(server, port)) {
    Serial.println(requestStr);
    // Send HTTP GET
    client.println(requestStr);
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("CONNECTION FAILED");
    client.stop();
  }
}

void resetReader(){
  //Reset the RFID reader to read again.
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

//void clearTag(char tagString[]){/
//   for(int i = 0; i < strlen(tagString); i++){
//    tagString[i] = ' ';
//   }
//}




