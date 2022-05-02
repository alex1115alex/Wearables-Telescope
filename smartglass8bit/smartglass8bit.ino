#include <ESP_8_BIT_GFX.h>
//#include "libs/ESP_8_BIT_Color_Composite_Video_Library/ESP_8_BIT_GFX.h"
#include <BluetoothSerial.h>
#include "BluetoothSerial.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_bt.h"
#include "esp32-hal-bt.h"
#include <ArduinoJson.h>
#include <TinyPICO.h>
#include <cstddef>
#include <Fonts/FreeSans18pt7b.h>
#include "myfont6pt7b.h"

using std::vector;
#include <vector>
#include <ESP32Time.h>
/* Check if Bluetooth configurations are enabled in the SDK */
/* If not, then you have to recompile the SDK */
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// APA102 Dotstar
#define DOTSTAR_PWR 13
#define DOTSTAR_DATA 2
#define DOTSTAR_CLK 12

// Battery
#define BAT_CHARGE 34
#define BAT_VOLTAGE 35

class Notification
{
  public:
    String title;
    String content;
    String notificationTime;

    Notification(String t, String c, String nt)
    {
      title = t;
      content = c;
      notificationTime = nt;
    }
};

class Theme
{
  public:
    int textColor;
    int backgroundColorPrimary;
    int backgroundColorSecondary;
    int flair;

    Theme(int text, int prim, int second, int fla)
    {
      textColor = text;
      backgroundColorPrimary = prim;
      backgroundColorSecondary = second;
      flair = fla;
    }
};

vector<Notification> notifications;

const int maxW = 255;
const int maxH = 239;

// A list of 8-bit color values that work well in a cycle.
uint8_t colorCycle[] = {
  0xFF, // White
  0xFE, // Lowering blue
  0xFD,
  0xFC, // No blue
  0xFD, // Raising blue
  0xFE,
  0xFF, // White
  0xF3, // Lowering green
  0xE7,
  0xE3, // No green
  0xE7, // Raising green
  0xF3,
  0xFF, // White
  0x9F, // Lowering red
  0x5F,
  0x1F, // No red
  0x5F, // Raising red
  0x9F,
  0xFF
};

//define some colors
const int lightPink = 0xF7;
const int white = 0xFF;
const int black = 0x00;
const int red = 0xE0;
const int maroon = 0x20;
const int cyberBlue = 0x13;
const int orange = 0xEC;
const int yummy = 0xE5;
const int darkPurple = 0x41;

Theme themeLight = Theme(black, white, lightPink, red);
Theme themeDark = Theme(white, black, maroon, cyberBlue);
Theme themeCyberpunk = Theme(orange, cyberBlue, red, red);
Theme currentTheme = themeLight;

const String deviceName = "ESP32test";

ESP_8_BIT_GFX videoOut(true /* = NTSC */, 8 /* = RGB332 color */);
ESP32Time rtc;
BluetoothSerial SerialBT;
TinyPICO tp = TinyPICO();

bool usbPluggedIn;
int buttonPressed;
bool buttonReset = true;
int currentTime = 0;
int currentTemperature = -1;
int currentNotification = -1;
String currentSongName = "";
bool isCharging = false;
bool bluetoothConnected = false;
int speed;
int rpm;
String teleprompterData = "No teleprompter data";

void setup() {
  Serial.begin(115200);
  tp.DotStar_SetPower(false); //Turn off TinyPICO RGB LED to save power
  pinMode(9, INPUT); //For checking if USB is plugged in
  pinMode(22, INPUT_PULLUP); //For button
  usbPluggedIn = digitalRead(9);
  buttonPressed = digitalRead(22);

  //We setup bluetooth, free memory pertaining to BLE, and then start. This is necessary.
  btStarted();
  esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  bt_cfg.mode = ESP_BT_MODE_CLASSIC_BT; // this line is important
  esp_bt_controller_init(&bt_cfg);
  esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT); // has to match the config

  rtc.setTime(00, 00, 00, 1, 1, 1970);  // 17th Jan 2021 15:24:30

  SerialBT.register_callback(disconnectCallback);
  SerialBT.begin(deviceName);
  Serial.println("The device started, now you can pair it with bluetooth!");
  videoOut.begin();

  printDeviceAddress();

  //custom backwards font because this is somehow the easiest way to mirror the screen ...
  videoOut.setFont(&myfont6pt7b);
}


//-5 SYSTEM POPUP
//-4 CONNECT BLUETOOTH
//-3 TELEPROMPTER
//-2 CAR
//-1 HOME SCREEN
//1->x NOTIFICATIONS
void nextButton(bool alsoDeleteNotif = false) {
  if (currentNotification == -2 || currentNotification == -3 || currentNotification == -5)
  {
    //Reserved for apps
    currentNotification = -1;
  }
  else if (currentNotification == -1 && !notifications.empty())
  {
    currentNotification = notifications.size() - 1;
  }
  else if (currentNotification >= 0) {
    currentNotification--;

    if (alsoDeleteNotif)
      notifications.pop_back();
  }
}

void loop() {
  //Serial.println("Port22: ");
  //Serial.print(digitalRead(22));
  isCharging = tp.IsChargingBattery();
  if(usbPluggedIn != digitalRead(9))
  {
    ESP.restart();
  }
  Serial.println(touchRead(32));
  //if buttonPressed is off AND the button is pressed => PressNext, set buttonPressed to on.
  if(buttonPressed == 1 && (touchRead(32) < 20))
  {
      buttonPressed = 0;
      nextButton();
  }
  else if(buttonPressed == 0 && (touchRead(32) >= 50))
  {
    buttonPressed = 1;
  }
  
  //Serial.println("Is the battery charging? :" + String(isCharging));
  //TODO: if 30 seconds has passed, reset currentNotificaion = -1

  //if we have information to send from serial port
  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  //if information is available from BT serial port
  if (SerialBT.available()) {

    String input = SerialBT.readString();
    Serial.println(input);

    DynamicJsonDocument doc(512);
    auto error = deserializeJson(doc, input);

    if(error)
    {
      drawSystemPopup("JSON error");
    }
    else
    {
      
    JsonObject obj = doc.as<JsonObject>();

    
      if (obj["type"] == "notif") {
        notifications.push_back(Notification(obj["title"], obj["content"], obj["time"]));
        currentNotification = notifications.size() - 1;
      }
      else if (obj["type"] == "sync")
      {
        Serial.println("syncing!");
        int hours = obj["hours"].as<int>();
        int minutes = obj["minutes"].as<int>();
        int seconds = obj["seconds"].as<int>();
        rtc.setTime(seconds, minutes, hours, 17, 1, 2022);
        String printableTime = String(rtc.getHour()) + ":" + String(rtc.getMinute());
        Serial.println("newtime: " + printableTime);
  
        currentTemperature = obj["temperature"].as<int>();
      }
      else if (obj["type"] == "next")
      {
        if (obj["dismiss"])
          nextButton(true);
        else
          nextButton(false);
      }
      else if (obj["type"] == "song")
      {
        currentSongName = obj["name"].as<String>();
      }
      else if (obj["type"] == "car")
      {
        speed = obj["speed"].as<int>();
        rpm = obj["rpm"].as<int>();
        currentNotification = -2;
      }
      else if (obj["type"] == "theme")
      {
        String id = obj["id"];
        if(id == "light") currentTheme = themeLight;
        else if (id == "dark") currentTheme = themeDark;
        else if (id == "cyberpunk") currentTheme = themeCyberpunk;
      }
      else if (obj["type"] == "teleprompter")
      {
        teleprompterData = obj["data"].as<String>();
        currentNotification = -3;
      }
    }
  }
  delay(20);

//-5 SYSTEM POPUP
//-4 CONNECT BLUETOOTH
//-3 TELEPROMPTER
//-2 CAR
//-1 HOME SCREEN
//1->x NOTIFICATIONS

  //HANDLE VIDEO OUTPUT
  videoOut.waitForFrame();

  if (!bluetoothConnected){
    videoOut.setTextWrap(true);
    drawMiddle("Connect Bluetooth");
    currentNotification = -4;
    }
  else if (currentNotification == -3)
    drawTeleprompter();
  else if (currentNotification == -1) //home screen
    drawHomeScreen();
  else if (currentNotification == -2)
    drawCarScreen();
  else if (currentNotification > -1)
    drawNotification(notifications[currentNotification]);
  
  //Serial.println(ESP.getFreeHeap());
}

int getHorPosForText(String str, int textSize)
{
  return maxW / 2 - ((str.length() * textSize * 5) / 2);
}

int getVertPosForText(String str, int textSize)
{
  return maxH / 2 - (textSize * 7) / 2;
}

String addPrefixZero(String str) {
  if (str.length() == 1) {
    return "0" + str;
  }
  return str;
}
String getPrintableTime(){
  String printableTime = addPrefixZero(String(rtc.getHour())) + ":" + addPrefixZero(String(rtc.getMinute()));
  if (rtc.getHour(true) > 12) printableTime += " pm"; else printableTime += " am";
  return printableTime;
}

void drawSystemPopup(String str)
{
  currentNotification = -5;
  drawMiddle(str);
}

void drawCarScreen()
{
  if(speed == NULL || rpm == NULL)
  {
    Serial.println("Speed or RPM are null when trying to draw car screen...");
    return;
  }
  videoOut.fillScreen(0);
  videoOut.setCursor(0, 0);
  videoOut.setTextWrap(true);

  videoOut.fillRect(0, 0, maxW, maxH, currentTheme.backgroundColorPrimary);

  videoOut.setTextColor(currentTheme.flair);
  videoOut.setTextSize(1);
  videoOut.setCursor(5, 5);
  vidPrint(getPrintableTime());

  videoOut.setTextColor(currentTheme.textColor);
  videoOut.setTextSize(3);
  String speedString = String(speed) + " mph";
  videoOut.setCursor(getHorPosForText(speedString, 3), getVertPosForText(speedString, 3) - 35);
  vidPrint(speedString);

  String rpmString = String(rpm) + " rpm";
  videoOut.setCursor(getHorPosForText(rpmString, 3), getVertPosForText(rpmString, 3) + 35);
  vidPrint(rpmString);
}

void drawHomeScreen()
{
  //Serial.print("draw home screen");
  videoOut.fillScreen(0);
  videoOut.setCursor(0, 0);
  videoOut.setTextWrap(true);

  videoOut.fillRect(0, 0, maxW, maxH, currentTheme.backgroundColorPrimary);
  
  videoOut.setTextColor(currentTheme.flair);
  videoOut.setTextSize(1);
  videoOut.setCursor(5, 5);
  vidPrint("TelescopeOS v0.1");

  videoOut.setTextColor(currentTheme.textColor);
  videoOut.setTextSize(3);
  String printableTime = getPrintableTime();
  videoOut.setCursor(getHorPosForText(printableTime, 3), getVertPosForText(printableTime, 3));
  vidPrint(printableTime);

  if (notifications.size() != 0) {
    videoOut.setTextSize(2);
    String notifString = String(notifications.size()) + " Notification";
    if (notifications.size() != 1) notifString += "s";
    videoOut.setCursor(getHorPosForText(notifString, 2), maxH / 2 + 50);
    vidPrint(notifString);
  }
}

void drawNotification(Notification notification)
{
  int dividerH = maxH / 3;

  // Clear screen
  videoOut.fillScreen(0);
  videoOut.setCursor(0, 0);

  videoOut.setTextColor(currentTheme.textColor);
  videoOut.setTextSize(2);
  videoOut.setTextWrap(true);

  //top rect, title
  videoOut.fillRect(0, 0, maxW, dividerH, currentTheme.backgroundColorSecondary);

  videoOut.setCursor(5, 30);
  vidPrint(notification.notificationTime);

  videoOut.setCursor(5, 50);
  vidPrint(notification.title);

  //bottom rect, content
  videoOut.setCursor(0, 0);
  videoOut.fillRect(0, dividerH, maxW, maxH - dividerH, currentTheme.backgroundColorPrimary);
  videoOut.setCursor(5, dividerH + 15);
  vidPrint(notification.content);

  //draw divider
  videoOut.drawLine(0, dividerH, maxW, dividerH, currentTheme.flair);
}

void drawTeleprompter(){
  videoOut.fillScreen(0);
  videoOut.setCursor(0,0);
  videoOut.fillRect(0, 0, maxW, maxH, currentTheme.backgroundColorPrimary);
  
  videoOut.setCursor(5, 40);
  videoOut.setTextColor(currentTheme.textColor);
  videoOut.setTextSize(2);
  vidPrint(teleprompterData);
}

void drawMiddle(String str) {
  videoOut.fillScreen(0);
  videoOut.setCursor(0, 0);
  videoOut.setTextColor(currentTheme.textColor);
  videoOut.setTextSize(2);
  videoOut.setTextWrap(true);
  videoOut.fillRect(0, 0, maxW, maxH, currentTheme.backgroundColorPrimary);
  videoOut.setCursor(getHorPosForText(str, 2), getVertPosForText(str, 2));
  vidPrint(str);
}

void printDeviceAddress() {

  const uint8_t* point = esp_bt_dev_get_address();

  for (int i = 0; i < 6; i++) {

    char str[3];

    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);

    if (i < 5) {
      Serial.print(":");
    }

  }
}

void disconnectCallback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {
    Serial.println("Client Connected");
    bluetoothConnected = true;
    currentNotification = -1;
  }

  if (event == ESP_SPP_CLOSE_EVT ) {
    Serial.println("Client disconnected");
    drawMiddle("Lost connection");
    //delay(3000);

    //For some reason ESP32 can't reconnect to Bluetooth after the first/only client disconnects and needs a total reboot. TODO: fix this hacky shit
    ESP.restart();
  }
}
int maxChars = 17;
void vidPrint(String myStr)
{
  if(myStr == NULL) return;
  String str = myStr;
  int n = str.length();
  int originalCursorX = videoOut.getCursorX();
  int cursorY = videoOut.getCursorY();
  //at text size == 2, we can display 20 characters
  
  if(n < 18)
  {
    oneLineVidPrint(str);
    return;
  }
  
  //This fixes a bug where the first letter of a string is printed on its own line.
  //I couldn't figure out where the issue lies, so I just prepend a space character lmfao
  str = " " + str; 
  n = str.length();
  
  videoOut.setTextWrap(false);
  //int charsUntilEdge = maxChars; //assume textSize==2
  String temp = "";
  for(int i = 0; i < n; i++)
  {
    temp += str[i];
    if(i % maxChars == 0 || i == n - 1) {
      videoOut.print(reverseStr(temp));
      temp = "";
      videoOut.setCursor(originalCursorX, cursorY += 20);
    }
  }
}

void oneLineVidPrint(String str){
    videoOut.print(reverseStr(str));
}

String reverseStr(String str)
{
  int n = str.length();
 String reversed = "";
  for(int i = n - 1; i > -1; i--){
    reversed += str[i];
  }
  return reversed;
}
