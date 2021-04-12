// Credits: Based on LaserWeb codebase, https://github.com/LaserWeb/LaserWeb3-ESP8266

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>    //https://github.com/Links2004/arduinoWebSockets/issues/61
#include <Hash.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <SoftwareSerial.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Transparent Serial Bridge code from Marcus https://github.com/Links2004/arduinoWebSockets/issues/61

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
const char* update_path = "/firmware";
const char* update_username = "admin";
const char* update_password = "admin";
String degisken ;
int wrt_ip = 0 ;
WebSocketsServer webSocket = WebSocketsServer(81);


WiFiManager wifiManager;

bool socketConnected = false;

#define SEND_SERIAL_TIME (50)
SoftwareSerial testSerial;



class SerialTerminal {
  public:
    void setup() {
      _lastRX = 0;
      resetBuffer();
      Serial.begin(115200);
    }

    void loop() {
      ArduinoOTA.handle();
      unsigned long t = millis();
      bool forceSend = false;

      size_t len = (_bufferWritePtr - &_buffer[0]);
      int free = (sizeof(_buffer) - len);

      int available = Serial.available();
      if (available > 0 && free > 0) {
        int readBytes = available;
        if (readBytes > free) {
          readBytes = free;
        }
        readBytes = Serial.readBytes(_bufferWritePtr, readBytes);
        _bufferWritePtr += readBytes;
        _lastRX = t;
      }

      // check for data in buffer
      len = (_bufferWritePtr - &_buffer[0]);
      if (len >=  sizeof(_buffer)) {
        forceSend = true;
      }
      if (len > (WEBSOCKETS_MAX_HEADER_SIZE + 1)) {
        if (((t - _lastRX) > SEND_SERIAL_TIME) || forceSend) {
          webSocket.broadcastTXT(&_buffer[0], (len - WEBSOCKETS_MAX_HEADER_SIZE), true);
          resetBuffer();
        }
      }
    }


  protected:
    uint8_t _buffer[1460];
    uint8_t * _bufferWritePtr;
    unsigned long _lastRX;

    void resetBuffer() {
      // offset for adding Websocket header
      _bufferWritePtr = &_buffer[WEBSOCKETS_MAX_HEADER_SIZE];
      // addChar('T');
    }

    inline void addChar(char c) {
      *_bufferWritePtr = (uint8_t) c; // message type for Webinterface
      _bufferWritePtr++;
    }
};

SerialTerminal term;
uint8_t globalNum = 0;
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght)
{

  globalNum = num;
  if (type == WStype_DISCONNECTED)
  {
    testSerial.print("t0.txt=");
    testSerial.print("\"");
    testSerial.print("Bağlantı Koptu");
    testSerial.print("\"");
    testSerial.write(0xff);
    testSerial.write(0xff);
    testSerial.write(0xff);
    socketConnected = false;
  }
  else if (type == WStype_CONNECTED)
  {
    socketConnected = true;
    webSocket.sendTXT(num, "Connected\n");
    testSerial.print("t0.txt=");
    testSerial.print("\"");
    testSerial.print("HAZIR");
    testSerial.print("\"");
    testSerial.write(0xff);
    testSerial.write(0xff);
    testSerial.write(0xff);



  }
  else if (type == WStype_TEXT)
  {
    if (lenght > 0)
    {
      String command = String((const char *)payload);
      //  SendFormat(num, "Nodemcu gelen veri:    %s\n", payload);
      command.toUpperCase();
      if (command == "WIFIRESET\n")
      {
        webSocket.sendTXT(num, "ok\n");
        webSocket.sendTXT(num, "Resetting WiFi settings!\n");
        delay(500);
        wifiManager.resetSettings();
        delay(100);
        ESP.restart();
      }
      else if (command == "WIFISTATUS\n")
      {
        webSocket.sendTXT(num, "ok\n");

        char buffer[100];      //declare array
        IPAddress local = WiFi.localIP();
        IPAddress gatew = WiFi.gatewayIP();

        SendFormat(num, "Connected to:    %s\n", WiFi.SSID().c_str());
        SendFormat(num, "Signal strength: %ddBm\n", WiFi.RSSI());
        SendFormat(num, "Local IP:        %d.%d.%d.%d\n", local[0], local[1], local[2], local[3]);
        SendFormat(num, "Gateway IP:      %d.%d.%d.%d\n", gatew[0], gatew[1], gatew[2], gatew[3]);
      }
      else if (command == "CHIPSTATUS\n")
      {
        webSocket.sendTXT(num, "ok\n");
        uint32_t realSize = ESP.getFlashChipRealSize();
        uint32_t ideSize = ESP.getFlashChipSize();
        FlashMode_t ideMode = ESP.getFlashChipMode();

        SendFormat(num, "CPU Freq:        %uMHz\n", ESP.getCpuFreqMHz());
        SendFormat(num, "Flash speed:     %uMHz\n", ESP.getFlashChipSpeed() / 1000000);
        SendFormat(num, "Flash real size: %uKB\n", realSize / 1024);
        SendFormat(num, "Flash ide  size: %uKB\n", ideSize / 1024);
        SendFormat(num, "Flash ide  mode: %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
        SendFormat(num, "Flash ID:        %08X\n", ESP.getFlashChipId());
      }
      else
      {
        Serial.write((const char *) (payload), (lenght));
      }
    }
  }

}

void SendFormat (uint8_t num, char * format, ...)
{
  char buffer[100];
  va_list args;
  va_start (args, format);
  vsnprintf (buffer, 100, format, args);
  va_end (args);
  webSocket.sendTXT(num, buffer);
}

void setup()
{
  delay(5000); //BOOT WAIT
  Serial.begin(115200);

  testSerial.begin(9600, SWSERIAL_8N1, 2, 0, false, 95, 11);

  Serial.print("baud=9600");
  testSerial.write(0xff);
  testSerial.write(0xff);
  testSerial.write(0xff);

  Serial.println("start");
  testSerial.print("t0.txt=");
  testSerial.print("\"");
  testSerial.print( WiFi.SSID());
  testSerial.print("Baglaniyor...");
  testSerial.print("\"");
  testSerial.write(0xff);
  testSerial.write(0xff);
  testSerial.write(0xff);

  //     wifiManager.resetSettings();
  wifiManager.setDebugOutput(true);
  Serial.println("start");
  if (wifiManager.autoConnect("ESP8266")) {

    Serial.println(WiFi.SSID());
    //Serial.println( "Signal strength: %ddBm\n", WiFi.RSSI());
    //      Serial.println( "Local IP:        %d.%d.%d.%d\n", local[0], local[1], local[2], local[3]);
    //      Serial.println( "Gateway IP:      %d.%d.%d.%d\n", gatew[0], gatew[1], gatew[2], gatew[3]);
  }
  Serial.println(WiFi.SSID());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  httpUpdater.setup(&httpServer, update_path, update_username, update_password);
  httpServer.begin();

  term.setup();

  WiFi.setSleepMode(WIFI_NONE_SLEEP); // disable WiFi sleep for more performance

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
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  testSerial.print("t0.txt=");
  testSerial.print("\"");
  testSerial.print("IP Adresi: ");
  testSerial.print(WiFi.localIP());
  testSerial.print("\"");
  testSerial.write(0xff);
  testSerial.write(0xff);
  testSerial.write(0xff);


}

void GetDataTestSerial(){

   if (testSerial.available() > 0) {
    degisken = testSerial.readString();
//    Serial.println(degisken);
    if (degisken.length() > 1) {
      String sub = degisken.substring(0, 3);
//      Serial.print("debug->sub->");
//      Serial.println(sub);
      if (sub.equals("D15")) {
        //     Serial.print("$J=G53X0.0F500\n");
      } else if ( sub.equals("D16")) {
        //   Serial.print("D25\n");
      } else if ( sub.equals("D17")) {
        //Serial.print("$J=G53Y0.0F500\n");
      } else if ( sub.equals("D18")) {
        //    Serial.print("D25\n");
      } else if ( sub.equals("D19")) {
        //    Serial.print("$J=G53X200.0F500\n");
      } else if ( sub.equals("D20")) {
        //     Serial.print("D25\n");
      } else if ( sub.equals("D21")) {
        //   Serial.print("$J=G53Y200.0F500\n");
      } else if ( sub.equals("D22")) {
        //    Serial.print("D25\n");
      } else {
     //   webSocket.sendTXT(globalNum, degisken);
      }
        webSocket.sendTXT(globalNum, degisken);




      //  webSocket.sendTXT(globalNum, degisken);
      //  int dgs = sizeof(degisken);
      //  String dggs = String(dgs);
      //  webSocket.sendTXT(globalNum, dggs);

      // uint8_t dataArray[degisken.length()];
      // degisken.toCharArray(dataArray, degisken.length());
      // Serial.write((const char *) (dataArray), (degisken.length()));

      yield();
      if (degisken.substring(0, 2) == "D6") {
        testSerial.print("t0.txt=");
        testSerial.print("\"");
        testSerial.print("Wi-Fi Yeniden Baslatiliyor...");
        testSerial.print("\"");
        testSerial.write(0xff);
        testSerial.write(0xff);
        testSerial.write(0xff);
        wifiManager.resetSettings();
        ESP.restart();
      }


    }
  }
}

void loop()
{
  GetDataTestSerial();
 //Serial.println("loop");
 
  ArduinoOTA.handle();
  term.loop();
  webSocket.loop();
  httpServer.handleClient();
}
