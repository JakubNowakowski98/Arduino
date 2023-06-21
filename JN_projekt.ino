#include <WiFi.h>
#include <WebServer.h>
#include <DS18B20.h>
const char* ssid = "Galaxy A329F28"; // Your ssid
const char* password = "jsin7241"; // Your Password
int licznik = 0;
int temp_licznik = 0;
float tempAll[10][1];
String addressAll[10][1];
String addressAsName[5][2];
String sOut="";
byte autorefresh;
String adres = "";
int autoRefresh = 10;
unsigned short lastTime = 0;
unsigned short delayTime = 2000;
DS18B20 ds(14);
WebServer webServer(80);
uint8_t address[8];

void handleRoot() {
  while(ds.selectNext())
      {
        adres = "";
        ds.getAddress(address);
        tempAll[licznik][0] = ds.getTempC();
        for (byte i = 0; i < 8; i++) {
        adres += String(address[i],HEX);
        }
        addressAll[licznik][0] = adres;
        licznik++;
      }
    temp_licznik = licznik;
  int sec = millis() / 1000;
  int mi = sec / 60;
  int hr = mi / 60;
  char tempS[100];
  sOut="<html><head><meta http-equiv='refresh' content='";
  sOut+=String(int(autoRefresh))+"'/>";
  sOut += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" /><meta http-equiv=\"Content-Language\" content=\"pl\" /><title>ESP32</title>\
  <style>h1{text-align: center;}\
  @media screen and (min-width: 800px) {\
    .container {\
        width: 47%;\
        margin: 10px;\
        text-align: center;\
    }\
  }\
  @media screen and (min-width: 1200px) {\
    .container {\
        width: 23%;\
        margin: 10px;\
        text-align: center;\
    }\
  }\
  </style>\
  </head><body style='background-color:#E0E0E0;'>\
  <h1>ESP32 projekt zaliczeniowy - Jakub Nowakowski</h1><br>\
  <h1>Adres IP urządzenia: " +String(WiFi.localIP().toString())+"</h1><br>";


  sOut += "<div style=\"display: flex; flex-wrap: wrap;\">";
  for (int i = 0; i < temp_licznik; i++) {
    sOut += "<div class = 'container'; ";
    // Ustawienie koloru dla temperatur mniejszych od 25 (niebieski)
    if (tempAll[i][0] <= 26) {
      sOut += "style = 'background-color: #CCE5FF;'";
    }
    // Ustawienie koloru dla temperatur z zakresu 25-26 (zielony)
    else if (tempAll[i][0] > 26 && tempAll[i][0] <= 28) {
      sOut += "style = 'background-color: #B3FFCC;'";
    }
    // Ustawienie koloru dla temperatur większych od 26 (czerwony)
    else {
      sOut += "style = 'background-color: #FFCCCC;'";
    }
    sOut += "\">";
    sOut += "<h3>Temperatura<br>";
    sOut += String(tempAll[i][0]) + " C<br>";
    for (int j = 0; j < 5; j++) {
      if (String(addressAll[i][0]) == addressAsName[j][0]) {
        sOut += String(addressAsName[j][1]);
        break;
      } else if (j == 4) {
        sOut += String(addressAll[i][0]);
      }
    }
    sOut += "</h3></div><br>";
  }
  sOut += "</div>";
  sprintf(tempS, "<br><p>Uptime: %02d:%02d:%02d</p>", hr, mi % 60, sec % 60);
  sOut += tempS;
  sOut+="</body></html>";
  webServer.send(200, "text/html", sOut);
}


void handleNotFound() {
//  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";

  for (uint8_t i = 0; i < webServer.args(); i++) { message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n"; }

  webServer.send(404, "text/plain", message);
}

void setup(void) {
 for(int i = 0; i < 10; i++) {
    tempAll[i][0] = 0.0;
 }
 for(int i = 0; i < 10; i++) {
    addressAll[i][0] = "";
 }
 addressAsName[0][0] = "28e413fd0053";
 addressAsName[1][0] = "28fcf6dd00a6";
 addressAsName[2][0] = "28529ced00cd";
 addressAsName[3][0] = "289ba0fd00ee";
 addressAsName[4][0] = "282737ed00fd";
 addressAsName[0][1] = "Sypialnia";
 addressAsName[1][1] = "Kuchnia";
 addressAsName[2][1] = "Łazienka";
 addressAsName[3][1] = "Korytarz";
 addressAsName[4][1] = "Salon";
 Serial.begin(115200);
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);
 int i = 0;
 while (WiFi.status() != WL_CONNECTED) {
 delay(1000);
 i = i +1;
 if(i%2==0){Serial.print(".");} else { Serial.print("+");}
 }
 Serial.println();
 Serial.println("WiFi connected");
 Serial.println("Server started");
 Serial.print("Use this URL : ");
 Serial.print("http://");
 Serial.print(WiFi.localIP());
 Serial.println("/");
 webServer.on("/", handleRoot);
 webServer.onNotFound(handleNotFound);
 webServer.begin();
 Serial.println("HTTP server started");
}

void loop(void) {
  webServer.handleClient();
  delay(2);//allow the cpu to switch to other tasks
  //odczyt zegara, BME, zapis na karcie
  licznik = 0;
  if((millis() - lastTime) > delayTime){
    
    lastTime = millis();
  }
}
