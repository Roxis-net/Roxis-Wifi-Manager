/**************************************************************
   WiFiManager is a library for the ESP8266/Arduino platform
   (https://github.com/esp8266/Arduino) to enable easy
   configuration and reconfiguration of WiFi credentials using a Captive Portal
   inspired by:
   http://www.esp8266.com/viewtopic.php?f=29&t=2520
   https://github.com/chriscook8/esp-arduino-apboot
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
   Built by AlexT https://github.com/tzapu
   Licensed under MIT license
 **************************************************************/

#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <memory>

extern "C"
{
#include "user_interface.h"
}

const char HTTP_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"tr\"><head>  <meta charset=\"UTF-8\"><meta name=\"viewport\"content=\"width=device-width,initial-scale=1,user-scalable=no\" /><title>Roxis Desktop</title>";
const char HTTP_STYLE[] PROGMEM = "<style> :root{ --primarycolor: #0c1d43; }body{ background-color: #020712;border-top:14px solid white;height:100%;margin:0;background-position:center;background-repeat:no-repeat;background-size:cover; }body.invert,body.invert a,body.invert h1{ background-color: #020712;color:white; }body.invert.msg{ background-color: #020a1b;border-top:2px solid#020a1b;border-right:2px solid#020a1b;border-bottom:2px solid#020a1b;color: #fff; }body.invert.q[role=img] { -webkit-filter:invert(1);filter:invert(1); } .c,body{ text-align:center;font-family:verdana} .wrap{ text-align:left;display:inline-block;min-width:260px;max-width:500px; }div,input{ padding:5px;font-size:1em; /*width:100%;*/margin:5px 0;box-sizing:border-box; }input,button, .msg{ border-radius: .3rem;width:100%; }input[type=radio] {width:auto; }button,input[type=\"button\"],input[type=\"submit\"] {border:0;background-color:var(--primarycolor);color: #fff;line-height:2.4rem;font-size:1.2rem;padding:5px; }input[type=\"file\"] {border:5px solid var(--primarycolor); }a {color: #000;font-weight:700;text-decoration:none; }a:hover{ color:var(--primarycolor);text-decoration:underline; } .h{ display:none; } .q{ height:16px;margin:0;padding:0 5px;text-align:right;min-width:38px;float:right; } .q.q-0:after{ background-position-x:0; } .q.q-1:after{ background-position-x: -16px; } .q.q-2:after{ background-position-x: -32px; } .q.q-3:after{ background-position-x: -48px; } .q.q-4:after{ background-position-x: -64px; } .q.l:before{ background-position-x: -80px;padding-right:5px} .ql.q{ float:left; } .q:after, .q:before{ content: '';width:16px;height:16px;display:inline-block;background-repeat:no-repeat;background-position:16px 0;background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGAAAAAQCAMAAADeZIrLAAAAJFBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADHJj5lAAAAC3RSTlMAIjN3iJmqu8zd7vF8pzcAAABsSURBVHja7Y1BCsAwCASNSVo3/v+/BUEiXnIoXkoX5jAQMxTHzK9cVSnvDxwD8bFx8PhZ9q8FmghXBhqA1faxk92PsxvRc2CCCFdhQCbRkLoAQ3q/wWUBqG35ZxtVzW4Ed6LngPyBU2CobdIDQ5oPWI5nCUwAAAAASUVORK5CYII='); } @media(-webkit-min-device-pixel-ratio:2), (min-resolution:192dpi) { .q:before, .q:after{ background-image:url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALwAAAAgCAMAAACfM+KhAAAALVBMVEX///8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAOrOgAAAADnRSTlMAESIzRGZ3iJmqu8zd7gKjCLQAAACmSURBVHgB7dDBCoMwEEXRmKlVY3L//3NLhyzqIqSUggy8uxnhCR5Mo8xLt+14aZ7wwgsvvPA/ofv9+44334UXXngvb6XsFhO/VoC2RsSv9J7x8BnYLW+AjT56ud/uePMdb7IP8Bsc/e7h8Cfk912ghsNXWPpDC4hvN+D1560A1QPORyh84VKLjjdvfPFm++i9EWq0348XXnjhhT+4dIbCW+WjZim9AKk4UZMnnCEuAAAAAElFTkSuQmCC');background-size:95px 16px; } } .msg{ padding:20px;margin:20px 0;border:50px solid#eee;border-left-width:20px; /*border-left-color: #777;*/ } .msg h4{ margin-top:0;margin-bottom:5px; } .msg.P{ border-left-color:var(--primarycolor); } .msg.P h4{ color:var(--primarycolor); } .msg.S{ border-left-color: #5cb85c; } .msg.S h4{ color: #5cb85c; } .msg.D{ border-left-color: #dc3630; } .msg.D h4{ color: #dc3630; }dt{ font-weight:bold; }dd{ margin:0;padding:0 0 0.5em 0; }td{ vertical-align:top; }button.D{ background-color: #dc3630; }input:disabled{ opacity:0.5; } .vertical-menu{ height:300px;overflow-y:auto; } .vertical-menu a{ background-color: #eee;color:black;display:block;padding:8px;text-decoration:none; } .vertical-menu a:hover{ background-color: #ccc; } .vertical-menu a.active{ color:white; } .modal{ display:none; /*Hidden by default*/ } .titlem{ color: #174cbf; } .acces{ font-size:15px; }hr{ border:1px solid#174cbf; } .passInputM{ border:2px solid#24499a;color:white;background-color: #020a1b;padding:12px;margin-top:12px; } </style>";
const char HTTP_SCRIPT[] PROGMEM = "<script>var modal= document.getElementById('id01');var scroll= document.getElementById('id02');function c(l) {document.getElementById('s').value= l.innerText||l.textContent;document.getElementById('o').innerHTML= \"<h2 style='color: #fff;'>\" +l.innerText||l.textContent+ \"</h2>\";p =l.nextElementSibling.classList.contains('l');document.getElementById('p').disabled= !p;if(p)document.getElementById('p').focus() };function wifiSelectNone() {document.getElementById('id01').style.display= 'block';document.getElementById('id02').style.display= 'none'; }function wifiSelectShow() {document.getElementById('id02').style.display= 'block'; } </script>";
const char HTTP_HEADER_END[] PROGMEM = "</head><body class=\"invert\"><div style=\"margin:auto;color:white;background-color:white;height:13px;width:35px;margin-top:0px;\"></div><div style=\"margin-top: -22px;padding-top:0%;font-size:65px;\">&#9660;</div><div class='wrap'><img><svg style=\"width: 100px; display: block; margin-left: auto; margin-right: auto;\" xmlns=\"http://www.w3.org/2000/svg\" viewBox=\"0 0 134.06 99.29\"><defs><style>.cls-1{fill:#fff;}</style></defs><title>Asset 13</title><g id=\"Layer_2\" data-name=\"Layer 2\"><g id=\"Layer_1-2\" data-name=\"Layer 1\"><path class=\"cls-1\" d=\"M17.89,90.34V53.42c0-2-.43-3.25-1.3-3.82a5.06,5.06,0,0,0-2.79-.84H10.08V97.9h4.79v.88H0V97.9H4.66V1.64H0V.76H13.86a8.7,8.7,0,0,1,6.87,3,10.12,10.12,0,0,1,2.58,6.86V40.31A7.7,7.7,0,0,1,18.14,48q5.17,2.26,5.17,8.55V93.38c0,1.93.44,3.16,1.32,3.7a5.27,5.27,0,0,0,2.84.82l.38.88H23.12a4.19,4.19,0,0,1-3.95-2.45A12.87,12.87,0,0,1,17.89,90.34ZM10.08,47.88H13.8a3.8,3.8,0,0,0,3-1.26,4.42,4.42,0,0,0,1.11-3V6.05q0-4.41-4.09-4.41H10.08Z\"/><path class=\"cls-1\" d=\"M53.67,90.72a8.12,8.12,0,0,1-2.52,6.17,9.64,9.64,0,0,1-6.93,2.4,9.9,9.9,0,0,1-7.12-2.58,8.62,8.62,0,0,1-2.7-6.49V9.07a8.62,8.62,0,0,1,2.7-6.49A9.9,9.9,0,0,1,44.22,0,8.8,8.8,0,0,1,51,2.9a9.77,9.77,0,0,1,2.64,6.8ZM39.81,94a4.63,4.63,0,0,0,1.1,3.14,4.17,4.17,0,0,0,3.31,1.26,3.69,3.69,0,0,0,2.93-1.26A4.58,4.58,0,0,0,48.26,94V5.28q0-4.4-4-4.4A4.22,4.22,0,0,0,41,2.14a4.31,4.31,0,0,0-1.23,3.14Z\"/><path class=\"cls-1\" d=\"M77.28,43,88.83,97.65h4.78v.88h-15v-.88h4.8L74.61,55.38,65.52,97.65H70.3v.88H59v-.88h4.75l10-46.51L63.26,1.64h-4.8V.76H73.33v.88H68.54l7.78,36.82L84.25,1.64H79.5V.76H91v.88H86.18Z\"/><path class=\"cls-1\" d=\"M104.07,1.64v96h4.79v.88H94v-.88h4.66v-96H94V.76h14.87v.88Z\"/><path class=\"cls-1\" d=\"M132.55,54.18a11.12,11.12,0,0,1,1.51,5.8V90.22a8.63,8.63,0,0,1-2.71,6.49,9.9,9.9,0,0,1-7.12,2.58,9.64,9.64,0,0,1-6.93-2.4,8.12,8.12,0,0,1-2.52-6.17V64.26h5.42V94a4.54,4.54,0,0,0,1.1,3.14,3.7,3.7,0,0,0,2.94,1.26,4.25,4.25,0,0,0,3.18-1.26A4.34,4.34,0,0,0,128.64,94V60.09a7.93,7.93,0,0,0-.17-1.88,9.84,9.84,0,0,0-.84-1.89l-12-18.65c-.59-.83-.88-3-.88-6.42V9.07a8.6,8.6,0,0,1,2.71-6.49A9.89,9.89,0,0,1,124.61,0a8.71,8.71,0,0,1,6.8,3,10,10,0,0,1,2.65,6.87v22.3h-5.42V5.29q0-4.41-4-4.41a4.25,4.25,0,0,0-3.18,1.26,4.31,4.31,0,0,0-1.22,3.13V31.36c0,1,0,1.86.06,2.57a4.55,4.55,0,0,0,.62,2.07Z\"/></g></g></svg></img><p style=\"text-align:center;\">Kurulumu tamamlamak için lütfen aşağıdan wifi ağınızıseçiniz..</p><div style=\"height:50px;background-color: #174cbf;color: #fff;\"><div style=\"float:right;\"><form action='/wifi'method='get'><a style=\"background: #174cbf\"href=\"/\"><svg onclick=\"wifiSelectShow()\" style=\"width: 20px; height: 20px; padding-right: 12px;\"  xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:svgjs=\"http://svgjs.com/svgjs\" version=\"1.1\" width=\"512\" height=\"512\" x=\"0\" y=\"0\" viewBox=\"0 0 512 512\" style=\"enable-background:new 0 0 512 512\" xml:space=\"preserve\" class=\"\"><g><path xmlns=\"http://www.w3.org/2000/svg\" d=\"m61.496094 279.609375c-.988282-8.234375-1.496094-16.414063-1.496094-23.609375 0-107.402344 88.597656-196 196-196 50.097656 0 97 20.199219 131.5 51.699219l-17.300781 17.601562c-3.898438 3.898438-5.398438 9.597657-3.898438 15 1.800781 5.097657 6 9 11.398438 10.199219 3.019531.605469 102.214843 32.570312 95.898437 31.300781 8.035156 2.675781 19.917969-5.894531 17.703125-17.699219-.609375-3.023437-22.570312-113.214843-21.300781-106.902343-1.199219-5.398438-5.101562-9.898438-10.5-11.398438-5.097656-1.5-10.800781 0-14.699219 3.898438l-14.699219 14.398437c-45.300781-42.296875-107.503906-68.097656-174.101562-68.097656-140.699219 0-256 115.300781-256 256v.597656c0 8.457032.386719 14.992188.835938 19.992188.597656 6.625 5.480468 12.050781 12.003906 13.359375l30.816406 6.160156c10.03125 2.007813 19.050781-6.402344 17.839844-16.5zm0 0\" fill=\"#ffffff\" data-original=\"#000000\" style=\"\" class=\"\"/><path xmlns=\"http://www.w3.org/2000/svg\" d=\"m499.25 222.027344-30.90625-6.296875c-10.042969-2.046875-19.125 6.371093-17.890625 16.515625 1.070313 8.753906 1.546875 17.265625 1.546875 23.753906 0 107.398438-88.597656 196-196 196-50.097656 0-97-20.199219-131.5-52l17.300781-17.300781c3.898438-3.898438 5.398438-9.597657 3.898438-15-1.800781-5.101563-6-9-11.398438-10.199219-3.019531-.609375-102.214843-32.570312-95.898437-31.300781-5.101563-.898438-10.203125.601562-13.5 4.199219-3.601563 3.300781-5.101563 8.699218-4.203125 13.5.609375 3.019531 22.574219 112.210937 21.304687 105.898437 1.195313 5.402344 5.097656 9.902344 10.496094 11.398437 6.261719 1.570313 11.488281-.328124 14.699219-3.898437l14.402343-14.398437c45.296876 42.300781 107.5 69.101562 174.398438 69.101562 140.699219 0 256-115.300781 256-256v-.902344c0-6.648437-.242188-13.175781-.796875-19.664062-.570313-6.628906-5.433594-12.074219-11.953125-13.40625zm0 0\" fill=\"#ffffff\" data-original=\"#000000\" style=\"\" class=\"\"/></g></svg></a></form></div><h3 style=\"margin:10px 0px;text-align:left;padding-left:12px;\"onclick=\"wifiSelectShow()\">Kullanılabilir Ağlar</h3></div><hr><div id=\"id02\"onclick=\"wifiSelectNone()\"class=\"vertical-menu\">";
const char HTTP_PORTAL_OPTIONS[] PROGMEM = "";
const char HTTP_ITEM[] PROGMEM = "<div><a class=\"acces\"onclick='c(this)'>{v}<div role='img'aria-label='{r}%'title='{r}%'class='q q-{q} {i}'></div></a></div>";
const char HTTP_FORM_START[] PROGMEM = "";
const char HTTP_FORM_PARAM[] PROGMEM = "";
const char HTTP_FORM_END[] PROGMEM = "";
const char HTTP_SCAN_LINK[] PROGMEM = "";
const char HTTP_SAVED[] PROGMEM = "<div>Ayarlar Kaydedildi</div>";
const char HTTP_END[] PROGMEM = "</div></body></html>";

#ifndef WIFI_MANAGER_MAX_PARAMS
#define WIFI_MANAGER_MAX_PARAMS 10
#endif

class WiFiManagerParameter
{
public:
  /** 
        Create custom parameters that can be added to the WiFiManager setup web page
        @id is used for HTTP queries and must not contain spaces nor other special characters
    */
  WiFiManagerParameter(const char *custom);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length);
  WiFiManagerParameter(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);
  ~WiFiManagerParameter();

  const char *getID();
  const char *getValue();
  const char *getPlaceholder();
  int getValueLength();
  const char *getCustomHTML();

private:
  const char *_id;
  const char *_placeholder;
  char *_value;
  int _length;
  const char *_customHTML;

  void init(const char *id, const char *placeholder, const char *defaultValue, int length, const char *custom);

  friend class WiFiManager;
};

class WiFiManager
{
public:
  WiFiManager();
  ~WiFiManager();

  boolean autoConnect();
  boolean autoConnect(char const *apName, char const *apPassword = NULL);

  //if you want to always start the config portal, without trying to connect first
  boolean startConfigPortal();
  boolean startConfigPortal(char const *apName, char const *apPassword = NULL);

  // get the AP name of the config portal, so it can be used in the callback
  String getConfigPortalSSID();

  void resetSettings();

  //sets timeout before webserver loop ends and exits even if there has been no setup.
  //useful for devices that failed to connect at some point and got stuck in a webserver loop
  //in seconds setConfigPortalTimeout is a new name for setTimeout
  void setConfigPortalTimeout(unsigned long seconds);
  void setTimeout(unsigned long seconds);

  //sets timeout for which to attempt connecting, useful if you get a lot of failed connects
  void setConnectTimeout(unsigned long seconds);

  void setDebugOutput(boolean debug);
  //defaults to not showing anything under 8% signal quality if called
  void setMinimumSignalQuality(int quality = 8);
  //sets a custom ip /gateway /subnet configuration
  void setAPStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //sets config for a static IP
  void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn);
  //called when AP mode and config portal is started
  void setAPCallback(void (*func)(WiFiManager *));
  //called when settings have been changed and connection was successful
  void setSaveConfigCallback(void (*func)(void));
  //adds a custom parameter, returns false on failure
  bool addParameter(WiFiManagerParameter *p);
  //if this is set, it will exit after config, even if connection is unsuccessful.
  void setBreakAfterConfig(boolean shouldBreak);
  //if this is set, try WPS setup when starting (this will delay config portal for up to 2 mins)
  //TODO
  //if this is set, customise style
  void setCustomHeadElement(const char *element);
  //if this is true, remove duplicated Access Points - defaut true
  void setRemoveDuplicateAPs(boolean removeDuplicates);

private:
  std::unique_ptr<DNSServer> dnsServer;
  std::unique_ptr<ESP8266WebServer> server;

  //const int     WM_DONE                 = 0;
  //const int     WM_WAIT                 = 10;

  //const String  HTTP_HEADER = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"/><title>{v}</title>";

  void setupConfigPortal();
  void startWPS();

  const char *_apName = "no-net";
  const char *_apPassword = NULL;
  String _ssid = "";
  String _pass = "";
  unsigned long _configPortalTimeout = 0;
  unsigned long _connectTimeout = 0;
  unsigned long _configPortalStart = 0;

  IPAddress _ap_static_ip;
  IPAddress _ap_static_gw;
  IPAddress _ap_static_sn;
  IPAddress _sta_static_ip;
  IPAddress _sta_static_gw;
  IPAddress _sta_static_sn;

  int _paramsCount = 0;
  int _minimumQuality = -1;
  boolean _removeDuplicateAPs = true;
  boolean _shouldBreakAfterConfig = false;
  boolean _tryWPS = false;

  const char *_customHeadElement = "";

  //String        getEEPROMString(int start, int len);
  //void          setEEPROMString(int start, int len, String string);

  int status = WL_IDLE_STATUS;
  int connectWifi(String ssid, String pass);
  uint8_t waitForConnectResult();

  void handleRoot();
  void handleWifi(boolean scan);
  void handleWifiSave();
  void handleInfo();
  void handleReset();
  void handleNotFound();
  void handle204();
  boolean captivePortal();
  boolean configPortalHasTimeout();

  // DNS server
  const byte DNS_PORT = 53;

  //helpers
  int getRSSIasQuality(int RSSI);
  boolean isIp(String str);
  String toStringIp(IPAddress ip);

  boolean connect;
  boolean _debug = true;

  void (*_apcallback)(WiFiManager *) = NULL;
  void (*_savecallback)(void) = NULL;

  int _max_params;
  WiFiManagerParameter **_params;

  template <typename Generic>
  void DEBUG_WM(Generic text);

  template <class T>
  auto optionalIPFromString(T *obj, const char *s) -> decltype(obj->fromString(s))
  {
    return obj->fromString(s);
  }
  auto optionalIPFromString(...) -> bool
  {
    DEBUG_WM("NO fromString METHOD ON IPAddress, you need ESP8266 core 2.1.0 or newer for Custom IP configuration to work.");
    return false;
  }
};

#endif
