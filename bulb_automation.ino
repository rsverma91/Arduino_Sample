#include <WiFi.h>

#include <FirebaseESP32.h>

#define FIREBASE_HOST "your_firebase_host"
#define FIREBASE_AUTH "your_firebase_auth"
#define WIFI_SSID "wifi"
#define WIFI_PASSWORD "password"

FirebaseData firebaseData;

const int threshold = 25;

const int ledPin16 = 16;
const int touchPin4 = 4;
int currentStatus = 1;

void setup() {
  Serial.begin(115200);

  pinMode(touchPin4, INPUT);
  pinMode(ledPin16, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Set Firebase info
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  //Auto reconnect the WiFi when connection lost
  Firebase.reconnectWiFi(true);

  Firebase.setInt(firebaseData, "/LED_Status", currentStatus);
}

void loop() {
  int touchValue = touchRead(touchPin4);

  Serial.print("Touch value: ");
  Serial.print(touchValue);

  if (touchValue < threshold) {
    if (currentStatus == 0) {
      currentStatus = 1;
      digitalWrite(ledPin16, currentStatus);
      Firebase.setInt(firebaseData, "/LED_Status", currentStatus);
    } else {
      currentStatus = 0;
      digitalWrite(ledPin16, currentStatus);
      Firebase.setInt(firebaseData, "/LED_Status", currentStatus);
    }
  }

  if (Firebase.getInt(firebaseData, "/LED_Status")) {
    //Success
    currentStatus = firebaseData.intData();

    Serial.print("Get int data success, int = ");
    Serial.println(currentStatus);

    digitalWrite(ledPin16, currentStatus);

  } else {
    //Failed?, get the error from firebaseData
    Serial.print("Error in getInt, ");
    Serial.println(firebaseData.errorReason());
  }
  delay(1000);
}
