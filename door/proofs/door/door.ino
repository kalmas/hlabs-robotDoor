#include <SPI.h>
#include <Ethernet.h> 

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server(192,168,1,109); // me

EthernetClient client;

int RFIDResetPin = 13;

void setup(){
	Serial.begin(9600);
    pinMode(RFIDResetPin, OUTPUT);
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
	char tagString[13];
	int index = 0;
	boolean reading = false;
	
	if (!client.connected()) {
		if (client.connect(server, 8888)) {
			// Serial.println("connected");
		} else {
			Serial.println("connection failed");
			client.stop();
			// do nothing forevermore:
			for(;;)
				;
		}
	}

	while(Serial.available()){
		Serial.println("here2");
		int readByte = Serial.read(); //read next available byte

		if(readByte == 2) reading = true; //begining of tag
		if(readByte == 3) reading = false; //end of tag

		if(reading && readByte != 2 && readByte != 10 && readByte != 13){
			//store the tag
			tagString[index] = readByte;
			index++;
		}
	}
	
	if (index > 0){
		Serial.println("here");
		Serial.println(tagString);
		pingServer(tagString);
		clearTag(tagString); //Clear the char of all value
		resetReader(); 		//reset the RFID reader
	} else {Serial.println("there");}
	
		
	while (client.available()) {
		char c = client.read();
		Serial.print(c);
	}

	// // if the server's disconnected, stop the client:
	// if (!client.connected()) {
		// Serial.println();
		// Serial.println("disconnecting.");
		// client.stop();

		// // do nothing forevermore:
		// for(;;)
		// ;
	// }
	// Serial.print("looped");
}

void pingServer(char tagString[]){
	// Make a HTTP request:
	client.println("GET /openSesame HTTP/1.1");
	//client.println(tag[]);
	client.println();
 }

void lightLED(int pin){
///////////////////////////////////
//Turn on LED on pin "pin" for 250ms
///////////////////////////////////
  Serial.println(pin);

  digitalWrite(pin, HIGH);
  delay(2000);
  digitalWrite(pin, LOW);
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

boolean compareTag(char one[], char two[]){
///////////////////////////////////
//compare two value to see if same,
//strcmp not working 100% so we do this
///////////////////////////////////

  if(strlen(one) == 0) return false; //empty

  for(int i = 0; i < 12; i++){
    if(one[i] != two[i]) return false;
  }

  return true; //no mismatches
}





















