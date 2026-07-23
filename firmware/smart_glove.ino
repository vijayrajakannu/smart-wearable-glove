#define BLYNK_TEMPLATE_ID "TMPL32wyJWEbw"
#define BLYNK_TEMPLATE_NAME "smart gloves"
#define BLYNK_AUTH_TOKEN "83sTGhHIGRwWsyxjOpz0oa3DV_buc7Ub"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "vivo Y200 5G";
char pass[] = "dass6312";

BlynkTimer timer;

// Flex pins
#define FLEX1 34
#define FLEX2 35

String lastMessage = "";

// Read flex with smoothing
int readFlex(int pin) {
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(pin);
    delay(2);
  }
  return sum / 10;
}

// Updated detection logic
String detectCommand(int f1, int f2) {

  bool f1Bent = (f1 < 3500);
  bool f2Bent = (f2 < 3500);

  bool f1Open = (f1 > 3800);
  bool f2Open = (f2 > 3800);

  if (f1Open && f2Open) {
    return "Hello, I need attention.";
  }
  else if (f1Open && f2Bent) {
    return "I need food.";
  }
  else if (f1Bent && f2Open) {
    return "Please give my medicine.";
  }
  else if (f1Bent && f2Bent) {
    return "Emergency! Please come immediately.";
  }

  return "No specific request.";
}

// Send data
void sendSensorData() {

  int f1 = readFlex(FLEX1);
  int f2 = readFlex(FLEX2);

  String msg = detectCommand(f1, f2);

  // Send to Blynk
  Blynk.virtualWrite(V0, f1);
  Blynk.virtualWrite(V1, f2);
  Blynk.virtualWrite(V9, msg);

  // Emergency alert
  if (msg != lastMessage) {
    lastMessage = msg;

    if (msg == "Emergency! Please come immediately.") {
      Blynk.logEvent("emergency_alert", "Emergency! Please come immediately.");
    }
  }

  // Send to laptop for voice
  Serial.print("F1:");
  Serial.print(f1);
  Serial.print(" | F2:");
  Serial.print(f2);
  Serial.print(" | Message:");
  Serial.println(msg);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, sendSensorData);

  Serial.println("Smart Glove Started");
}

void loop() {
  Blynk.run();
  timer.run();
}