#include <Arduino_LSM6DS3.h>
#include <ArduinoBLE.h>
#include <WiFiNINA.h>

#define BLE_UUID_GESTURE_SERVICE "1101"
#define BLE_UUID_GESTURE_CHARACTERISTIC "2101"
#define BLE_NAME "Nano 33 IoT"
#define BAUD_RATE (9600)
#define SSID "Dante"
#define PASS "vafj478#"
enum Gesture {
  UP, DOWN, STOP  
};

BLEService gestureService(BLE_UUID_GESTURE_SERVICE);
BLEIntCharacteristic gestureCharacteristic(BLE_UUID_GESTURE_CHARACTERISTIC, BLERead | BLENotify);

float ax, ay, az;

//please enter your sensitive data in the Secret tab
char ssid[] = SSID;                // your network SSID (name)
char pass[] = PASS;                // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;             // the Wi-Fi radio's status
int ledState = LOW;                       //ledState used to set the LED
unsigned long previousMillisInfo = 0;     //will store last time Wi-Fi information was updated
unsigned long previousMillisLED = 0;      // will store the last time LED was updated
const int intervalInfo = 5000;            // interval at which to update the board information

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  /* Initialize IMU */
  if (!IMU.begin()) {
    Serial.println(F("Error: Failed to initialize IMU!"));
    while (1);
  }

  //COMMENT OUT WHICHEVER CONNECTION MODE YOU ARE NOT USING

  /* Initialize BLE */
  if (!BLE.begin()) {
    Serial.println(F("Error: Failed to initialize BLE module!"));
    while (1);
  }

  /* Set advertised local name and service UUID */
  BLE.setLocalName(BLE_NAME);
  BLE.setDeviceName(BLE_NAME);
  BLE.setAdvertisedService(gestureService);

  /* Add characteristic(s) to the service */
  gestureService.addCharacteristic(gestureCharacteristic);

  /* Add service */
  BLE.addService(gestureService);

  /* Set initial values for characteristics */
  gestureCharacteristic.writeValue(STOP);

  /* Start advertising */
  BLE.advertise();
  Serial.print(F("BLE advertising as "));
  Serial.println(BLE_NAME);

  //-----------------------------------------------------------------------------------------------------------------
  // /* Initialize serial */
  Serial.begin(BAUD_RATE);
  while (!Serial);

  // attempt to connect to Wi-Fi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  // you're connected now, so print out the data:
  Serial.println("You're connected to the network");
  Serial.println("---------------------------------------");
  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1);
  }
}

void loop()
{
  BLEDevice central = BLE.central();
  if (IMU.accelerationAvailable()) {
    digitalWrite(LED_BUILTIN, HIGH);
    IMU.readAcceleration(ax, ay, az);
    if (az >= 0.8) {
      gestureCharacteristic.writeValue(UP);
    } else if (az <= -0.8) {
      gestureCharacteristic.writeValue(DOWN);
    } else {
      gestureCharacteristic.writeValue(STOP);
    }
  }
  digitalWrite(LED_BUILTIN, LOW);

  //-----------------------------------------------------------------------------------------------------------------
  unsigned long currentMillisInfo = millis();

  // check if the time after the last update is bigger the interval
  if (currentMillisInfo - previousMillisInfo >= intervalInfo) {
    previousMillisInfo = currentMillisInfo;

    if (IMU.accelerationAvailable()) {
      digitalWrite(LED_BUILTIN, HIGH);
      IMU.readAcceleration(ax, ay, az);
      if (az >= 0.8) {
        Serial.print("UP");
      } else if (az <= -0.8) {
        Serial.print("DOWN");
      } else {
        Serial.print("STOP");
      }
      Serial.print('\t');
    }
  }
  digitalWrite(LED_BUILTIN, LOW);
}
