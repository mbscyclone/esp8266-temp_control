// enson 192.168.2.84 e gönderildi
//165 ya da 84
#include <Time.h>
// espwifiden önce firebaseESP8266.h tanımlanmalı sonra espwifi olacak
#include "FirebaseESP8266.h"
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "uMQTTBroker.h"
//#include <ESP8266HTTPClient.h>
//#include <ESP8266WebServer.h>
//#include "FS.h"
#include <EEPROM.h>
#include <ArduinoOTA.h>
//#include <FirebaseArduino.h>
#include <DHT.h>
//#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(D4, D3, D2, D1, D0);
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#define LOGO8x16_GLCD_HEIGHT 8
#define LOGO8x16_GLCD_WIDTH  16

static const unsigned char PROGMEM termlogo_logo8x8_glcd_bmp[] =
{ B00010000,
  B00101000,
  B00101000,
  B00111000,
  B00111000,
  B01111100,
  B01111100,
  B00111000
};

static const unsigned char PROGMEM damla_logo8x8_glcd_bmp[] =
{ B00000000,
  B00000100,
  B00001100,
  B00010110,
  B00100001,
  B00111001,
  B00111010,
  B00011100
};


static const unsigned char PROGMEM Lan_on_logo8x16_glcd_bmp[] =
{ B00000000, B00000000,
  B00000000, B00000110,
  B00000000, B00100110,
  B00000010, B01100110,
  B00100110, B01100110,
  B01100110, B01100110,
  B11111111, B11111111,
  B00000000, B00000000
};

static const unsigned char PROGMEM Lan_off_logo8x16_glcd_bmp[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B11001100, B00000000,
  B00110000, B00000000,
  B00110000, B00000000,
  B11001100, B00000000,
  B11111111, B11111111,
  B00000000, B00000000
};

static const unsigned char PROGMEM on_logo16_glcd_bmp[] =
{ B01111111, B11111110,
  B10000000, B00000001,
  B10011111, B11111001,
  B10100100, B10001101,
  B10100100, B10001101,
  B10100100, B10001101,
  B10100100, B10001101,
  B10101101, B10011101,
  B10011111, B11111001,
  B10000001, B10000001,
  B10000011, B01100001,
  B10001011, B01101001,
  B10001111, B11111001,
  B10000111, B11100001,
  B10000000, B00000001,
  B01111111, B11111110
};


static const unsigned char PROGMEM off_logo16_glcd_bmp[] =
{ B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000
};

String FIREBASE_HOSTA = "miot-espXY.YY.YY";  // change thiis
String FIREBASE_AUTH = "!22Gm6knh69XRDNv1qt10smuIGJl97bYqlaKAIXyj";  // change this
String hosttmp;
String anahtartmp;

//Define FirebaseESP8266 data object
FirebaseData firebaseData;
FirebaseJson json;



// Set web server port number to 80
WiFiServer server(80);
String lipStr, gipStr, dnsStr, ipStr;
// kart pinleri
// esp8266 esp-01 pins  değerleri gpio0 5  GPOI0 5. pin için 0  -- GPIO 3. pin için 2 kapıyı açan 2
#define DHTPIN D6  //5 dht pin   3 role için

String myString;
int vr = A0; // variable resistor connected
int sdata = 0; // The variable resistor value will be stored in sdata.
unsigned long wt = 0;
int oldwt;
// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
int buzzPin = D5;
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);

// Variable to store the HTTP request
String header;
int headerset = 0;

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
bool WiFiAP = true;      // Do yo want the ESP as AP?
String esphostname = "ESP-TERMOSTAT";
String topicim;
String GTopic;
String topicims;

char *datam;
String datamS = "";
String OlddatamS = "";
bool con = false;

long lastMsg = 0;
String datasatir[1023];
String satirtmp = "";


int value = 0;
String oldwanip;
String wanip;
int rescanwifi = 0;
//int s=0;

unsigned long cursayac = 0;
unsigned long prevsayac = 0;
unsigned long mcursayac = 0;
unsigned long mprevsayac = 0;
unsigned long oldfirebasesayac = 0;
unsigned long oldtimerx = 0;
unsigned long timerxsayac;
String ssidtmp;
String passtmp;


// firebaseye gidecekler old olanlar.
bool gonder = false;
String mydata = "";


float usrtemp;
float oldusrtemp;
int hum; //humidy                     ******* push
float olsh; //humidy old
float t; // temp c
float f; // temp fahren
float hic; // heat index temp c       ******* push
float hif;
float oldhum;
float oldhic; //
int n = 0;
float hicfark = 0;
float humfark = 0;
String localip = "";
int Kombistate = 0;
int oldKombistate = 1000;
int Kombistateyazildi = 0;


//IPAddress local_ip(192, 168, 4, 2);
//IPAddress gateway(192, 168, 4, 1);
//IPAddress netmask(255, 255, 255, 0);

/*
   Custom broker class with overwritten callback functions
*/
/*
   Custom broker class with overwritten callback functions
*/
int hproto = 0;
int gecicihproto=0;

char data_str[1024];
class myMQTTBroker: public uMQTTBroker
{
  public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      //Serial.println(addr.toString()+" connected");
      return true;
    }

    virtual bool onAuth(String username, String password) {
      //Serial.println("Username/Password: "+username+"/"+password);
      return true;
    }

    virtual void onData(String topic, const char *data, uint32_t length) {
      data_str[length];
      memcpy(data_str, data, length);
      data_str[length] = '\0';
      topicim = topic;
      //datam=data_str;

      //Serial.println(/*"received topic '"+topic+"' with data '"+*/(String)data_str);
      datamS = (String)data_str;
      Serial.println(topicim);
      Serial.println(datamS);
    }
};

myMQTTBroker myBroker;
/*
   WiFi init stuff
*/

void startWiFiClient()
{

  //Serial.println("Connecting to "+(String)ssid);

  WiFi.begin(ssid, pass);

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
void datasatiryaz(void);
void httpcl(void);
void getfirebaseusrtempdata(void);
void dhtget(void);
void lcdupdate(void);
void printResult(FirebaseData &data , String komut);
//void createWebServer(void);
//Establishing Local server at port 80 whenever required


float geciciusrtemp;
float gecicihicfark;
float gecicihumfark;

int oldbuttondeger;
int buttonsayac;
int buttondeger;
int button = A0;

void printResult(FirebaseData &data , String komut)
{

  if (data.dataType() == "int") {
    Serial.println(data.intData());
    //          if(komut=="/koistemp"){
    //          usrtemp=data.floatData();
    //          }
  }
  else if (data.dataType() == "float") {
    Serial.println(data.floatData(), 5);
    //          if(komut=="/koistemp"){
    //          usrtemp=data.floatData();
    //          }
  }
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string") {
    Serial.println(data.stringData());
    if (komut == "/koistemp") {
      usrtemp = (data.stringData().substring(0, data.stringData().length())).toFloat();
    }
  }
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get the array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else
  {
    Serial.println(data.payload());
  }
}

void setup()
{
  // Start the broker
  //Serial.println("Starting MQTT broker");

  Serial.begin(115200); //Initialising if(DEBUG)Serial Monitor
  //Serial.begin(230400); //Initialising if(DEBUG)Serial Monitor
  Serial.println();
  Serial.println("Disconnecting current wifi connection");
  WiFi.disconnect();
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  display.begin();
  display.setContrast(20);
  display.clearDisplay();   // clear the screen and buffer
  pinMode(button, INPUT);
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

  /*
                                // ssid passdatas   0-99
                              for(int i=0;i<99;i++){
                                // firebase datas
                              Stage=0;
                              for(int i=0;i<180;i++){  100-180
                                // usrtemp
                              for(int i=;i<5;i++){     185

  */


  EEPROM.begin(512);
  //delay(10);
  String essid = "";
  String epass = "";
  String eFIREBASE_HOSTA = "";
  String eFIREBASE_AUTH = "";
  int Stage = 0;

  Serial.println("Reading ssid;pass;");
  // ssid passdatas
  for (int i = 0; i < 99; i++) {
    if (EEPROM.read(i) == 0) {
      Stage += 1;
    } else
    {
      if (Stage == 0) {
        essid += char(EEPROM.read(i));
      }
      if (Stage == 1) {
        epass += char(EEPROM.read(i));
      }
    }
  }
  ssid = essid;
  pass = epass;

  // firebase datas
  Stage = 0;
  for (int i = 0; i < 180; i++) {
    if (EEPROM.read(i + 100) == 0) {
      Stage += 1;
    } else
    {
      if (Stage == 0) {
        eFIREBASE_HOSTA += char(EEPROM.read(i + 100));
      }

      if (Stage == 1) {
        eFIREBASE_AUTH += char(EEPROM.read(i + 100));
      }
    }
  }
  FIREBASE_HOSTA = eFIREBASE_HOSTA + ".firebaseio.com";
  FIREBASE_AUTH = eFIREBASE_AUTH;


  String hprotoc;
  hprotoc += char(EEPROM.read(181));
  hproto = hprotoc.toInt();
  Stage == 0;
  gecicihproto=hproto;
  // usrtemp
  String usertemp = "";
  for (int iw = 0; iw < 6; iw++) {
    usertemp += char(EEPROM.read(iw + 183));
    Serial.print(char(EEPROM.read(iw + 183)));
  }
  usrtemp = usertemp.toFloat();
  if(usrtemp<17){usrtemp=17;}
  oldusrtemp = usrtemp;
  geciciusrtemp = usrtemp;

  String hicfarks;
  for (int iw = 0; iw < 6; iw++) {
    hicfarks += char(EEPROM.read(iw + 200));
    Serial.print(char(EEPROM.read(iw + 200)));
  }
  hicfark = hicfarks.toFloat();
  gecicihicfark = hicfark;


  String humfarks;
  for (int iw = 0; iw < 6; iw++) {
    humfarks += char(EEPROM.read(iw + 210));
    Serial.print(char(EEPROM.read(iw + 210)));
  }
  humfark = humfarks.toFloat();
  gecicihumfark = humfark;



  Serial.print ("hproto= ");
  Serial.println(hproto);
  delay(1000);
  delay(1000);
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(FIREBASE_HOSTA);
  Serial.println(FIREBASE_AUTH);
  buzzercal(3000, 50); delay(100);
  buzzercal(4000, 50); delay(10);
  connectWifi();

  dht.begin();
  arduinoota();
  launchWeb();

  
  if (hproto >= -1) {
    myBroker.init();
    myBroker.subscribe("#");
  }


  if (hproto >= -1) {
    if (hproto == 2) {
      //Firebase.begin(FIREBASE_HOSTA, FIREBASE_AUTH);
      Firebase.begin(FIREBASE_HOSTA, FIREBASE_AUTH);

      // Optional başı
      //Firebase.reconnectWiFi(true);
      Firebase.setMaxRetry(firebaseData, 3);
      Firebase.setMaxErrorQueue(firebaseData, 30);
      Firebase.enableClassicRequest(firebaseData, true);
      // Optional sonu
      
      //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
      firebaseData.setBSSLBufferSize(1024, 1024);

      //Set the size of HTTP response buffers in the case where we want to work with large data.
      firebaseData.setResponseSize(1024);

      //Set database read timeout to 1 minute (max 15 minutes)
      Firebase.setReadTimeout(firebaseData, 1000 * 60);
      //tiny, small, medium, large and unlimited.
      //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
      Firebase.setwriteSizeLimit(firebaseData, "tiny");

      //optional, set the decimal places for float and double data to be stored in database
      Firebase.setFloatDigits(2);
      Firebase.setDoubleDigits(6);
    }

  }

  // Start the server
  server.begin();
  Serial.println("Server started");

}


void getfirebaseusrtempdata() {
}

void buzzercal(int sesfreq , int sestime) {
  tone(buzzPin, sesfreq);
  delay(sestime);
  noTone(buzzPin);
}

void dhtget() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

  hum = dht.readHumidity() + humfark;
  f = dht.readTemperature(true);
  //  hum=dht.computeHeatIndex(f, hum);
  // Read temperature as Celsius (the default)
  t = dht.readTemperature(); //
  // Read temperature as Fahrenheit (isFahrenheit = true)


  // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Fahrenheit (the default)
  //  hum = dht.computeHeatIndex(f, hum);
  // Compute heat index in Celsius (isFahreheit = false)
  hic = t + hicfark; //
  //hic = dht.computeHeatIndex(hic, hum, false)+hicfark;  // eskiden t idi

  Serial.print(("Humidity: "));
  Serial.print(hum);
  Serial.print(("%  Temperature: "));
  Serial.print(hic);
  Serial.print((" C"));
  Serial.println(".");
}

String newdata;
String degisken;
String deger;
int manualchg = 0;
String topictmp = "";
int lcdupdated = 0;
int butonok = 0;
int ekrantip = 0;



void lcdupdate() {

  if (ekrantip == 4) {
    display.clearDisplay();
    display.setCursor(20, 0);
    String areads = " " + (String)buttondeger;
    display.println(areads);
    display.display();
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);

    display.setCursor(0, 8);
    IPAddress lip = WiFi.localIP();
    String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
    display.println(lipStr);
    display.setCursor(0, 24);
    display.println(ssid);
    display.display();
  }


  if (ekrantip == 1) {
    display.clearDisplay();
    display.setCursor(20, 0);
    String areads = " " + (String)buttondeger;
    display.println(areads);
    display.display();

    display.clearDisplay();
    display.setCursor(40, 0);
    if(gecicihproto!=hproto){
      display.setTextColor(WHITE, BLACK);
    }
    String hprotos = "NoCon";
    if(gecicihproto==2){
    hprotos = "FBCon";
    }
    display.println(hprotos);
    display.display();
    
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    if (WiFi.status() == WL_CONNECTED)
    {
      display.drawBitmap(0, 0,  Lan_on_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    } else {
      display.drawBitmap(0, 0,  Lan_off_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    }
    display.setTextSize(1);
    display.setCursor(10, 30);


    String usrtemptemp = (String)usrtemp;
    usrtemptemp = usrtemptemp.substring(0, usrtemptemp.indexOf(".") + 2);
    if (usrtemp != geciciusrtemp) {
      display.setTextColor(WHITE, BLACK);
      usrtemptemp = (String)geciciusrtemp;
      usrtemptemp = usrtemptemp.substring(0, usrtemptemp.indexOf(".") + 2);
      display.print(usrtemptemp);
    } else
    {
      display.setTextColor(BLACK, WHITE);
      display.print(usrtemptemp);
    }
    display.setTextColor(BLACK, WHITE);
    display.drawCircle(80, 9, 2, BLACK);
    display.setTextSize(1);
    display.setTextSize(2);
    //  display.drawBitmap(0, 9, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    display.setCursor(24, 9);
    String temptemp = (String)hic;
    temptemp = temptemp.substring(0, temptemp.indexOf(".") + 2);
    display.print(temptemp);
    display.drawCircle(42, 28, 1, BLACK);
    display.setTextSize(1);
    display.drawBitmap(47, 28, damla_logo8x8_glcd_bmp, 8, 8, 1);
    display.setCursor(58, 30);
    int nem = ((String)hum).toInt();
    String nemtemp = "% " + (String)nem;
    display.println(nemtemp);

    if (Kombistate == 1) {
      display.drawBitmap(0, 9,  on_logo16_glcd_bmp, 16, 16, 1);
    }
    if (Kombistate == 0) {
      display.drawBitmap(0, 9, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    }
    display.display();
  }




  if (ekrantip == 0) {
    display.clearDisplay();
    display.setCursor(20, 0);
    String areads = " " + (String)buttondeger;
    display.println(areads);
    display.display();
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    if (WiFi.status() == WL_CONNECTED)
    {
      display.drawBitmap(0, 0,  Lan_on_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    } else {
      display.drawBitmap(0, 0,  Lan_off_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    }
    display.setTextSize(2);
    display.setCursor(24, 25);
    String usrtemptemp = (String)usrtemp;
    usrtemptemp = usrtemptemp.substring(0, usrtemptemp.indexOf(".") + 2);
    if (usrtemp != geciciusrtemp) {
      display.setTextColor(WHITE, BLACK);
      usrtemptemp = (String)geciciusrtemp;
      usrtemptemp = usrtemptemp.substring(0, usrtemptemp.indexOf(".") + 2);
      display.print(usrtemptemp);
    } else
    {
      display.setTextColor(BLACK, WHITE);
      display.print(usrtemptemp);
    }
    display.setTextColor(BLACK, WHITE);
    display.drawCircle(80, 9, 2, BLACK);
    //display.drawBitmap(0, 9, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    String temptemp = (String)hic;
    temptemp = temptemp.substring(0, temptemp.indexOf(".") + 2);
    display.setCursor(24, 9);
    display.print(temptemp);
    display.drawCircle(80, 26, 2, BLACK);
    display.setTextSize(1);
    display.drawBitmap(0, 40, damla_logo8x8_glcd_bmp, 8, 8, 1);
    display.setCursor(10, 40);
    int nem = ((String)hum).toInt();
    String nemtemp = "% " + (String)nem;
    display.println(nemtemp);


    if (Kombistate == 1) {
      display.drawBitmap(0, 9,  on_logo16_glcd_bmp, 16, 16, 1);
    }
    if (Kombistate == 0) {
      //display.drawBitmap(0, 25,  off_logo16_glcd_bmp, 16, 16, 1);
      display.drawBitmap(0, 9, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    }
    display.display();
  }


  if (ekrantip == 2) { // hicfark
    display.clearDisplay();
    display.setCursor(20, 0);
    String areads = " " + (String)buttondeger;
    display.println(areads);
    display.display();
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    if (WiFi.status() == WL_CONNECTED)
    {
      display.drawBitmap(0, 0,  Lan_on_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    } else {
      display.drawBitmap(0, 0,  Lan_off_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    }
    display.setTextSize(2);
    display.setCursor(24, 9);
    String gecicihicfarktemp = (String)hicfark;
    gecicihicfarktemp = gecicihicfarktemp.substring(0, gecicihicfarktemp.indexOf(".") + 2);
    if (hicfark != gecicihicfark) {
      display.setTextColor(WHITE, BLACK);
      gecicihicfarktemp = (String)gecicihicfark;
      gecicihicfarktemp = gecicihicfarktemp.substring(0, gecicihicfarktemp.indexOf(".") + 2);
      display.print(gecicihicfarktemp);
    } else
    {
      display.setTextColor(BLACK, WHITE);
      display.print(gecicihicfarktemp);
    }
    display.setTextColor(BLACK, WHITE);
    display.drawCircle(80, 9, 2, BLACK);
    display.drawBitmap(0, 30, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    //  String temptemp=(String)hic;
    //  display.setCursor(18,25);
    //  display.print(temptemp);
    //  display.drawCircle(80, 26, 2, BLACK);
    display.setTextSize(1);
    //  display.drawBitmap(0, 40, damla_logo8x8_glcd_bmp, 8, 8, 1);
    display.setCursor(24, 32);
    //  int nem = ((String)hum).toInt();
    //  String nemtemp="% "+(String)nem;
    //  display.println(nemtemp);
    display.println("T kalibre");

    if (Kombistate == 1) {
      display.drawBitmap(0, 9,  on_logo16_glcd_bmp, 16, 16, 1);
    }
    if (Kombistate == 0) {
      // display.drawBitmap(0, 9,  off_logo16_glcd_bmp, 16, 16, 1);
      display.drawBitmap(0, 9, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    }
    display.display();
  }


  if (ekrantip == 3) { // hicfark
    display.clearDisplay();
    display.setCursor(20, 0);
    String areads = " " + (String)buttondeger;
    display.println(areads);
    display.display();
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    if (WiFi.status() == WL_CONNECTED)
    {
      display.drawBitmap(0, 0,  Lan_on_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    } else {
      display.drawBitmap(0, 0,  Lan_off_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
    }
    display.setTextSize(2);
    display.setCursor(40, 9);
    String gecicihumfarktemp = (String)humfark;
    gecicihumfarktemp = gecicihumfarktemp.substring(0, gecicihumfarktemp.indexOf("."));
    if (humfark != gecicihumfark) {
      display.setTextColor(WHITE, BLACK);
      gecicihumfarktemp = (String)gecicihumfark;
      gecicihumfarktemp = gecicihumfarktemp.substring(0, gecicihumfarktemp.indexOf("."));
      display.print(gecicihumfarktemp);
    } else
    {
      display.setTextColor(BLACK, WHITE);
      display.print(gecicihumfarktemp);
    }
    display.setTextColor(BLACK, WHITE);
    display.drawTriangle(78, 9, 81, 15, 84, 9, BLACK);
    //  display.drawBitmap(0, 16, termlogo_logo8x8_glcd_bmp, 8, 8, 1);
    //  String temptemp=(String)hic;
    //  display.setCursor(18,25);
    //  display.print(temptemp);
    //  display.drawCircle(80, 26, 2, BLACK);
    display.setTextSize(1);
    //display.drawBitmap(0, 40, damla_logo8x8_glcd_bmp, 8, 8, 1);
    display.drawBitmap(0, 30, damla_logo8x8_glcd_bmp, 8, 8, 1);
    display.setCursor(24, 32);
    //  int nem = ((String)hum).toInt();
    //  String nemtemp="% "+(String)nem;
    //  display.println(nemtemp);
    display.println("H kalibre");

    if (Kombistate == 1) {
      display.drawBitmap(0, 9,  on_logo16_glcd_bmp, 16, 16, 1);
    }
    if (Kombistate == 0) {
      display.drawBitmap(0, 9,  off_logo16_glcd_bmp, 16, 16, 1);
    }
    display.display();
  }


}


int zamanasimi = 0;
int resetsayac = 0;


void loop() {
  ArduinoOTA.handle();
  buttondeger = analogRead(button);

  if (buttondeger > 100) {
    if (zamanasimi > 0 & zamanasimi < 1100) wt = -3000;
    if (buttonsayac == 0) {
      // menu ve okay buton
      if (buttondeger > 523 && buttondeger < 529) {
        buzzercal(1397, 20); delay(10);
        if (ekrantip == 0 && geciciusrtemp != usrtemp) {
          butonok = 1;
        }
        else if (ekrantip == 1 && gecicihproto != hproto) {
          butonok = 4;
        }
        else if (ekrantip == 2 && gecicihicfark != hicfark) {
          butonok = 2;
        }
        else if (ekrantip == 3 && gecicihumfark != humfark) {
          butonok = 3;
        }
        else {
          ekrantip += 1;
          if (ekrantip > 4) {
            ekrantip = 0;
          }
        }
        zamanasimi = 0;
        if (resetsayac > 10)ESP.reset();
      }

      // eksi buton
      if (buttondeger > 529) {
        buzzercal(1175, 20); delay(10);
        if (ekrantip == 0 && geciciusrtemp > 7.1)geciciusrtemp -= 0.1;
        if (ekrantip == 1 && gecicihproto!=2){gecicihproto = 2;} else {gecicihproto = 0;}
        if (ekrantip == 2 && gecicihicfark > -2.9)gecicihicfark -= 0.1;
        if (ekrantip == 3 && gecicihumfark > -16)gecicihumfark -= 1;
        zamanasimi = 0;
      }

      // artı buton
      if (buttondeger > 519 && buttondeger < 523) {
        buzzercal(1760, 20); delay(10);
        if (ekrantip == 0 && geciciusrtemp < 28.9)geciciusrtemp += 0.1;
        if (ekrantip == 1 && gecicihproto!=2){gecicihproto = 2;} else {gecicihproto = 0;}
        if (ekrantip == 2 && gecicihicfark < 2.9)gecicihicfark += 0.1;
        if (ekrantip == 3 && gecicihumfark < 16)gecicihumfark += 1;
        zamanasimi = 0;
      }
      oldbuttondeger = buttondeger;
      lcdupdate();
    }


    buttonsayac += 1;
    zamanasimi = 0;
    if (buttonsayac > 20) {
      buttonsayac = 0; delay(1);
      resetsayac += 1;
    }
  } else {
    buttonsayac = 0;
    zamanasimi += 1;
    resetsayac = 0;
    delay(1);
    if (zamanasimi > 1100) {
      zamanasimi = 0;
      geciciusrtemp = usrtemp;
      gecicihicfark = hicfark;
      gecicihumfark = humfark;
      gecicihproto = hproto;
      lcdupdate();
    }
  }

  if (wt % 100 == 1) {
  }

  //dht ve lcd kısmı
  if (wt == 5) {
    dhtget();
    lcdupdate();
    long unsigned timerim = millis();
    //saat
    int sa = (int)(timerim / 3600000);
    if (sa < 10)Serial.print( "0" + (String)sa + ":");
    else Serial.print((String)sa + ":");
    int dk = (int)(timerim / 60000);
    if (dk < 10)Serial.print( "0" + (String)dk + ":");
    else Serial.print((String)dk + ":");
    int sn = (int)(((timerim - (dk * 60000)) / 1000));
    if (sn < 10)Serial.println( "0" + (String)sn);
    else Serial.println((String)sn);
    //Serial.print(f);
    //Serial.print(F("°F  Heat index: "));
    //Serial.print(hic);
    //Serial.print(F("°C "));
    //Serial.print(hif);
    //Serial.println(F("°F"));


    delay(100);
  }
  //dht son


  //cxtime=millis();
  //cxtime - pxtime <= totime
  cxtime = millis();
  if (cxtime - pxtime > totime * 60) {
    pxtime = millis();

    IPAddress lipx = WiFi.localIP();
    String lipxS = String(lipx[0]);


    if (lipxS.toInt() < 1 & rescanwifi == 0)
    {
      Serial.print((WiFi.status() != WL_CONNECTED));
      connectWifi();
      Serial.print((WiFi.localIP()));

    }
    pxtime = millis();
  }

  httpcl();
  //şart///////////////////////////////////
  delay(1); wt += 1;
  if (wt >= 2101) {
    wt = 1;
  }
  if (rescanwifi == 1) {
    wifiscan();
    rescanwifi = 0;
  }
  //şart///////////////////////////////////

  if (hproto < 7)
  {


    // wt 1000 içinde firebase oku yaz yapalım
    if (wt == 900  || wt == 1800) {
      //Serial.println("firebase data read");
      /*
                      hum=Firebase.getFloat("hum");
                                      if (Firebase.failed()) {
                      Serial.print("Getting /number failed:oldhic");
                      Serial.println(Firebase.error());
                      return;
                      }
                      oldhic=Firebase.getFloat("oldhic");
                                      if (Firebase.failed()) {
                      Serial.print("Getting /number failed:oldhic");
                      Serial.println(Firebase.error());
                      return;
                      }
      */

      if (hproto == 2) {


        if (Firebase.getString(firebaseData, "/koistemp"))
        {
          Serial.println("PASSED");
          Serial.println("PATH: " + firebaseData.dataPath());
          Serial.println("TYPE: " + firebaseData.dataType());
          Serial.println("ETag: " + firebaseData.ETag());
          Serial.print("VALUE: ");
          printResult(firebaseData, "/koistemp");
          Serial.println("------------------------------------");
          Serial.println();
        if (usrtemp != oldusrtemp & butonok==0) 
        {
        deger = "usrtemp:" +  (String)usrtemp  + "|";
        myBroker.publish("burole", deger);
        deger = "geciciusrtemp:" +  (String)usrtemp  + "|";
        //myBroker.publish("bumqtt",deger);
        String usertemp = (String)usrtemp + "|";
        int str_len = usertemp.length() + 1;
        char char_array[str_len];
        usertemp.toCharArray(char_array, str_len);
        for (int iw = 0; iw < usertemp.length() + 1; iw++) {
          if (char_array[iw] == '|') {
            EEPROM.write(iw + 183, 0);
          } else {
            EEPROM.write(iw + 183, char_array[iw]);
            Serial.print(char_array[iw]);
          }
        }
        EEPROM.commit();
        lcdupdate();
        }
        
        } else {
          Serial.println("FAILED");
          Serial.println("REASON: " + firebaseData.errorReason());
          Serial.println("------------------------------------");
          Serial.println();
        }



        if (usrtemp != oldusrtemp) {
          oldusrtemp = usrtemp;
          geciciusrtemp = usrtemp;
        }
        lcdupdate();
      }


      Serial.println(oldusrtemp);
      Serial.println(usrtemp);
      Serial.println(oldhic);
      Serial.println(oldhum);




    }





    if (datamS != "") {
      // XXXXX sorulanı bul gönder XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      degisken = datamS.substring(datamS.indexOf("?") + 1, datamS.indexOf(":"));
      deger = datamS.substring(datamS.indexOf(":") + 1, datamS.indexOf("|"));
      if (ekrantip == 0) {
        display.setCursor(40, 40);
        String tempdisp = degisken + " " + deger;
        display.println(tempdisp);
        display.display();
      }
      else if (ekrantip == 4) {
        display.setCursor(0, 32);
        String tempdisp = degisken + " " + deger;
        display.println(tempdisp);
        display.display();
      } else
      {
        display.setCursor(0, 40);
        String tempdisp = degisken + " " + deger;
        display.println(tempdisp);
        display.display();
      }


      // istenen abonelik var bişeyler var onu bul ve gönder
      if (datamS.indexOf("?") > -1) {
        degisken = datamS.substring(datamS.indexOf("?") + 1, datamS.indexOf(":"));
        deger = datamS.substring(datamS.indexOf(":") + 1, datamS.indexOf("|"));
        topictmp = "bu" + topicim.substring(2, topicim.length());
        manualchg = 0;
        if (datamS.indexOf("<") > -1) {
          lcdupdate();
        }

        if (degisken == "geldim" & deger == "hepsi") {
          dhtget();
          if (usrtemp > hic) {
            Kombistate = 1;
          }
          if (usrtemp < hic - 0.2) {
            Kombistate = 0;
          }
          deger = "";
          deger += "odatemp:" +  (String)hic  + "|";
          deger += "odahum:" +  (String)hum  + "|";
          deger += "usrtemp:" +  (String)usrtemp  + "|";
          deger += "Kombistate:" +  (String)Kombistate  + "|";
        }
        datamS = "";
        myBroker.publish(topictmp, deger);

      } else
      {

        degisken = datamS.substring(0, datamS.indexOf(":"));
        if (degisken == "usrtemp") {
          usrtemp = datamS.substring(datamS.indexOf(":") + 1, datamS.indexOf("|")).toFloat();
          usrtemp;
          degisken = "";
          datamS = "";
          geciciusrtemp = usrtemp;
        }
        // saat
        long unsigned timerim = millis();
        int sa = (int)(timerim / 3600000);
        if (sa < 10)Serial.print( "0" + (String)sa + ":");
        else Serial.print((String)sa + ":");
        int dk = (int)(timerim / 60000);
        if (dk < 10)Serial.print( "0" + (String)dk + ":");
        else Serial.print((String)dk + ":");
        int sn = (int)(((timerim - (dk * 60000)) / 1000));
        if (sn < 10)Serial.println( "0" + (String)sn);
        else Serial.println((String)sn);
        Serial.print("  datams:" + datamS);
        datamS = "";
      }

    }


    if (oldhic != hic) {
      oldhic = hic;  deger = "odatemp:" +  (String)hic  + "|";
      myBroker.publish("burole", deger);
      //myBroker.publish("bumqtt",deger);
      if (hproto == 2)Firebase.setFloat(firebaseData, "/oldhic", hic);
    }
    delay(5);
    if (oldhum != hum) {
      oldhum = hum;  deger = "odahum:" +  (String)hum  + "|";
      myBroker.publish("burole", deger);
      myBroker.publish("bumqtt", deger);
      if (hproto == 2)Firebase.setFloat(firebaseData, "hum", hum);
    }
    delay(5);

if (gecicihproto != hproto) {
      if (butonok == 4) {
        butonok = 0;
        hproto = gecicihproto;
        String hprotos = (String)hproto + "|";
        int str_len = hprotos.length() + 1;
        char char_array[str_len];
        hprotos.toCharArray(char_array, str_len);
            EEPROM.write(181, char_array[0]);
            EEPROM.write(182, 0);
            Serial.print(char_array[0]);
        EEPROM.commit();
        ESP.reset();
        }
      }

    delay(5);
    if (geciciusrtemp != usrtemp) {
      if (butonok == 1) {
        butonok = 0;
        oldusrtemp = usrtemp;
        usrtemp = geciciusrtemp;
        if (hproto == 2){Firebase.setString(firebaseData, "/koistemp", (String)usrtemp);
          if (Firebase.setTimestamp(firebaseData, "/Set/Timestamp"))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());

    //Timestamp saved in millisecond, get its seconds from intData()
    Serial.print("TIMESTAMP (Seconds): ");
    Serial.println(firebaseData.intData());

    //Or print the total milliseconds from doubleData()
    //Due to bugs in Serial.print in Arduino library, use printf to print double instead.
    printf("TIMESTAMP (milliSeconds): %.0lf\n", firebaseData.doubleData());

    //Or print it from payload directly
    Serial.print("TIMESTAMP (milliSeconds): ");
    Serial.println(firebaseData.payload());

    //Due to some internal server error, ETag cannot get from setTimestamp
    //Try to get ETag manually
    Serial.println("ETag: " + Firebase.getETag(firebaseData, "/Set/Timestamp"));
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
        }
        String usertemp = (String)usrtemp + "|";
        int str_len = usertemp.length() + 1;
        char char_array[str_len];
        usertemp.toCharArray(char_array, str_len);
        for (int iw = 0; iw < usertemp.length() + 1; iw++) {
          if (char_array[iw] == '|') {
            EEPROM.write(iw + 183, 0);
          } else {
            EEPROM.write(iw + 183, char_array[iw]);
            Serial.print(char_array[iw]);
          }
        }
        EEPROM.commit();
        lcdupdate();
        deger = "usrtemp:" +  (String)usrtemp  + "|";
        myBroker.publish("burole", deger);
        deger = "geciciusrtemp:" +  (String)usrtemp  + "|";
        //myBroker.publish("bumqtt",deger);

      }
    }

    if (gecicihicfark != hicfark) {
      if (butonok == 2) {
        butonok = 0;
        hicfark = gecicihicfark;
        String hicfarks = (String)hicfark + "|";
        int str_len = hicfarks.length() + 1;
        char char_array[str_len];
        hicfarks.toCharArray(char_array, str_len);
        for (int iw = 0; iw < hicfarks.length() + 1; iw++) {
          if (char_array[iw] == '|') {
            EEPROM.write(iw + 200, 0);
          } else {
            EEPROM.write(iw + 200, char_array[iw]);
            Serial.print(char_array[iw]);
          }
        }
        EEPROM.commit();
        dhtget();
        lcdupdate();
      }
    }


    if (gecicihumfark != humfark) {
      if (butonok == 3) {
        butonok = 0;
        humfark = gecicihumfark;
        String humfarks = (String)humfark + "|";
        int str_len = humfarks.length() + 1;
        char char_array[str_len];
        humfarks.toCharArray(char_array, str_len);
        for (int iw = 0; iw < humfarks.length() + 1; iw++) {
          if (char_array[iw] == '|') {
            EEPROM.write(iw + 210, 0);
          } else {
            EEPROM.write(iw + 210, char_array[iw]);
            Serial.print(char_array[iw]);
          }
        }
        EEPROM.commit();
        dhtget();
        lcdupdate();
      }
    }


    if (oldusrtemp != usrtemp) {
      if (usrtemp > 0) {
        oldusrtemp = usrtemp;
        deger = "usrtemp:" +  (String)usrtemp  + "|";
        myBroker.publish("burole", deger);
        deger = "geciciusrtemp:" +  (String)usrtemp  + "|";
        //myBroker.publish("bumqtt",deger);
        String usertemp = (String)usrtemp + "|";
        int str_len = usertemp.length() + 1;
        char char_array[str_len];
        usertemp.toCharArray(char_array, str_len);
        for (int iw = 0; iw < usertemp.length() + 1; iw++) {
          if (char_array[iw] == '|') {
            EEPROM.write(iw + 183, 0);
          } else {
            EEPROM.write(iw + 183, char_array[iw]);
            Serial.print(char_array[iw]);
          }
        }
        EEPROM.commit();
        lcdupdate();
      }


    }


    if (oldKombistate != Kombistate) {
      oldKombistate = Kombistate;  deger = "Kombistate:" +  (String)Kombistate  + "|";
      lcdupdate();
      myBroker.publish("burole", deger);
      //myBroker.publish("bumqtt",deger);
      if (hproto == 2)Firebase.setFloat(firebaseData, "/Kombistate", Kombistate);
      if (Kombistate == 1)buzzercal(4000, 20); delay(100);
      if (Kombistate == 0)buzzercal(400, 20); delay(100);
    }
  }



  // kendiliğinden yollama olayı
  if (oldhic != hic) {
    lcdupdate();
    oldhic = hic;  deger = "odatemp:" +  (String)hic  + "|";
    myBroker.publish("burole", deger);
    //myBroker.publish("bumqtt",deger);
    if (hproto == 2)Firebase.setFloat(firebaseData, "/oldhic", hic);
  }
  delay(5);
  if (oldhum != hum) {
    lcdupdate();
    oldhum = hum;  deger = "odahum:" +  (String)hum  + "|";
    myBroker.publish("burole", deger);
    //myBroker.publish("bumqtt",deger);
    if (hproto == 2)Firebase.setFloat(firebaseData, "/hum", hum);
  }
  delay(5);

  if (oldusrtemp != usrtemp) {
    oldusrtemp = usrtemp;
    deger = "usrtemp:" +  (String)usrtemp  + "|";
    myBroker.publish("burole", deger);
    if (usrtemp > 0) {
      String usertemp = (String)usrtemp + "|";
      int str_len = usertemp.length() + 1;
      char char_array[str_len];
      usertemp.toCharArray(char_array, str_len);

      for (int iw = 0; iw < usertemp.length() + 1; iw++) {
        if (char_array[iw] == '|') {
          EEPROM.write(iw + 183, 0);
        } else {
          EEPROM.write(iw + 183, char_array[iw]);
          Serial.print(char_array[iw]);
        }
      }
      EEPROM.commit();
      lcdupdate();
    }
  }

  if (oldKombistate != Kombistate) {
    oldKombistate = Kombistate;  deger = "Kombistate:" +  (String)Kombistate  + "|";
    myBroker.publish("burole", deger);
    //myBroker.publish("bumqtt",deger);
    if (hproto == 2) Firebase.setFloat(firebaseData, "/Kombistate", Kombistate);
    lcdupdate();
  }

  if (wt == 800 || wt == 1200 || wt == 1600 || wt == 2000) {


    if (usrtemp > hic) {
      Kombistate = 1;
    }

    if (usrtemp < hic - 0.2) {
      Kombistate = 0;
    }

  }



  //    for( int im = 0; im < sizeof(data_str); ++im ){ memset(data_str, 0, sizeof(data_str)-1);}

}

void httpheader() {
  content = "";
  content = "<!DOCTYPE HTML>\r\n<html>Welcome to main page";
  content += "<head>";
  content += "<META charset=iso-8859-9\">";
  content += "<link rel=\"shortcut icon\" href=\"http://sanalkurs.net/favicon.ico\">";
  content += "</head>";
  content += "<body scroll=\"no\" style=\"overflow: hidden\"><br>";
  content += "<font size=\"3\" color=\"#0000ff\">" + esphostname + "</font>";
  content += "<br>";
  content += "<table style=\"border:5px solid black;width:500px\"><tr><td style=\"border:1px solid black;width:250p; align:center; \">";
  content += "<p><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form></p>";
  content += "</td><td style=\"border:1px solid black;width:250px;\">";
  content += "<form action=\"/ssidset\" method=\"POST\"><input type=\"submit\" value=\"Wifi Lookap\"></form>";
  content += "</td><td style=\"border:1px solid black;width:250px\">";
  content += "<form action=\"/firebasecfg\" method=\"POST\"><input type=\"submit\" value=\"firebase config\"></form>";
  content += "</td><td style=\"border:1px solid black;width:250px\">";
  content += "<form action=\"/hproto\" method=\"POST\"><input type=\"submit\" value=\"Link methods\"></form>";
  content += "</td></tr></table>";

}

void httpcl() {

  WiFiClient client = server.available();
  delay(10);
  if (client) {
    wt=-2000;
    // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    String ah="";
    while (client.connected() && currentTime - previousTime <= timeoutTime * 2) { // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.print(c);                    // print it out the serial monitor
        if (headerset == 0)header += c;

        if (c == '\r') {
        }
        
        if (c == '\n') {
          Serial.print("header::::" + header);
          headerset=1;
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:  /*




          /*
            if(header.indexOf(" /favicon.ico HTTP/")>-1)
            {
            <link rel="shortcut icon" href="http://sanalkurs.net/favicon.ico">
                  client.println("HTTP/1.1 200 OK");
                  client.println("Content-type:text/html");
                  client.println("Connection: close");
                  header = "";
                  client.stop();
            }
          */


          if (header.indexOf(" / HTTP/") > -1) {
            httpheader();
Serial.print(content);
            IPAddress lip = WiFi.localIP();
            String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
            IPAddress gip = WiFi.gatewayIP();
            String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
            IPAddress dns = WiFi.dnsIP();
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
            if (Kombistate == 1) {
              content += " Kombi [&] <br>";
            }
            if (Kombistate == 0) {
              content += " Kombi [_] <br>";
            }
            if (Kombistate == 3) {
              content += " Kombi [_] &nbsp;&nbsp;&nbsp; Role_Esp offline <br>";
            }
            content += "istenen temp:&nbsp;";
            content += (String)usrtemp + "<br>";
            content += ((" Temperature: "));
            content += "  hic:" + (String)hic;
            content += ((" &#8451;"));
            content += (("/"));
            content += (String)((1.8 * hic) + 32);
            content += ((" &#8457;"));
            content += "<br>";
            content += (("Humidity: "));
            content += " hum: " + (String)hum;
            content += "% <br>";
            content += "<br>";
            content += "<br>";
            content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>";
            content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br></body>";
            content += "</html>";
            //client.println("HTTP/1.1 200 OK");
            //client.println("Content-type:text/html");
            //client.println("Connection: close");
            //client.println();
            client.println(content);

            // Clear the header variable
            header = "";

            //client.stop();
          }




          if (header.indexOf(" /ssidset") > -1) {
            httpheader();

            IPAddress lip = WiFi.localIP();
            String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
            IPAddress gip = WiFi.gatewayIP();
            String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
            IPAddress dns = WiFi.dnsIP();
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
            if (header.indexOf("scanwifi HTTP") > -1) {
              rescanwifi = 1;
            }
            content += "<br>Click Scan Wifi Button 10 second waiting.<br>";
            content += "<br>Click Result Button get APs infos.<br>";
            content += "<form action=\"/ssidset\" method=\"POST\"><input type=\"submit\" value=\"Result Wifi Scan\"></form>";
            content += "</td></tr></table>";
            content += "<p>";
            content += "<table style=\"border:2px solid black;width:500px\"><tr><td style=\"border:1px solid black;width:500px\">";
            content += "Scanned WiFi Devices</td><tr><td>";
            content += "<br>";
            content += st;
            content += "</td><tr></table>";
            content += "<font size=\"4\" color=\"#aa0000\">Gonder'e bastiktan sonra bilgiler dogru ise modeme baglanirsiniz</font>";
            content += "<form method='get' action='ssidset'><label>SSID: </label><input name='ssid' length=32><label>PASS: </label><input name='pass' length=64><input type='submit'></form>";
            content += "<br>";
            content += "<br><br>";
            content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>";
            content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br></body>";
            content += "</html>";
          }

          if (header.indexOf("/ssidset") > -1) { //   "/setting?ssid=Zyxel&pass=bebedede14"
            // yazma kısmı
            //Serial.println(header);
            if (header.indexOf("ssid=") > -1) {
              ssidtmp = header.substring((header.indexOf("ssid=") + 5), (header.indexOf("&")));
              Serial.println(ssidtmp);
            }
            if (header.indexOf("&pass=") > -1) {
              passtmp = header.substring((header.indexOf("&pass=") + 6), (header.indexOf(" HTTP")));
              Serial.println(passtmp);
              String buf = ssidtmp + '|' + passtmp + '|';

              //---------------------------------------- Write flash for ssid and pass
              Serial.println("Write flash file ssid;pass;");

              int str_len = buf.length() + 1;
              char char_array[str_len];
              buf.toCharArray(char_array, str_len);


              for (int i = 0; i < buf.length() + 1; i++) {

                if (char_array[i] == '|') {
                  EEPROM.write(i, 0);
                } else {
                  EEPROM.write(i, char_array[i]);

                }

              }
              EEPROM.commit();
              content = "{\"Success\":\"saved to file... reset to boot into new wifi\"}";
              statusCode = 200;
              delay(50);
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              client.println();
              client.println("Reseting ESP ... waiting 10 seconds");
              client.println("<br><form action=\"/\" method=\"POST\"><input type=\"submit\" value=\"Home\"></form><br>");
              client.println("<form action=\"/ssidset\" method=\"POST\"><input type=\"submit\" value=\"firebase config\"></form>");
              // Clear the header variable
              header = "";
              //client.stop();
              ESP.reset();
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println(content);
            // Clear the header variable
            header = "";
            //client.stop();
          }


          ///////////////////////////////////////////////////////////////////////
          //               EEPROM.write(200,protokol bne ise onu yazacağız. okuma yukarıya yazılmalı;
          //               EEPROM.commit();
          ///////////////////////////////////////////////////////////////////////


          if (header.indexOf(" /firebasecfg") > -1) {
            httpheader();

            IPAddress lip = WiFi.localIP();
            String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
            IPAddress gip = WiFi.gatewayIP();
            String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
            IPAddress dns = WiFi.dnsIP();
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

          if (header.indexOf(" /firebaseset") > -1) { //   "/setting?ssid=Zyxel&pass=bebedede14"
            // yazma kısmı
            //Serial.println(header);
            if (header.indexOf("host=") > -1) {
              hosttmp = header.substring((header.indexOf("host=") + 5), (header.indexOf("&")));
              Serial.println(hosttmp);
            }
            if (header.indexOf("&anahtar=") > -1) {
              anahtartmp = header.substring((header.indexOf("&anahtar=") + 9), (header.indexOf(" HTTP")));
              Serial.println(anahtartmp);
              String buf = hosttmp + '|' + anahtartmp + '|';
              //---------------------------------------- Write flash for ssid and pass
              Serial.println("Write flash file firebase settings;");

              int str_len = buf.length() + 1;
              Serial.println(buf);
              char char_array[str_len];
              buf.toCharArray(char_array, str_len);


              for (int i = 0; i < buf.length() + 1; i++) {
                Serial.print(char_array[i]);
                if (char_array[i] == '|') {
                  EEPROM.write(i + 100, 0);
                } else {
                  EEPROM.write(i + 100, char_array[i]);

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
              client.println("<form action=\"/firebasecfg\" method=\"POST\"><input type=\"submit\" value=\"firebase config\"></form>");
              // Clear the header variable
              header = "";
              //client.stop();

              delay(50);
              ESP.reset();
            }
            header = "";
          }


          if (header.indexOf("/info") > -1) {
            // firebase datas eeprom
            int Stage = 0;
            String eFIREBASE_HOSTA = ""; //+=char(EEPROM.read(i+100));
            String eFIREBASE_AUTH = ""; //+=char(EEPROM.read(i+100));
            for (int i = 0; i < 180; i++) {
              if (EEPROM.read(i + 100) == 0) {
                Stage += 1;
              } else
              {
                if (Stage == 0) {
                  eFIREBASE_HOSTA += char(EEPROM.read(i + 100));
                }

                if (Stage == 1) {
                  eFIREBASE_AUTH += char(EEPROM.read(i + 100));
                }
              }
            }
            httpheader();

            IPAddress lip = WiFi.localIP();
            String lipStr = String(lip[0]) + '.' + String(lip[1]) + '.' + String(lip[2]) + '.' + String(lip[3]);
            IPAddress gip = WiFi.gatewayIP();
            String gipStr = String(gip[0]) + '.' + String(gip[1]) + '.' + String(gip[2]) + '.' + String(gip[3]);
            IPAddress dns = WiFi.dnsIP();
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
            if (Kombistate == 1) {
              content += " Kombi [&] <br>";
            }
            if (Kombistate == 0) {
              content += " Kombi [_] <br>";
            }
            if (Kombistate == 3) {
              content += " Kombi [_] &nbsp;&nbsp;&nbsp; Kombi role offline <br>";
            }
            content += "istenen temp:&nbsp;";
            content += (String)usrtemp + "<br>";
            content += ((" Temperature: "));
            content += "  hic:" + (String)hic;
            content += ((" &#8451;"));
            content += (("/"));
            content += (String((1.8 * hic) + 32));
            content += ((" &#8457;"));
            content += "<br>";
            content += (("Humidity: "));
            content += " hum: " + (String)hum;
            content += "% <br>";
            content += "<br>";
            content += "<br>";
            content += eFIREBASE_HOSTA;
            content += "<br>";
            content += eFIREBASE_AUTH;
            content += "<br>";
            content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>";
            content += "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br></body>";
            content += "</html>";
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println(content);
            // Clear the header variable
            header = "";
            //client.stop();
          }

          if (header.indexOf("/hproto") > -1) {
            // firebase datas eeprom
            int Stage = 0;
            httpheader();
            content += "<table style=\"border:2px solid green;width:500px\"><tr><td style=\"border:1px solid black;width:250px\">";
            content += "<td style=\"border:1px solid black;width:250px\">";
            content += "Haberlesme protokolleri secimi";
            content += "<form action=\"/hprotoset\">";
            content += "<p>Please select connection protocol</p>";

            content += "<input type=\"radio\" id=\"MQTT\" name=\"hproto\" value=\"1\"";
            if (hproto == 1) {
              content += " checked=\"true\">";
            } else {
              content += ">";
            }
            content += "<label for=\"hproto\">MQTT</label><br>";

            content += "<input type=\"radio\" id=\"MQTT\" name=\"hproto\" value=\"2\"";
            if (hproto == 2) {
              content += " checked=\"true\">";
            } else {
              content += ">";
            }
            content += "<label for=\"hproto\">FiREBASE SERVER</label><br>";

            content += "<br><br>";
            content += "<input type=\"submit\" value=\"Submit\">";
            content += "</form>";
            content += "</td><td style=\"border:1px solid black;width:250px\">";
            content += " mqtt icin baglanti soyle boyle<br< boyle filan falan";
            content += "</td></tr></table>";
            content += "<br>";
            content += "</html>";




            if (header.indexOf(" /hprotoset") > -1) {

              Serial.println(header);
              if (header.indexOf("hproto=") > -1) {
                String buf = header.substring((header.indexOf("hproto=") + 7), (header.indexOf(" HTTP")));
                int str_len = buf.length() + 1;
                Serial.println(buf);
                char char_array[str_len];
                buf.toCharArray(char_array, str_len);


                for (int i = 0; i < buf.length() + 1; i++) {
                  Serial.print(char_array[i]);
                  if (char_array[i] == '|') {
                    EEPROM.write(i + 181, 0);
                  } else {
                    EEPROM.write(i + 181, char_array[i]);

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
                header = "";
                //client.stop();
                delay(500);
                ESP.reset();
              }
            }


            // kapanış html kısmı
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println(content);
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
            headerset = 0;
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
    // Clear the header variable
    header = "";
    // Close the connection
    delay(800);
    // remove bytes in the read buffer
    client.flush();  

    //client.stop();

    Serial.println("Client disconnected.");
    Serial.println("");

  }
  headerset = 0;

}


void wanipsorgula(void)
{ // kombiye bağlı esp içinde çalışacak
  timerxsayac = millis();
  if ((int(timerxsayac / 60000)) % 2 == 1 & oldtimerx < timerxsayac)
  {
    oldtimerx = timerxsayac + 60000;
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
    while (clienthttp.available())
    {
      line = clienthttp.readStringUntil('\n');
      wanip = line;
    }
    Serial.println(wanip);
  }
}

//-------------OTA OTA OTA START----------------------------------
void arduinoota(void) {
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

    display.clearDisplay();
    display.setCursor(8, 8);
  display.setTextSize(1);
    display.println("Yukleniyor:");
  String dis= (String)(progress / (total / 100));
  display.setCursor(32, 20);
  display.setTextSize(2);
  display.println(dis);
  display.drawRect(8, 44, 72, 44, BLACK);
  display.drawRect(8, 45, 5+((progress / (total / 100))/(1.5)), 45, BLACK);
  display.drawRect(8, 46, 5+((progress / (total / 100))/(1.5)), 46, BLACK);
  display.drawRect(8, 47, 72, 47, BLACK);
  display.display();
if((progress / (total / 100))==100){
  display.clearDisplay();
  display.setCursor(8, 8);
  display.setTextSize(1);
  display.println("Yaziliyor");
  display.display();
  }
    
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
  display.drawBitmap(0, 0,  Lan_off_logo8x16_glcd_bmp, 16, 8, 1);
  display.println();
  display.println("Modeme");
  display.println("Baglaniyor");
  display.display();
  while ( c < 6 ) {
    if (WiFi.status() == WL_CONNECTED)
    {
      display.drawBitmap(0, 0,  Lan_on_logo8x16_glcd_bmp, 16, 8, 1);
      display.display();
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
    st = "[ " + String(n) + " ] networks found<br>";
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
    buzzercal(2000, 3); delay(100);
    buzzercal(3000, 2); delay(10);
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
    buzzercal(3000, 50); delay(100);
    buzzercal(2500, 70); delay(100);
    buzzercal(1500, 100); delay(10);
  }
}
