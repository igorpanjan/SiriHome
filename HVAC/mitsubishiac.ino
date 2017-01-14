// 2nd, January 2017 ESP8266 Mitsubishi AC control (Mitsubishi MFZ-KA Inverter)
//
// My home is full of devices from TVs, ACs, IPTV boxes, light switches, etc. and you have to push
// gazillion buttons to make them work. I really don't like physical switches or remotes, because you need to press them
// while you are, for example, watching a TV. We live in 2017 and it should be fairly easy to control everything
// by voice. So this is why I'm working on solutions to control everything in my home through Siri (Homebridge).
//
// Hardware setup:
// Raspberry Pi 2 works as a TV (Samsung) and IPTV box LIRC IR server. Same Raspberry also works as
// Homebridge server (https://github.com/nfarina/homebridge). One of plugins I'm using in Homebridge is
// http plugin (https://www.npmjs.com/package/homebridge-http) which also controls my EPS8266 web servers.
// I am using ESP8266 12e NodeMcu V3 (https://www.aliexpress.com/item/V3-Wireless-module-NodeMcu-4M-bytes-Lua-WIFI-Internet-of-Things-development-board-based-ESP8266-esp/32647542733.html?spm=2114.13010608.0.0.c6KwdC)
// These modules are super easy to work with. You can find it on Aliexpress for couple of EURs. 
//
// Resources - Mitsubishi IR codes and ESP8266 web server:
// The Mitsubishi IR code is based on Vincent Cruvellier's work 
// More information on Vincent's work:
// https://www.analysir.com/blog/2015/01/06/reverse-engineering-mitsubishi-ac-infrared-protocol/
// https://github.com/r45635/HVAC-IR-Control
// 
// Resources on ESP8266 web server - passing multiple parameters in URL:
// https://techtutorialsx.wordpress.com/2016/10/22/esp8266-webserver-getting-query-parameters/
// ESP8266 web server is based on Nuno Santos's code 
// 
//
// Hardware Connection
//  IR LED SIGNAL => ESP/GPIO_4
//
// URL construction:
// http://192.168.1.216/genericArgs?HvacMode=1&temp=25&HvacFanMode=3&HvacVanneMode=3&HvacStatus=1
// 
// HvacMode
// 1 - HOT
// 2 - COLD
// 3 - DRY
// 4 - AUTO
// 
// HvacFanMode
// 1 - 5 - Fan speed 1 - 5
// 6     - Fan speed AUTO
// 7     - Fan speed SILENT
//
// HvacVanneMode
// 0     - Vanne AUTO
// 1 - 5 - Vanne 1-5
// 6     - Vanne AUTO_MOVE
//
// HvacStatus = 0 - HVAC ON
// HvacStatus = 1 - HVAC OFF



#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

const char* ssid = "YOUR_SSID";           // wifi ssid
const char* password = "PASSWORD";  // wifi password

int halfPeriodicTime;
int IRpin;
int khz;


// HVAC MITSUBISHI_
#define HVAC_MITSUBISHI_HDR_MARK    3400
#define HVAC_MITSUBISHI_HDR_SPACE   1750
#define HVAC_MITSUBISHI_BIT_MARK    450
#define HVAC_MITSUBISHI_ONE_SPACE   1300
#define HVAC_MISTUBISHI_ZERO_SPACE  420
#define HVAC_MITSUBISHI_RPT_MARK    440
#define HVAC_MITSUBISHI_RPT_SPACE   17100 // Above original iremote limit


/****************************************************************************
/* Send IR command to Mitsubishi HVAC - sendHvacMitsubishi
/***************************************************************************/
void sendHvacMitsubishi(
  int                     HVAC_Mode,           // Example HVAC_HOT  HvacMitsubishiMode
  int                     HVAC_Temp,           // Example 21  (°c)
  int                     HVAC_FanMode,        // Example FAN_SPEED_AUTO  HvacMitsubishiFanMode
  int                     HVAC_VanneMode,      // Example VANNE_AUTO_MOVE  HvacMitsubishiVanneMode
  int                     OFF                  // Example false
)
{

//#define  HVAC_MITSUBISHI_DEBUG;  // Un comment to access DEBUG information through Serial Interface

  byte mask = 1; //our bitmask
  byte data[18] = { 0x23, 0xCB, 0x26, 0x01, 0x00, 0x20, 0x08, 0x06, 0x30, 0x45, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F };
  // data array is a valid trame, only byte to be chnaged will be updated.

  byte i;

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 18; i++) {
    Serial.print("_");
    Serial.print(data[i], HEX);
  }
  Serial.println(".");
#endif

  // Byte 6 - On / Off
  if (OFF) {
    data[5] = (byte) 0x0; // Turn OFF HVAC
  } else {
    data[5] = (byte) 0x20; // Tuen ON HVAC
  }

  // Byte 7 - Mode
  switch (HVAC_Mode)
  {
    case 1:  data[6] = (byte) 0x08; break;  // HVAC_HOT
    case 2:  data[6] = (byte) 0x18; break;  // HVAC_COLD
    case 3:  data[6] = (byte) 0x10; break;  // HVAC_DRY
    case 4:  data[6] = (byte) 0x20; break;  // HVAC_AUTO
    default: break;
  }

  // Byte 8 - Temperature
  // Check Min Max For Hot Mode
  byte Temp;
  if (HVAC_Temp > 31) { Temp = 31;}
  else if (HVAC_Temp < 16) { Temp = 16; } 
  else { Temp = HVAC_Temp; };
  data[7] = (byte) Temp - 16;

  // Byte 10 - FAN / VANNE
  switch (HVAC_FanMode)
  {
    case 1:       data[9] = (byte) B00000001; break;  // FAN_SPEED_1
    case 2:       data[9] = (byte) B00000010; break;  // FAN_SPEED_2
    case 3:       data[9] = (byte) B00000011; break;  // FAN_SPEED_3
    case 4:       data[9] = (byte) B00000100; break;  // FAN_SPEED_4
    case 5:       data[9] = (byte) B00000100; break;  //No FAN speed 5 for MITSUBISHI so it is consider as Speed 4
    case 6:       data[9] = (byte) B10000000; break;  // FAN_SPEED_AUTO
    case 7:       data[9] = (byte) B00000101; break;  // FAN_SPEED_SILENT
    default: break;
  }

  switch (HVAC_VanneMode)
  {
    case 0:          data[9] = (byte) data[9] | B01000000; break; // VANNE_AUTO
    case 1:          data[9] = (byte) data[9] | B01001000; break; // VANNE_H1
    case 2:          data[9] = (byte) data[9] | B01010000; break; // VANNE_H2
    case 3:          data[9] = (byte) data[9] | B01011000; break; // VANNE_H3
    case 4:          data[9] = (byte) data[9] | B01100000; break; // VANNE_H4
    case 5:          data[9] = (byte) data[9] | B01101000; break; // VANNE_H5
    case 6:          data[9] = (byte) data[9] | B01111000; break; // VANNE_AUTO_MOVE
    default: break;
  }

  // Byte 18 - CRC
  data[17] = 0;
  for (i = 0; i < 17; i++) {
    data[17] = (byte) data[i] + data[17];  // CRC is a simple bits addition
  }

#ifdef HVAC_MITSUBISHI_DEBUG
  Serial.println("Packet to send: ");
  for (i = 0; i < 18; i++) {
    Serial.print("_"); Serial.print(data[i], HEX);
  }
  Serial.println(".");
  for (i = 0; i < 18; i++) {
    Serial.print(data[i], BIN); Serial.print(" ");
  }
  Serial.println(".");
#endif

  enableIROut(38);  // 38khz
  space(0);
  for (int j = 0; j < 2; j++) {  // For Mitsubishi IR protocol we have to send two time the packet data
    // Header for the Packet
    mark(HVAC_MITSUBISHI_HDR_MARK);
    space(HVAC_MITSUBISHI_HDR_SPACE);
    for (i = 0; i < 18; i++) {
      // Send all Bits from Byte Data in Reverse Order
      for (mask = 00000001; mask > 0; mask <<= 1) { //iterate through bit mask
        if (data[i] & mask) { // Bit ONE
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MITSUBISHI_ONE_SPACE);
        }
        else { // Bit ZERO
          mark(HVAC_MITSUBISHI_BIT_MARK);
          space(HVAC_MISTUBISHI_ZERO_SPACE);
        }
        //Next bits
      }
    }
    // End of Packet and retransmission of the Packet
    mark(HVAC_MITSUBISHI_RPT_MARK);
    space(HVAC_MITSUBISHI_RPT_SPACE);
    space(0); // Just to be sure
  }
}

/****************************************************************************
/* enableIROut : Set global Variable for Frequency IR Emission
/***************************************************************************/ 
void enableIROut(int khz) {
  // Enables IR output.  The khz value controls the modulation frequency in kilohertz.
  halfPeriodicTime = 500/khz; // T = 1/f but we need T/2 in microsecond and f is in kHz
}

/****************************************************************************
/* mark ( int time) 
/***************************************************************************/ 
void mark(int time) {
  // Sends an IR mark for the specified number of microseconds.
  // The mark output is modulated at the PWM frequency.
  long beginning = micros();
  while(micros() - beginning < time){
    digitalWrite(IRpin, HIGH);
    delayMicroseconds(halfPeriodicTime);
    digitalWrite(IRpin, LOW);
    delayMicroseconds(halfPeriodicTime); //38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
  }
}

/****************************************************************************
/* space ( int time) 
/***************************************************************************/ 
/* Leave pin off for time (given in microseconds) */
void space(int time) {
  // Sends an IR space for the specified number of microseconds.
  // A space is no output, so the PWM output is disabled.
  digitalWrite(IRpin, LOW);
  if (time > 0) delayMicroseconds(time);
}

/****************************************************************************
/* sendRaw (unsigned int buf[], int len, int hz)
/***************************************************************************/ 
void sendRaw (unsigned int buf[], int len, int hz)
{
  enableIROut(hz);
  for (int i = 0; i < len; i++) {
    if (i & 1) {
      space(buf[i]);
    } 
    else {
      mark(buf[i]);
    }
  }
  space(0); // Just to be sure
}


void handleGenericArgs() 
{ //Handler

  // The easiest way to pass parameters for sendHVACMitsubishi function is through int
  int hvacModeUrl;
  int temp;
  int hvacFanModeUrl;
  int hvacVanneModeUrl;
  int hvacStatusUrl;
  
  String message = "Number of args received:";
  message += server.args();            //Get number of parameters
  message += "\n";                     //Add a new line

  for (int i = 0; i < server.args(); i++) 
  {

    message += "Arg nº" + (String)i + " –> ";   // Include the current iteration value
    message += server.argName(i) + ": ";        // Get the name of the parameter
    message += server.arg(i) + "\n";            // Get the value of the parameter

    if (server.argName(i) == "HvacMode")
    {
      hvacModeUrl = server.arg(i).toInt();
      Serial.println(hvacModeUrl);
    }

    if (server.argName(i) == "temp")
    {
      temp = server.arg(i).toInt();
      Serial.println(temp);
    }

    if (server.argName(i) == "HvacFanMode")
    {
      hvacFanModeUrl = server.arg(i).toInt();
      Serial.println(hvacFanModeUrl);
    }

    if (server.argName(i) == "HvacVanneMode")
    {
      hvacVanneModeUrl = server.arg(i).toInt();
      Serial.println(hvacVanneModeUrl);
    }

    if (server.argName(i) == "HvacStatus")
    {
      hvacStatusUrl = server.arg(i).toInt();
      Serial.println(hvacStatusUrl);
    }

  } 

  server.send(200, "text/plain", message);       //Response to the HTTP request

  sendHvacMitsubishi(hvacModeUrl, temp, hvacFanModeUrl, hvacVanneModeUrl, hvacStatusUrl);

}


/****************************************************************************
/* setup ()
*  
*/
/***************************************************************************/
void setup() {
  // put your setup code here, to run once:
  IRpin=4;
  khz=38;
  halfPeriodicTime = 500/khz;
  pinMode(IRpin, OUTPUT);
  Serial.begin(115200);
  
  Serial.println("Demo Started.");
  Serial.println("Connecting to WiFi network....");
  Serial.println(ssid);
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }

  Serial.println(""); 
  Serial.println("WiFi connected"); 
   
  Serial.println("Server started"); 
  // Print the IP address 
  Serial.print(WiFi.localIP()); 

  server.on("/genericArgs", handleGenericArgs);
  server.begin();
  Serial.println("");
  Serial.println("Server listening");
}

/****************************************************************************
/* Loop ()
/***************************************************************************/
void loop() {

  server.handleClient();
}
