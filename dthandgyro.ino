
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
// WiFi settings
const char* ssid = "iot";
const char* password = "123456789";

#define DHTPIN 2     // Digital pin connected to the DHT sensor

//Azure IoT Hub
const String AzureIoTHubURI="https://roverDT.azure-devices.net/devices/majorProject05/messages/events?api-version=2020-03-13"; 
//openssl s_client -servername myioteventhub.azure-devices.net -connect myioteventhub.azure-devices.net:443 | openssl x509 -fingerprint -noout //
const String AzureIoTHubFingerPrint="89:EC:30:21:CC:F6:E6:70:39:33:AC:DB:C6:84:3E:F9:0B:EF:28:A5"; 
//az iot hub generate-sas-token --device-id {YourIoTDeviceId} --hub-name {YourIoTHubName} 
const String AzureIoTHubAuth="SharedAccessSignature sr=roverDT.azure-devices.net%2Fdevices%2FmajorProject05&sig=G%2F4et%2BAcbRuePY4EZ70P9rP9N14wGyyIw%2FrEcNw3ATc%3D&se=1652974284";
#define DHTTYPE DHT11  
int minV=265;
int maxV=402;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(10);


  

  dht.begin();
  // Init serial line
 // Serial.begin(115200);
  Serial.println("ESP8266 starting in normal mode");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Wait a few seconds between measurements.
  delay(5000);
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s"); 
  int xAng = map(a.acceleration.x,minV,maxV,-90,90);
  int yAng = map(a.acceleration.x,minV,maxV,-90,90);
  int zAng = map(a.acceleration.x,minV,maxV,-90,90);
  double X= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
  double Y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
  double Z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
  
  String PostData="{ \"DeviceId\":\"majorProject05\",\"Temperature\":"+String(t)+",\"Humidity\":"+String(h)+",\"X\":"+String(X)+",\"Y\":"+String(Y)+",\"Z\":"+String(Z)+"}";
      Serial.println(PostData);
      // Send data to cloud
      int returnCode=RestPostData(AzureIoTHubURI,AzureIoTHubFingerPrint,AzureIoTHubAuth,PostData);
      Serial.println(returnCode);
}

// Functions
int RestPostData(String URI, String fingerPrint, String Authorization, String PostData)
{
    HTTPClient http;
    http.begin(URI,fingerPrint);
    http.addHeader("Authorization",Authorization);
    http.addHeader("Content-Type","application/json");
    int returnCode=http.POST(PostData);
    if(returnCode<0) 
  {
    Serial.println("RestPostData: Error sending data: "+String(http.errorToString(returnCode).c_str()));
  
  // file found at server
      if (returnCode == HTTP_CODE_OK) {
        const String& payload = http.getString();
        Serial.println("received payload:\n<<");
        Serial.println(payload);
        Serial.println(">>");
      }
       else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(returnCode).c_str());
    }
}
    http.end();
  return returnCode;
}
