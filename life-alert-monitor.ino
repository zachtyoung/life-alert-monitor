#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

//declare the webserver
ESP8266WebServer server(80);
int maxValue = 0;
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect
  }
  
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(F("Connecting"));
        
    WiFi.persistent(false);       // WiFi config isn't saved in flash
    WiFi.mode(WIFI_STA);          // use WIFI_AP_STA if you want an AP
    WiFi.hostname("ESP8266");    // must be called before wifi.begin()
    WiFi.begin("RobotZone", "RobotZone1");
   
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
  }
     
  Serial.println();
  Serial.print(F("IP address: "));
  Serial.println(WiFi.localIP());  
  
  //start the webserver
  server.begin();
  //Server Sent Events will be handled from this URI
  server.on("/", handleSSEdata);
}

void loop() {
  // listen for incoming clients
  server.handleClient();
}

void handleSSEdata(){
  WiFiClient client = server.client();
  
  if (client) {
    Serial.println("new client");
    serverSentEventHeader(client);
    while (client.connected()) {
      int curr = analogRead(A0);
      if(curr > maxValue){
      maxValue = curr;
      serverSentEvent(client);
      }
 
      delay(500); // round about 60 messages per second
    }

    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void serverSentEventHeader(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/event-stream;charset=UTF-8");
  client.println("Connection: close");  // the connection will be closed after completion of the response
  client.println("Access-Control-Allow-Origin: *");  // allow any connection. We don't want Arduino to host all of the website ;-)
  client.println("Cache-Control: no-cache");  // refresh the page automatically every 5 sec
  client.println();
  client.flush();
}

void serverSentEvent(WiFiClient client) {
  client.println("event: esp8266"); // this name could be anything, really.
  client.print("data: {");
  client.print("\"A0\": ");
  client.print(analogRead(A0));
  client.println("}");
  client.println();
  client.flush();
}
