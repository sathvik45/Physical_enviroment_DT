

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial



#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_TEMPLATE_ID "TMPLYQs12vEx"
#define BLYNK_DEVICE_NAME "controller"
#define BLYNK_AUTH_TOKEN "ToPhF9chFJBte8vqvb1Ios5t9ibLbRUQ"
char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "iot";
char pass[] = "123456789";

BlynkTimer timer;

//// This function is called every time the Virtual Pin 0 state changes
//BLYNK_WRITE(V0)
//{
//  // Set incoming value from pin V0 to a variable
//  int value = param.asInt();
//
//  // Update state
//  Blynk.virtualWrite(V1, value);
//}
//
//// This function is called every time the device is connected to the Blynk.Cloud

#define ENA D0
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
#define ENB D5
 
bool forward = 0;
bool backward = 0;
bool left = 0;
bool right = 0;
int Speed;

BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}
BLYNK_WRITE(V0) {
  forward = param.asInt();
}
 
BLYNK_WRITE(V2) {
  backward = param.asInt();
}
 
BLYNK_WRITE(V1) {
  left = param.asInt();
}
 
BLYNK_WRITE(V3) {
  right = param.asInt();
}
 
BLYNK_WRITE(V4) {
  Speed = param.asInt();
}
 
void smartcar() {
  if (forward == 1) {
    carforward();
    Serial.println("carforward");
  } else if (backward == 1) {
    carbackward();
    Serial.println("carbackward");
  } else if (left == 1) {
    carturnleft();
    Serial.println("carfleft");
  } else if (right == 1) {
    carturnright();
    Serial.println("carright");
  } else if (forward == 0 && backward == 0 && left == 0 && right == 0) {
    carStop();
    Serial.println("carstop");
  }
}
void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  //Blynk.begin("ToPhF9chFJBte8vqvb1Ios5t9ibLbRUQ", ssid, pass);
  // You can also specify server:
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

}

void loop()
{
  Blynk.run();
  timer.run();
  smartcar();
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}
void carforward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carbackward() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carturnleft() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}
void carturnright() {
  analogWrite(ENA, Speed);
  analogWrite(ENB, Speed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}
void carStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
