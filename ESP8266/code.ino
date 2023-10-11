#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <string.h>

const char* hostName = "NodeMCU_ESP8266";//This is the hostname for nodemcu ota.
const char* server_path="";//Here will be the url for the webserver
const char* ssid = "";//Name of your wifi network
const char* password = "";//Password of the wifi
int p0=0,p1=1,p2=1,p3=1,p4=0,p5=1,p6=1;
const uint16_t kRecvPin = D7;//This pin is for the IR reciver.
int indicator=D8;//This pin is for the indicator led.
int ota_first=0;//This is part of OTA system.
int sync_first=0;//This is use for the first sync when the nodemcu will get the states and sync with the server.
int synced=0;//This is for if the pin states are synced or not, if synced then it will be 1 else 0
int wifi_state=0;//This will tell the current wifi state if it is 1 that's mean the nodemcu is connected to a wifi else if 0 then it is diconnected.
int remote_wifi=1;//If it is 0 then thats mean it is in sleep mode for wifi.
ESP8266WebServer server(80);
IRrecv irrecv(kRecvPin);
decode_results results;
// unsigned long startTime;

int OTA=0;
int IR=0;
int SP=0;//This is for the serial print.
int antibug=0;

void println(String text)
{
  if(antibug==1)
  {
    String tmp="?massage=";
    http_request(server_path+tmp+text);
  }
  else
  {
    if(SP==1)
    {
      Serial.println(text);
    }
  }
}
void mainpage()
{
   String response = "<html><body style='margin:0;'><iframe style='width:100%;height:100%;position:absolute;top:0;left:0;right:0;bottom:0;' src='http://192.168.43.1:8006/asnodemcu/'></iframe></body></html>";
   show(response);
}
void show(String response)
{
   server.send(200, "text/html", response);
   command();
}
void show_state()
{
  String response = String(p0)+String(p1)+String(p2)+String(p3)+","+String(p4)+","+String(p5)+","+String(p6);
  show(response);
}

void ota0()
{
  show("OTA TURNED OFF");
  activate_ota(0);
}
void ota1()
{
  activate_ota(1);
  show("OTA TURNED ON");
}

void activate_ota(int state)
{
  if(state==1)
    OTA=1;
  else
  {
    OTA=0;
    ESP.restart();
  }
}

void debug0()
{
  antibug=0;
  show("0");
}
void debug1()
{
  antibug=1;
  show("1");
}

void serial0()
{
  SP=0;
  show("0");
}
void serial1()
{
  SP=1;
  show("1");
}

void try_sync()
{
  sync(0);
}

String http_request(String url)
{
  if(WiFi.status() == WL_CONNECTED)
  {
  WiFiClient client;
  HTTPClient http;
    
    http.begin(client,url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) 
    {
      String response = http.getString();
      return response;
    }
    else
    {
      return "-1";
    }
    http.end();
  }
  else
    return "-1W0";
}
int sync(int mode)
{
  if(mode==0)//It will sync the nodemcu with the server.
  {
    String tmp="?state=1";
    String data=http_request(server_path+tmp);
    println(data);
    if(data.length()==7)
    {
      char state[7];
      data.toCharArray(state, 7);
      if(state[0]=='1')
        pin_mode_set(D0,HIGH);
      if(state[1]=='1')
        pin_mode_set(D1,HIGH);
      if(state[2]=='1')
        pin_mode_set(D2,HIGH);
      if(state[3]=='1')
        pin_mode_set(D3,HIGH);
      if(state[4]=='1')
        pin_mode_set(D4,HIGH);
      if(state[5]=='1')
        pin_mode_set(D5,HIGH);
      if(state[6]=='1')
        pin_mode_set(D6,HIGH);
      return 1;
    }
    else
      return 0;
  }
  else//It will sync the server with nodemcu
  {
    String tmp="?set="+String(digitalRead(D0))+String(digitalRead(D1))+String(digitalRead(D2))+String(digitalRead(D3))+String(digitalRead(D4))+String(digitalRead(D5))+String(digitalRead(D6));
    if(http_request(server_path+tmp)=="1")
      return 1;
    else
      return 0;
  }
}

void pin_mode_toogle(int pin)
{
  if(digitalRead(pin)==HIGH)
    pin_mode_set(pin,LOW);
  else if(digitalRead(pin)==LOW)
    pin_mode_set(pin,HIGH);
}
void pin_mode_set(int pin,int mode)
{
  if(digitalRead(pin)!=mode)
  {
    digitalWrite(pin,mode);
    synced=0;
  }
}
void p00()
{
  pin_mode_set(D0,LOW);
  show("1");
}
void p01()
{
  pin_mode_set(D0,HIGH);
  show("1");
}
void p10()
{
  pin_mode_set(D1,LOW);
  show("1");
}
void p11()
{
  pin_mode_set(D1,HIGH);
  show("1");
}
void p20()
{
  pin_mode_set(D2,LOW);
  show("1");
}
void p21()
{
  pin_mode_set(D2,HIGH);
  show("1");
}
void p30()
{
  pin_mode_set(D3,LOW);
  show("1");
}
void p31()
{
  pin_mode_set(D3,HIGH);
  show("1");
}
void p40()
{
  pin_mode_set(D4,LOW);
  show("1");
}
void p41()
{
  pin_mode_set(D4,HIGH);
  show("1");
}
void p50()
{
  pin_mode_set(D5,LOW);
  show("1");
}
void p51()
{
  pin_mode_set(D5,HIGH);
  show("1");
}
void p60()
{
  pin_mode_set(D6,LOW);
  show("1");
}
void p61()
{
  pin_mode_set(D6,HIGH);
  show("1");
}

void connecting()
{
    digitalWrite(indicator,HIGH);
    delay(200);
    digitalWrite(indicator,LOW);
    delay(300);
}
void command()
{
    digitalWrite(indicator,HIGH);
    delay(75);
    digitalWrite(indicator,LOW);
    delay(75);
}

void setup() {
  // pinMode(D0,OUTPUT);
  pinMode(D1,OUTPUT);
  pinMode(D2,OUTPUT);
  pinMode(D3,OUTPUT);
  // pinMode(D4,OUTPUT);
  pinMode(D5,OUTPUT);
  pinMode(D6,OUTPUT);
  pinMode(D8,OUTPUT);
  // digitalWrite(D0,LOW);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  digitalWrite(D3,LOW);
  // digitalWrite(D4,LOW);
  digitalWrite(D5,LOW);
  digitalWrite(D6,LOW);
  digitalWrite(D8,LOW);//It use as indicator
  
  if(SP==1)
  {
    Serial.begin(115200);
    delay(10);
  }

  String tmp="Connecting to ";
  println(tmp+ssid);

  irrecv.enableIRIn(); 
  WiFi.begin(ssid, password);
  
  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    connecting();
    if(c>10)
    {
      digitalWrite(indicator,HIGH);
      break;
    }
    c++;
  }
  if(sync(0)==1)
  {
    synced=1;
    sync_first=1;
  }

  server.on("/",mainpage);
  server.on("/d00",p00);
  server.on("/d01",p01);
  server.on("/d10",p10);
  server.on("/d11",p11);
  server.on("/d20",p20);
  server.on("/d21",p21);
  server.on("/d30",p30);
  server.on("/d31",p31);
  server.on("/d40",p40);
  server.on("/d41",p41);
  server.on("/d50",p50);
  server.on("/d51",p51);
  server.on("/d60",p60);
  server.on("/d61",p61);
  server.on("/state",show_state);
  server.on("/ota1",ota1);
  server.on("/ota0",ota0);
  server.on("/debug0",debug0);
  server.on("/debug1",debug1);
  server.on("/serial0",serial0);
  server.on("/serial1",serial1);
  // server.on("/serial1",serial1);
  server.begin();
}

int errorsync=0;
void loop() {
  if(WiFi.status() != WL_CONNECTED)
  {
    //Here will be some methode to 
    if(remote_wifi==1)
      digitalWrite(indicator,HIGH);
    wifi_state=0;
  }
  else
  {
    wifi_state=1;
    digitalWrite(indicator,LOW);
    //HTTP SYSTEM
    server.handleClient();
    if(synced==0)
    {
      if(sync_first==0)
      {
        if(sync(0)==1)
        {
          synced=1;
          sync_first=1;
          errorsync=0;
        }
        else
        {
          errorsync++;
          if(errorsync>20)
            synced=1;
        }
      }
      else
      {
        if(sync(1)==1)
        {
          synced=1;
          errorsync=0;
        }
        else
        {
          errorsync++;
          if(errorsync>20)
            synced=1;
        }
      }
      digitalWrite(indicator,HIGH);
      delay(30);
      digitalWrite(indicator,LOW);
      delay(20);
    }
  }

  //IR SYSTEM
  if (irrecv.decode(&results)) 
  {
    int receivedValue = static_cast<int>(results.value);
    if(receivedValue==-2134908642||receivedValue==16714485)
    {
      if(p0==1)
        pin_mode_toogle(D0);
      command();
    }
    else if(receivedValue==-2134936692||receivedValue==16752735||receivedValue==33464415)
    {
      if(p1==1)
        pin_mode_toogle(D1);
      command();
    }
    else if(receivedValue==-2134942812||receivedValue==16728255||receivedValue==33427695)
    {
      if(p2==1)
        pin_mode_toogle(D2);
      command();
    }
    else if(receivedValue==-2134924452||receivedValue==16722645||receivedValue==33478695)
    {
      if(p3==1)
        pin_mode_toogle(D3);
      command();
    }
    else if(receivedValue==-2134937202||receivedValue==16767015)
    {
      if(p4==1)
        pin_mode_toogle(D4);
      command();
    }
    else if(receivedValue==-2134961682||receivedValue==16730295||receivedValue==33460335)
    {
      if(p5==1)
        pin_mode_toogle(D5);
      command();
    }
    else if(receivedValue==-2134904052||receivedValue==16716015)
    {
      if(p6==1)
        pin_mode_toogle(D6);
      command();
    }
    else if(receivedValue==-2134949442||receivedValue==16718565)//This is for turn off all the things.
    {
      if(p0==1)
        pin_mode_set(D0,LOW);
      if(p1==1)
        pin_mode_set(D1,LOW);
      if(p2==1)
        pin_mode_set(D2,LOW);
      if(p3==1)
        pin_mode_set(D3,LOW);
      if(p4==1)
        pin_mode_set(D4,LOW);
      if(p5==1)
        pin_mode_set(D5,LOW);
      if(p6==1)
        pin_mode_set(D6,LOW);
      command();
    }

    else if(receivedValue==-2134950972)//This is for restart the nodemcu
    {
      command();
      ESP.restart();
    }
    else if(receivedValue==-2134938732||receivedValue==16726215)//This is for turn off the wifi
    {
      if(remote_wifi==0)
      {
        int tmp1=0;
        for(tmp1=0;tmp1<15;tmp1++)
          command();
        WiFi.mode(WIFI_STA);
        delay(2000);
        WiFi.begin(ssid, password);
        remote_wifi=1;
      }
      else if(WiFi.getMode() != WIFI_OFF)
      {
        command();
        WiFi.disconnect();
        WiFi.mode(WIFI_OFF);
        wifi_state=0;
        remote_wifi=0;
        for(int i=0;i<5;i++)
          connecting();
      }
    }
    if(receivedValue!=-1)
      println(String(receivedValue));
    irrecv.resume();
  }

  //OTA UPDATE SYSTEM
  if(OTA==1)
  {
    if(ota_first==0)
    {
      ArduinoOTA.setHostname(hostName);
      ArduinoOTA.begin();
      ota_first=1;
    }
    else
    {
      ArduinoOTA.handle();
      command();
    }
  }
}
