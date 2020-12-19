// MQTT OLACAK FİREBASE YOK RESET ÇEKİP DURMASIN.
// MQTT İLE MESAJ YOLLAMAYA HAZIR.
// 192.168.2.105
//http://192.168.4.1/ssidset?ssid=Zyxel&pass=abcddede14
//
// esp role alttaki direnci sök.   bir de 10k ile 3v3 ü aşırt 3. bacağına bağla
// bordun üstünden bakarken
//
//  /-10k--\
//|*   *    *    *        5v GND    role çıkışlar
//|               
//|*   *    *    *              
//|
//|    ESP8266
//|
//|     anten                     ROLE
//|_______________                   |
//                                   |
//     5v>3.3v                       |
// GPIO0 = 0      int role=0         |
//       R1\                         |
//          |-base-- Transistor -3v  |
//       R2/           |_____________|
//led
//      R2 yi sök


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
//#include <ESP8266HTTPClient.h>
//#include <ESP8266WebServer.h>
//#include "FS.h"
#include <EEPROM.h>
#include <ArduinoOTA.h>
//#include <FirebaseArduino.h>
//#include <ArduinoJson.h>



//#define FIREBASE_HOST "miot-esp8266.firebaseio.com"
//#define FIREBASE_AUTH "a2Gm6knh69XRDNv1qt10smuIGJl97bYqlaKAIXyj"
//String FIREBASE_HOSTA ="miot-esp8266";
//String FIREBASE_AUTH ="a2Gm6knh69XRDNv1qt10smuIGJl97bYqlaKAIXyj";
String hosttmp;
String anahtartmp;

///// kart pinleri
///// esp8266 esp-01 pins  değerleri || gpio0 5  GPOI0 5. pin için 0  --  GPIO 3. pin için 2 kapıyı açan 2
const int kombirole=0;
int Kombistate=0;

// Set web server port number to 80
WiFiServer server(80);
String lipStr,gipStr,dnsStr,ipStr;
// kart pinleri
// esp8266 esp-01 pins  değerleri gpio0 5  GPOI0 5. pin için 0  -- GPIO 3. pin için 2 kapıyı açan 2
//#define DHTPIN D4  //5 dht pin   3 role için
String myString;
//int vr = A0; // variable resistor connected 
//int sdata = 0; // The variable resistor value will be stored in sdata.
unsigned long wt=0;
int oldwt;
// Uncomment whatever type you're using!

// Variable to store the HTTP request
String header;
int headerset=0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 100;


unsigned long          cxtime;
unsigned long          pxtime = currentTime;
unsigned long          totime = 1000;

//Variables
int i = 0;
int statusCode;
String ssid;
String pass;
String st;
String content;

//tanktan gelen

String esphostname ="ESP_KOMBi_MQTT_ROLE";
String topicim;
String GTopic;
String topicims;
char *datam;
String datamS="";
String OlddatamS="";

String oldwanip;
String wanip;
int rescanwifi=0;
//int s=0;

unsigned long cursayac = 0;
unsigned long prevsayac = 0;
unsigned long mcursayac = 0;
unsigned long mprevsayac = 0;
unsigned long oldtimerx=0;
unsigned long timerxsayac;
String ssidtmp;
String passtmp;


// firebaseye gidecekler old olanlar.
bool gonder=false;
String mydata="";
float usrtemp;
float hum; //humidy                     ******* push
float olsh; //humidy old
float t; // temp c
float f; // temp fahren
float hic; // heat index temp c       ******* push
float hif;
float oldhum;
float oldhic; //
int n = 0;

String localip="";

int oldKombistate=1000;

String firebasedata="78.100.33.20|24.00|53.0|22.20|";
//IPAddress local_ip(192, 168, 4, 2);
//IPAddress gateway(192, 168, 4, 1);
//IPAddress netmask(255, 255, 255, 0);

/*
 * Custom broker class with overwritten callback functions
 */
 int hproto=0;


String mqtt_server = "192.168.2.84";
// char*
WiFiClient espClient;
PubSubClient mclient(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (512)
char msg[MSG_BUFFER_SIZE];
int value = 0;
/*
 * WiFi init stuff
 */
void startWiFiClient()
{

  //Serial.println("Connecting to "+(String)ssid);

  WiFi.begin(ssid,pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
  //  Serial.print(".");
  }
  //Serial.println("");
  
  //Serial.println("WiFi connected");
  //Serial.println("IP address: " + WiFi.localIP().toString());

  
}

//tanktan gelen bitti
//Function Decalration
bool testWifi(void);
void launchWeb(void);
void setupAP(void);
void lookAP(void);
void wifiscan(void);
void arduinoota(void);
void firebaseloop(void);
void wanipsorgula(void);
void httpheader(void);
void connectWifi(void);
void mqttloop(void);
void httpcl(void);
//void buzzercal(void);

//void createWebServer(void);
//Establishing Local server at port 80 whenever required


                            void setup()
                            {
                              pinMode(kombirole, OUTPUT);  // esp kombi rölesi 
                              digitalWrite(kombirole, LOW);
                              Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
                              //Serial.begin(230400); //Initialising if(DEBUG)Serial Monitor
                              Serial.println();
                              Serial.println("Disconnecting current wifi connection");
                              WiFi.disconnect();
                              delay(10);
                              Serial.println();
                              Serial.println();
                              Serial.println("Startup");
                            
                              //---------------------------------------- Read flash for ssid and pass
                              

                              Serial.print((WiFi.status()));

/*
Serial.println("Writing ssid;pass;");
String buf = "Zyxel|bebedede14|";
int str_len = buf.length() + 1;
char char_array[str_len];
buf.toCharArray(char_array, str_len);


for(int i=0;i<buf.length()+1;i++){

  if(char_array[i]=='|'){
        EEPROM.write(i,0);
    }else{
          EEPROM.write(i,char_array[i]);
  
      }

 delay(10);
}
               EEPROM.commit();
                delay(500);   
                content = "{\"Success\":\"saved to file... reset to boot into new wifi\"}";
                statusCode = 200;
                delay(50000);
                ESP.reset();                                        

*/




                            EEPROM.begin(512);
                            //delay(10);
                              String essid = "";
                              String epass = "";
                              String eFIREBASE_HOSTA="";
                              String eFIREBASE_AUTH="";
                              int Stage=0;

                              Serial.println("Reading ssid;pass;");
                              // ssid passdatas
                            for(int i=0;i<99;i++){
                              if(EEPROM.read(i)==0){Stage+=1;}else
                              {
                                if (Stage==0){
                                  essid+=char(EEPROM.read(i));
                                  }
                                if (Stage==1){
                                  epass+=char(EEPROM.read(i));
                                  }
                              }
                            }
                            ssid=essid;
                            pass=epass;
                            // firebase datas
                            Stage=0;
                            for(int i=0;i<180;i++){
                              if(EEPROM.read(i+100)==0){Stage+=1;}else
                              {
                                if (Stage==0){
                                  eFIREBASE_HOSTA+=char(EEPROM.read(i+100));
                                  }

                                  if (Stage==1){
                                  eFIREBASE_AUTH+=char(EEPROM.read(i+100));
                                  }  
                              }
                            }
//                              FIREBASE_HOSTA=eFIREBASE_HOSTA+".firebaseio.com";
//                              FIREBASE_AUTH=eFIREBASE_AUTH;
                            String hprotoc;
                                   hprotoc+=char(EEPROM.read(181));
                            hproto=hprotoc.toInt();

                            Serial.print ("hproto= ");
                            Serial.println(hproto);
                            delay(1000);
                            Serial.println(ssid);
                            Serial.println(pass);
//                            Serial.println(FIREBASE_HOSTA);
//                            Serial.println(FIREBASE_AUTH);


                            
                            String emqtt_server;
                            for(int iw=0;iw<15;iw++){
                                  emqtt_server+=char(EEPROM.read(iw+220));
                                  Serial.print(char(EEPROM.read(iw+220)));
                            }
                            if(emqtt_server.length()>8)mqtt_server=emqtt_server;
                            

                            connectWifi();

                              Serial.println();
                              Serial.println("Web server Lunched.");
                              arduinoota();
                              launchWeb();


  int mqtt_serverLen = mqtt_server.length(); // Get the length
  char *mqttserver = (char *)malloc(mqtt_serverLen+1);
  mqtt_server.toCharArray(mqttserver,mqtt_server.length()+1);
  mclient.setServer(mqttserver, 1883);
  mclient.setCallback(callback);
  
//                              if(hproto<3){
//                              Firebase.begin(FIREBASE_HOSTA, FIREBASE_AUTH);
//                              }



}



void reconnect() {
  // Loop until we're reconnected
  if (mcursayac==0 && !mclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266role-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mclient.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      // Once connected, publish an announcement...
      mclient.subscribe("burole");
      mclient.publish("nerole", "?geldim:hepsi|role");
      mcursayac=0;
      delay(100);
      return;
    } else {
      Kombistate=0;
      digitalWrite(kombirole, LOW);
      Serial.print("failed, rc=");
      Serial.print(mclient.state());
      Serial.println(" try again in 50 mili seconds");
    }
  }
      ArduinoOTA.handle();
    delay(1);
    mcursayac+=1;
    if(mcursayac>400)mcursayac=0;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
datamS+=(char)payload[i];
  }
  Serial.println();
}


void mqttloop(){
    if (!mclient.connected()) {
        reconnect();
        if (!mclient.connected()) {
          return;
          }
  }
  mclient.loop();
}  

int connectcontrol=0;

void loop() {
                ArduinoOTA.handle();
  
             if(wt % 2==0)
             {
              mqttloop();
             }


//cxtime=millis();
//cxtime - pxtime <= totime
cxtime=millis();
if(cxtime-pxtime>totime*60){
pxtime=millis();

IPAddress lipx=WiFi.localIP();
String lipxS = String(lipx[0]);

  
        if (lipxS.toInt()<1 & rescanwifi==0)
        { 
                 Serial.print((WiFi.status() != WL_CONNECTED));
                 connectWifi();
                 Serial.print((WiFi.localIP()));
              
        }

pxtime=millis();
}

  httpcl();
  
            


                   //şart///////////////////////////////////
                   delay(1);wt+=1;
                   if(wt>=4101){wt=1;}
                   if(rescanwifi==1){
                       wifiscan();
                       rescanwifi=0;
                     }



      if(datamS!=""){
        Serial.println(datamS);

              for(int si=0;si<datamS.length();si++){     
        String datamSt=datamS.substring(0,datamS.indexOf("|")+1);
        datamS=datamS.substring(datamSt.length(),datamS.length());         
              if(datamSt.indexOf("Kombistate:")>-1){
                Kombistate=datamSt.substring(datamSt.indexOf(":")+1,datamSt.indexOf("|")).toFloat();
              }

              if(datamSt.indexOf("usrtemp:")>-1){
                usrtemp=datamSt.substring(datamSt.indexOf(":")+1,datamSt.indexOf("|")).toFloat();
              }
              if(datamSt.indexOf("odatemp:")>-1){
                hic=datamSt.substring(datamSt.indexOf(":")+1,datamSt.indexOf("|")).toFloat();
              }
              
              if(datamSt.indexOf("odahum:")>-1){
                hum=datamSt.substring(datamSt.indexOf(":")+1,datamSt.indexOf("|")).toFloat();
              }
              
              Serial.println(datamSt);
              si=datamSt.length();
              if(datamSt.length()<1)break;
              } // for si sonu
      }
    
               
              

          if(wt==101)
          {
  
                if(Kombistate==1){digitalWrite(kombirole, HIGH);}
                if(Kombistate==0){digitalWrite(kombirole, LOW);}
          }


    }

void httpheader(){
                          
                          content = "<!DOCTYPE HTML>\r\n<html>Welcome to main page";
                          content += "<head>";
                          content += "<META charset=iso-8859-9\">";
                          content += "<link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAS0lEQVR42s2SMQ4AIAjE+P+ncSYdasgNXMJgcyIIlVKPIKdvioAXyWBeJmVpqRZKWtj9QWAKZyWll50b8IcL9JUeQF50n28ckyb0ADG8RLwp05YBAAAAAElFTkSuQmCC' type='image/x-png' />";
                          content += "</head>";
                          content += "<br>";  
                          content += "<font size=\"3\" color=\"#0000ff\">"+esphostname+"</font>";  
                          content += "<br>";  
                          content += "<table style=\"border:5px solid black;width:500px\"><tr><td style=\"border:1px solid black;width:250p; align:center; \">";
                          content += "<p><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form></p></td>";
                          content += "<td style=\"border:1px solid black;width:250px;\">"; 
                          content += "<form action=\"/ssidset\" method=\"POST\"><input type=\"submit\" value=\"Wifi Lookap\"></form></td>";
                          
                          content += "<td style=\"border:1px solid black;width:250px\">"; 
                          content += "<form action=\"/firebasecfg\" method=\"POST\"><input type=\"submit\" value=\"firebase config\"></form></td>";
                          
                          content += "<td style=\"border:1px solid black;width:250px\">"; 
                          content += "<form action=\"/mqttipcfg\" method=\"POST\"><input type=\"submit\" value=\" mqtt ip cfg\"></form></td>";
                          
                          content += "<td style=\"border:1px solid black;width:250px\">"; 
                          content += "<form action=\"/hproto\" method=\"POST\"><input type=\"submit\" value=\"Link methods\"></form></td>";

                          content += "</tr></table>";
                          
  }
  
void httpcl(){
  
          WiFiClient client = server.available();
           
         //server kodları
          if (client) {                             // If a new client connects,
          Serial.println("New Client.");          // print a message out in the serial port
          String currentLine = "";                // make a String to hold incoming data from the client
          currentTime = millis();
          previousTime = currentTime;
          while (client.connected() && currentTime - previousTime <= timeoutTime*5) { // loop while the client's connected
            currentTime = millis();         
            if (client.available()) {             // if there's bytes to read from the client,
              char c = client.read();             // read a byte, then
              //Serial.print(c);                    // print it out the serial monitor
              if(headerset==0)header += c;

              if (c=='\r') {
                //headerset=1;
              }
              if (c=='\n') {

                
                Serial.print(header);
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:  /*
/*
                  if(header.indexOf(" /favicon.ico HTTP/")>-1)
                  {
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          header = "";
                          client.stop();
                  }

*/
          
                  if(header.indexOf(" / HTTP/")>-1){

                          httpheader();

                          IPAddress lip=WiFi.localIP();
                          String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
                          IPAddress gip=WiFi.gatewayIP();
                          String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress dns=WiFi.dnsIP();
                          String dnsStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress ip = WiFi.softAPIP();
                          String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                          
                          content += "Wan ip:";
                          content += (oldwanip);
                          content += "<br>";
                          content += "Local IP : ";
                          content += lipStr;
                          content += "<br>";

                          content += "Local Gateway : ";
                          content += gipStr;
                          content += "<br>";
                          
                          content += "Access IP : ";                         
                          content += ipStr;
                          content += "<p>";
                          content += "<br>"; 
                          content += "<p>";
                          content += "<br>"; 
                          if(Kombistate==1){content += " Kombi [&] <br>";}
                          if(Kombistate==0){content += " Kombi [_] <br>";}
                          if(Kombistate==3){content += " Kombi [_] &nbsp;&nbsp;&nbsp; Role_Esp offline <br>";}
                          content += "istenen temp:&nbsp;";
                          content += String(usrtemp) +"<br>"; 
                          content += ((" Temperature: "));
                          content += " hic:" + (String)hic; // firebasede oldhic
                          content += ((" &#8451;"));
                          content += (("/"));
                          content += (String((1.8*hic)+32));
                          content += ((" &#8457;"));
                          content += "<br>"; 
                          content += (("Humidity: "));
                          content += " hum:" + (String)hum;
                          content += "% <br>"; 
                          content += "<br>"; 
                          content += "<br>"; 
                          content += "</html>";
                          client.println(content);
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          client.println();
                          // Clear the header variable
                          header = "";
                          headerset=1;
                          //client.stop();
                        }



                        
                  if(header.indexOf(" /ssidset")>-1){
                          httpheader();

                          IPAddress lip=WiFi.localIP();
                          String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
                          IPAddress gip=WiFi.gatewayIP();
                          String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress dns=WiFi.dnsIP();
                          String dnsStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress ip = WiFi.softAPIP();
                          String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

                          content += "<table style=\"border:2px solid green;width:500px\"><tr><td style=\"border:1px solid black;width:250px\">";

                          content += "informations";
                          content += "<br>";
                          content += "<br>";
                          content += "Wan ip:";
                          content += (oldwanip);
                          content += "<br>";
                          content += "Local IP : ";
                          content += lipStr;
                          content += "<br>";
                          content += "Local Gateway : ";
                          content += gipStr;
                          content += "<br>";
                          content += "Access IP : ";                         
                          content += ipStr;
                          content += "</td><td style=\"border:1px solid black;width:250px\">";
                          content += "<form action=\"/ssidsetscanwifi\" method=\"POST\"><input type=\"submit\" value=\"Scan Wifi\"></form>";
                          if(header.indexOf("scanwifi HTTP")>-1){   
                          rescanwifi=1;
                          }
                          content+= "<br>Click Scan Wifi Button 10 second waiting.<br>";
                          content+= "<br>Click Result Button get APs infos.<br>";
                          content += "<form action=\"/ssidset\" method=\"POST\"><input type=\"submit\" value=\"Result Wifi Scan\"></form>";
                          content+="</td></tr></table>";
                          content += "<p>";
                          content += "<table style=\"border:2px solid black;width:500px\"><tr><td style=\"border:1px solid black;width:500px\">";
                          content += "Scanned WiFi Devices</td><tr><td>";
                          content += "<br>";  
                          content += st;
                          content += "</td><tr></table>"; 
                          content += "<font size=\"4\" color=\"#aa0000\">Gonder'e bastiktan sonra bilgiler dogru ise modeme baglanirsiniz</font>";
                          content += "<form method='get' action='ssidset'><label>SSID: </label><input name='ssid' length=32><label>PASS: </label><input name='pass' length=64><input type='submit'></form>";
                          content += "<br>";
                          content+="<br><br>";
                          content += "</html>";
                        }

                  if(header.indexOf("/ssidset?")>-1){ //   "/setting?ssid=Zyxel&pass=bebedede14"
                         // yazma kısmı
                           //Serial.println(header);
                           if(header.indexOf("ssid=")>-1){
                            ssidtmp= header.substring((header.indexOf("ssid=")+5),(header.indexOf("&")));
                            Serial.println(ssidtmp);
                            }
                           if(header.indexOf("&pass=")>-1){
                            passtmp= header.substring((header.indexOf("&pass=")+6),(header.indexOf(" HTTP")));
                            Serial.println(passtmp);
                            String buf = ssidtmp + '|' + passtmp + '|';

                                            //---------------------------------------- Write flash for ssid and pass
                                                  Serial.println("Write flash file ssid;pass;");

int str_len = buf.length() + 1;
char char_array[str_len];
buf.toCharArray(char_array, str_len);


for(int i=0;i<buf.length()+1;i++){

  if(char_array[i]=='|'){
        EEPROM.write(i,0);
    }else{
          EEPROM.write(i,char_array[i]);
  
      }

}
               EEPROM.commit();
                content = "{\"Success\":\"saved to file... reset to boot into new wifi\"}";
                statusCode = 200;
                delay(500);
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();
                client.println();
                            client.println("Reseting ESP ... waiting 10 seconds");
                            client.println("<br><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form><br>");
                            client.println("<form action=\"/ssidset\" method=\"POST\"><input type=\"submit\" value=\"firebase config\"></form>");
                // Clear the header variable
                client.println(content);
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();
                delay(500);
                header = "";
                //client.stop();
                ESP.reset();                                        
}

                client.println(content);
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();
                header = "";
                //client.stop();
              }


              ///////////////////////////////////////////////////////////////////////
//               EEPROM.write(200,protokol bne ise onu yazacağız. okuma yukarıya yazılmalı;
//               EEPROM.commit();
              ///////////////////////////////////////////////////////////////////////
              
 
                         if(header.indexOf(" /firebasecfg")>-1){
                          httpheader();

                          IPAddress lip=WiFi.localIP();
                          String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
                          IPAddress gip=WiFi.gatewayIP();
                          String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress dns=WiFi.dnsIP();
                          String dnsStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress ip = WiFi.softAPIP();
                          String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                          
                          content += "Wan ip:";
                          content += (oldwanip);
                          content += "<br>";
                          content += "Local IP : ";
                          content += lipStr;
                          content += "<br>";

                          content += "Local Gateway : ";
                          content += gipStr;
                          content += "<br>";
                          
                          content += "Access IP : ";                         
                          content += ipStr;
                          content += "<p>";
                          content += "<br>";
                          content += "<br>";
                          content += "realtime database adi tamamla &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Projenizin Gizli Anahtarini kopyala yapistir.";
                          content += "</p><form method='get' action='firebaseset'><label>HOST: https://</label><input name='host' length=32><label>.firebaseio.com/ AUTH:</label><input name='anahtar' length=64><input type='submit'></form>";
                          content += "</html>";

                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          client.println();
                          client.println(content);
                          // Clear the header variable
                          //client.stop();
                        }

                         if(header.indexOf(" /firebaseset")>-1){ //   "/setting?ssid=Zyxel&pass=bebedede14"
                         // yazma kısmı
                           //Serial.println(header);
                           if(header.indexOf("host=")>-1){
                            hosttmp= header.substring((header.indexOf("host=")+5),(header.indexOf("&")));
                            Serial.println(hosttmp);
                            }
                           if(header.indexOf("&anahtar=")>-1){
                            anahtartmp= header.substring((header.indexOf("&anahtar=")+9),(header.indexOf(" HTTP")));
                            Serial.println(anahtartmp);
                            String buf = hosttmp + '|' + anahtartmp + '|';
                                            //---------------------------------------- Write flash for ssid and pass
                                                  Serial.println("Write flash file firebase settings;");

int str_len = buf.length() + 1;
Serial.println(buf);
char char_array[str_len];
buf.toCharArray(char_array, str_len);


for(int i=0;i<buf.length()+1;i++){
Serial.print(char_array[i]);
  if(char_array[i]=='|'){
        EEPROM.write(i+100,0);
    }else{
          EEPROM.write(i+100,char_array[i]);
  
      }

}
               EEPROM.commit();
                content = "{\"Success\":\"saved to file... reset to boot into new wifi\"}";
                statusCode = 200;


                            client.println("Reseting ESP ... waiting 10 seconds");
                            client.println("<br><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form><br>");
                            client.println("<form action=\"/firebasecfg\" method=\"POST\"><input type=\"submit\" value=\"firebase config\"></form>");
                // Clear the header variable
                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();
                header = "";
                delay(500);
                //client.stop();
                
                delay(500);
                ESP.reset();                                        
}
                          header = "";
}




                         if(header.indexOf(" /mqttipcfg")>-1){
                          httpheader();

                          IPAddress lip=WiFi.localIP();
                          String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
                          IPAddress gip=WiFi.gatewayIP();
                          String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress dns=WiFi.dnsIP();
                          String dnsStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress ip = WiFi.softAPIP();
                          String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                          
                          content += "Wan ip:";
                          content += (oldwanip);
                          content += "<br>";
                          content += "Local IP : ";
                          content += lipStr;
                          content += "<br>";

                          content += "Local Gateway : ";
                          content += gipStr;
                          content += "<br>";
                          
                          content += "Access IP : ";                         
                          content += ipStr;
                          content += "<p>";
                          content += "<br>";
                          content += "<br>";
                          content += "realtime database adi tamamla &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; Projenizin Gizli Anahtarini kopyala yapistir.";
                          content += "</p><form method='get' action='mqttipset'><label>mqtt ip :</label><input name='mqttip' length=15><label>mqtt pwd :</label><input name='mqttpwd' length=6><input type='submit'></form>";
//epromda 231 inci byte dahil kullanıldı
                          content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>";
                          content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br></body>";
                          content += "</html>";
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          client.println();
                          client.println(content);
                          // Clear the header variable
                          //client.stop();
                        }
                          String mqttiptmp="";
                         if(header.indexOf(" /mqttipset")>-1){ //   "/setting?ssid=Zyxel&pass=bebedede14"
                         // yazma kısmı
                           //Serial.println(header);
                           if(header.indexOf("mqttip=")>-1){
                            mqttiptmp= header.substring((header.indexOf("mqttip=")+7),(header.indexOf("&")));
                            Serial.println(mqttiptmp);
                            }
                            String buf = mqttiptmp + '|';
                                            //---------------------------------------- Write flash for ssid and pass
                                                  Serial.println("Write flash file firebase settings;");

int str_len = buf.length() + 1;
Serial.println(buf);
char char_array[str_len];
buf.toCharArray(char_array, str_len);


for(int i=0;i<buf.length()+1;i++){
Serial.print(char_array[i]);
  if(char_array[i]=='|'){
        EEPROM.write(i+220,0);
    }else{
          EEPROM.write(i+220,char_array[i]);
      }

}
               EEPROM.commit();
                content = "{\"Success\":\"saved to file... reset to boot into new wifi\"}";
                statusCode = 200;

                client.println("HTTP/1.1 200 OK");
                client.println("Content-type:text/html");
                client.println("Connection: close");
                client.println();
                            client.println("Reseting ESP ... waiting 10 seconds");
                            client.println("<br><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form><br>");
                            client.println("<form action=\"/mqttipcfg\" method=\"POST\"><input type=\"submit\" value=\"mqtt ip config\"></form>");
                // Clear the header variable
                header = "";
                //client.stop();
                
                delay(50);
                ESP.reset();                                        
}







                  if(header.indexOf("/info")>-1){
                            // firebase datas eeprom
                            int Stage=0;
                               String eFIREBASE_HOSTA="";//+=char(EEPROM.read(i+100));
                               String eFIREBASE_AUTH="";//+=char(EEPROM.read(i+100));
                            for(int i=0;i<180;i++){
                              if(EEPROM.read(i+100)==0){Stage+=1;}else
                              {
                                if (Stage==0){
                                    eFIREBASE_HOSTA+=char(EEPROM.read(i+100));
                                  }

                                  if (Stage==1){
                                    eFIREBASE_AUTH+=char(EEPROM.read(i+100));
                                  }  
                              }
                            }

                          httpheader();

                          IPAddress lip=WiFi.localIP();
                          String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
                          IPAddress gip=WiFi.gatewayIP();
                          String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress dns=WiFi.dnsIP();
                          String dnsStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
                          IPAddress ip = WiFi.softAPIP();
                          String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
                          
                          content += "Wan ip:";
                          content += (oldwanip);
                          content += "<br>";
                          content += "Local IP : ";
                          content += lipStr;
                          content += "<br>";

                          content += "Local Gateway : ";
                          content += gipStr;
                          content += "<br>";
                          
                          content += "Access IP : ";                         
                          content += ipStr;
                          content += "<p>";
                          content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"Scan Wifi\"></form>";
                          content += "<br>"; 
                          content += "<p>";
                          content += "<br>"; 
                          if(Kombistate==1){content += " Kombi [&] <br>";}
                          if(Kombistate==0){content += " Kombi [_] <br>";}
                          if(Kombistate==3){content += " Kombi [_] &nbsp;&nbsp;&nbsp; Kombi role offline <br>";}
                          content += "istenen temp:&nbsp;";
                          content += String(usrtemp) +"<br>"; 
                          content += ((" Temperature: "));
                          content += "firebase hic:" + String(hic);
                          content += ((" &#8451;"));
                          content += (("/"));
                          content += (String((1.8*hic)+32));
                          content += ((" &#8457;"));
                          content += "<br>"; 
                          content += (("Humidity: "));
                          content += "firebase hum:" + String(hum);
                          content += "% <br>"; 
                          content += "<br>"; 
                          content += "<br>";
                          content += eFIREBASE_HOSTA;
                          content += "<br>";
                          content += eFIREBASE_AUTH;  
                          content += "<br>"; 
                          content += "<br>"; 
                          content += "mqtt server ip :"; 
                          content += mqtt_server;
                          content += "<br>"; 
                          content += "<br>"; 
                           
                          content += "</html>";
                          client.println(content);
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          client.println();
                          delay(500);
                          // Clear the header variable
                          header = "";
                          //client.stop();
                  }

                  if(header.indexOf("/hproto")>-1){
                            // firebase datas eeprom
                            int Stage=0;
                          httpheader();
                          content += "<table style=\"border:2px solid green;width:500px\"><tr><td style=\"border:1px solid black;width:250px\">";
                          content += "<td style=\"border:1px solid black;width:250px\">";
                          content += "Haberlesme protokolleri secimi";                          
                          content += "<form action=\"/hprotoset\">";
                          content += "<p>Please select connection protocol</p>";
                          
                          content +="<input type=\"radio\" id=\"MQTT\" name=\"hproto\" value=\"1\""; 
                          if(hproto==1){content += " checked=\"true\">";} else {content += ">";}
                          content +="<label for=\"hproto\">MQTT</label><br>";
                          
                          content +="<input type=\"radio\" id=\"MQTT\" name=\"hproto\" value=\"2\"";
                          if(hproto==2) {content += " checked=\"true\">";} else {content += ">";}
                          content+="<label for=\"hproto\">FiREBASE SERVER</label><br>";

                          content+="<br><br>";
                          content+="<input type=\"submit\" value=\"Submit\">";
                          content+="</form>";
                          content+="</td><td style=\"border:1px solid black;width:250px\">";
                          content +=" mqtt icin baglanti soyle boyle<br< boyle filan falan";
                          content += "</td></tr></table>";
                          content += "<br>";
                          content += "</html>";




                  if(header.indexOf(" /hprotoset")>-1){ 
                  
                   Serial.println(header);
                            if(header.indexOf("hproto=")>-1){
                            String buf= header.substring((header.indexOf("hproto=")+7),(header.indexOf(" HTTP")));
                            int str_len = buf.length() + 1;
                            Serial.println(buf);
                            char char_array[str_len];
                            buf.toCharArray(char_array, str_len);
                            
                            
                            for(int i=0;i<buf.length()+1;i++){
                            Serial.print(char_array[i]);
                              if(char_array[i]=='|'){
                                    EEPROM.write(i+181,0);
                                }else{
                                      EEPROM.write(i+181,char_array[i]);
                              
                                  }
                            
                            }

                            EEPROM.commit();
                            content = "{\"Success\":\"saved to file... reset to boot into new wifi\"}";
                            statusCode = 200;
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-type:text/html");
                            client.println("Connection: close");
                            client.println();
                            client.println("Reseting ESP ... waiting 10 seconds");
                            client.println("<br><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form><br>");
                            client.println("<br><form action=\"/hproto\" method=\"POST\"><input type=\"submit\" value=\"Communication protocols\"></form>");
                            // Clear the header variable
                          client.println(content);
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          client.println();
                          // Clear the header variable
                          header = "";
                          //client.stop();
                          delay(500);
                          ESP.reset();  
                  }
                  }


// kapanış html kısmı
                          client.println(content);
                          client.println("HTTP/1.1 200 OK");
                          client.println("Content-type:text/html");
                          client.println("Connection: close");
                          client.println();
                          // Clear the header variable
                          header = "";
                          //client.stop();
                  }
                                if (currentLine.length() == 0) {
                                  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                                  // and a content-type so the client knows what's coming, then a blank line:
                                  client.println("HTTP/1.1 200 OK");
                                  client.println("Content-type:text/html");
                                  client.println("Connection: close");
                                  client.println();
                                  client.println(content);
                                  // Clear the header variable
                                  header = "";
                                  delay(500);
                                  headerset=0;
                                  // Close the connection
                                  //client.stop();
                                  //break;
                      
                                } else { // if you got a newline, then clear currentLine
                                  currentLine = "";
                                }

}

}
}

            client.println(content);
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Clear the header variable
            header = "";
            // Close the connection
            delay(500);
            //client.stop();

            Serial.println("Client disconnected.");
            Serial.println("");  

  }
}


                                      void wanipsorgula(void)
                                      {  // kombiye bağlı esp içinde çalışacak
                                        timerxsayac=millis();
                                        if((int(timerxsayac/60000))%2==1 & oldtimerx<timerxsayac)
                                        {
                                        oldtimerx=timerxsayac+60000;
                                              WiFiClient clienthttp;
                                              if (clienthttp.connect("api.ipify.org", 80)) 
                                              {
                                                  Serial.println("connected");
                                                  clienthttp.println("GET / HTTP/1.0");
                                                  clienthttp.println("Host: api.ipify.org");
                                                  clienthttp.println();
                                              } else {
                                                  Serial.println("Connection to ipify.org failed");
                                            
                                              }
                                              String line;
                                              while(clienthttp.available())
                                              {
                                                line = clienthttp.readStringUntil('\n');
                                                wanip=line;
                                              }
                                                Serial.println(wanip);
                                        
                                      }
                                      }
                                                                            


//-------------OTA OTA OTA START---------------------------------- 
void arduinoota(void){
   //OTA ayarla
  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  }
//---------------------- OTA FİNİSH --------------------------------------




                  
                                      bool testWifi(void)
                                      {
                                        int c = 0;
                                        Serial.println("Waiting for Wifi to connect");
                                        while ( c < 6 ) {
                                          if (WiFi.status() == WL_CONNECTED)
                                          {
                                            return true;
                                          }
                                          delay(1000);
                                          Serial.print("*");
                                          c++;
                                        }
                                        Serial.println("");
                                        Serial.println("Connect timed out, opening AP");
                                        return false;
                                      }


void wifiscan(void)
{
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    st = "[ "+ String(n) +" ] networks found<br>";
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  //st = "<ol>";
  st += "<ol>";
  for (int i = 0; i < n; ++i)
  {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol><br>";
  
 }



                                void lookAP(void)
                                {
                                  WiFi.mode(WIFI_STA);
                                  WiFi.disconnect();
                                  int n = WiFi.scanNetworks();
                                  Serial.println("scan done");
                                  if (n == 0)
                                    Serial.println("no networks found");
                                  else
                                  {
                                    Serial.print(n);
                                    Serial.println(" networks found");
                                    for (int i = 0; i < n; ++i)
                                    {
                                      // Print SSID and RSSI for each network found
                                      Serial.print(i + 1);
                                      Serial.print(": ");
                                      Serial.print(WiFi.SSID(i));
                                      Serial.print(" (");
                                      Serial.print(WiFi.RSSI(i));
                                      Serial.print(")");
                                      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
                                    }
                                  }
                                  Serial.println("");
                                  st = "<ol>";
                                  for (int i = 0; i < n; ++i)
                                  {
                                    // Print SSID and RSSI for each network found
                                    st += "<li>";
                                    st += WiFi.SSID(i);
                                    st += " (";
                                    st += WiFi.RSSI(i);
                                
                                    st += ")";
                                    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
                                    st += "</li>";
                                  }
                                  st += "</ol>";
                                  Serial.println("Initializing_softap_for_wifi credentials_modification");
                                  Serial.println("over");
                                }


void launchWeb()
{
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  // Start the server
  server.begin();
  Serial.println("Server started");
}

                                void connectWifi(void)
                                {
                                    WiFi.mode(WIFI_AP_STA); /*1*/ //ESP8266 works in both AP mode and station mode
                                     //WiFi.mode(WIFI_STA); /*2*/  // ESP8266 works in station mode
                                   // WiFi.begin(ssid, password); // given the network
                                  
                                  //    Serial.print(ssid);
                                  //    Serial.print("connecting to ");
                                  //    while (WiFi.status() != WL_CONNECTED) {
                                  //      // not connected to the network
                                  //    delay(500);
                                  //    Serial.print(".");
                                  //  }
                                    WiFi.hostname(esphostname);               
                                    WiFi.begin(ssid, pass);
                                    Serial.println(ssid);
                                    Serial.println(pass);
                                    //delay(1000);
                                    if (testWifi())
                                    {
                                      Serial.println("Succesfully Connected!!!");
                                      Serial.println(WiFi.localIP());
                                      Serial.println(WiFi.gatewayIP());
                                      WiFi.softAP(esphostname, "bebedede14"); // bağlanınca ap kalksın için // koyabiliriz.
                                      
                                
                                      Serial.println();
                                    Serial.println("Web server Lunched.");

                                    }
                                    else
                                    {
                                      WiFi.hostname(esphostname);    
                                      Serial.println("Turning the HotSpot On");
                                //                                wifiscan();
                                //                                lookAP();// Setup HotSpot
                                      WiFi.softAP(esphostname, "12345678");
                                      Serial.println(WiFi.localIP());
                                      Serial.println(WiFi.gatewayIP());

                                    }
                                    }
