#include <SPI.h>
#include <Ethernet.h> 

// Server IP
IPAddress server(192,168,1,114);
// Server Port
int portNum = 8888;
// Server Endpoint
char endpoint[] = "openSesame";
// RFID Reset Pin
int RFIDResetPin = 9;
// Door Open Pin
int doorOpenPin = 2;
// Card MAC Address
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client;

void setup(){
	Serial.begin(9600);
        pinMode(RFIDResetPin, OUTPUT);
        pinMode(doorOpenPin, OUTPUT);
	digitalWrite(RFIDResetPin, HIGH);

	// start the Ethernet connection:
	if (!Ethernet.begin(mac)) {
		// if DHCP fails, start with a hard-coded address:
		Serial.println("failed to get an IP address using DHCP, trying manually");
		IPAddress ip(192,168,1,20);
		Ethernet.begin(mac, ip);
	}
	Serial.println(Ethernet.localIP());
	
	// give the Ethernet shield a second to initialize:
	delay(1000);
}

void loop(){
	 delay(1000);
         while(!client.connected()){
           attemptConnect();
           delay(1000);
         }
         Serial.println("Connection Ready");         

        /*
         * If there data coming in from serial, store as a rfid tag
         */
        char tagString[] = "1234567890";
        // clearTag(tagString);
	int index = 0;
	boolean reading = false;
	while(Serial.available()){
		int readByte = Serial.read(); //read next available byte

		if(readByte == 2) reading = true; //begining of tag
		if(readByte == 3) reading = false; //end of tag

		if(reading && readByte != 2 && readByte != 10 && readByte != 13 && index < strlen(tagString)){
			// Store the tag
			tagString[index] = readByte;
			index++;
		}
	}
        
        delay(100); // seems we require a delay here. idunno.
	if (index > 0){
                Serial.print("Tag: ");
		Serial.println(tagString);
		sendRequest(tagString);
		resetReader();
	}
	
        index = 0;
        char response[] = "123456789012345";
        delay(100);
	while (client.available()) {
          char c = client.read();
          Serial.print(c);
          if(index < strlen(response)){ 
            // Store relevant part of response
            response[index] = c; 
          }
          index++;
	}
        Serial.print("Response: ");
        Serial.println(response);

        if(evaluateResponse(response)){
          openDoor();
        }
}

void attemptConnect(){
  Serial.println("Attempting to Connect");
  client.connect(server, portNum);
}

boolean evaluateResponse(char response[]){
  if(response[9] == '2' && response[10] == '0' && response[11] == '0'){
    Serial.println("Access Granted");
    return true;
  }
  Serial.println("Access Denied");
  return false;
}

void sendRequest(char tagString[]){
  // Make a HTTP request:
  Serial.println("Making HTTP Request");
  client.print("GET /");
  client.print(endpoint);
  client.print("?q=");
  client.print(tagString);
  // client.print("123");
  client.println(" HTTP/1.1");
  client.println("User-Agent: arduino-ethernet");
  client.println("Connection: Keep-Alive");
  client.println();
}

void openDoor(){
  Serial.println("Opening Door");
  digitalWrite(doorOpenPin, HIGH);
  delay(2000);
  digitalWrite(doorOpenPin, LOW);
}

void resetReader(){
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char tag[]){
  for(int i = 0; i < strlen(tag); i++){
    tag[i] = 0;
  }
}






















