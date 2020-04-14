#include <SPI.h>
#include <WiFi101.h>

char ssid[] = "<SSID>";
char pass[] = "<PASSWORD>";

int LAMP_PIN = 7;

int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  setPinModes();
  connect();
}


void loop() {
  listen();
}

void setPinModes() {
  pinMode(LAMP_PIN, OUTPUT);
}

void listen() {
  WiFiClient client = server.available();
  if (client) {
    String line = "";
    while(client.connected()) {
      if(client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (line.length() == 0) {
            break;
          }
          else {
            line = "";
          }
        }
        else if (c != '\r') {
          line += c;
        }
        if (line.endsWith("HTTP/1.1")) {
          handleRequest(line, client);
        }
      }
    }
    client.stop();
  }
}

void handleRequest(String line, WiFiClient client) {
  String method = getMethod(line);
  String url = getUrl(line);
  if (method == "POST") handlePostRequest(url, client);
  if (method == "GET") handleGetRequest(url, client);
}

void handleGetRequest(String line, WiFiClient client) {
  return;
}

void handlePostRequest(String line, WiFiClient client) {
  if (line.indexOf("lamp") > 0) handlePostRequestForLamp(line, client);
}

void handlePostRequestForLamp(String line, WiFiClient client) {
  if (line.endsWith("True")) {
    digitalWrite(LAMP_PIN, HIGH);
    responseSuccess(client);
  }
  if (line.endsWith("False")) {
    digitalWrite(LAMP_PIN, LOW);
    responseSuccess(client);
  }
}

String getMethod(String line) {
  String method = "";
  for (char c : line) {
    if (c == ' ') return method;
    method += c;
  }
}

String getUrl(String line) {
  String url = "";
  bool slashFound = false;
  bool spaceFound = false;
  for (char c : line) {
    if (c == '/') slashFound = true;
    if (c == ' ') {
      if (spaceFound) return url;
      else spaceFound = true;
    }
    if (slashFound) {
      url += c;
    }
  }
}

void connect() {
  WiFi.config(IPAddress(192, 168, 0, 230));
  while (status != WL_CONNECTED) {
// Serial.println("Connecting...");
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  // Serial.println("Connected!");
  // printIp();
  server.begin();
}

void responseSuccess(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
}

void printIp() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(ip);
}
