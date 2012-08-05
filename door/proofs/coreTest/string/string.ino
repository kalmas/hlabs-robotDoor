


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
}

void loop() {
  // using a constant String:
  String requestStr = "GET /open?q=";
  while(Serial.available()){
    char c = Serial.read();
    requestStr += c;
  }
  requestStr += " HTTP/1.1";
  
  Serial.println(requestStr);
  
  if (requestStr == "GET /open?q=123 HTTP/1.1"){
    Serial.println("true");
  } else {
    Serial.println("false");
  }
  delay(2000);

}
