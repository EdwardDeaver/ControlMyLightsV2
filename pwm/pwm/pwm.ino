#include <ArtnetWifi.h>
#include <Arduino.h>


//Wifi settings
const char* ssid = "MySpectrumWiFide-2G";
const char* password = "littlecanoe411";

// Pixel settings
const int numLeds = 144; // change for your setup
const int numberOfChannels = numLeds * 3; // Total number of channels you want to receive (1 led = 3 channels)

// Artnet settings
ArtnetWifi artnet;
const int startUniverse = 1; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
// Check if we got all universes
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;


// the number of the LED pin
const int ledRPin = 13;  // 16 corresponds to GPIO16
const int ledGPin = 12;  // 16 corresponds to GPIO16
const int ledBPin = 27;  // 16 corresponds to GPIO16

// setting PWM properties
const int freq = 40000;
const int ledRChannel = 0;
const int ledGChannel = 1;
const int ledBChannel = 2;
const int resolution = 8;


// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data)
{


   boolean tail = false;
  
  Serial.print("DMX: Univ: ");
  Serial.print(universe, DEC);
  Serial.print(", Seq: ");
  Serial.print(sequence, DEC);
  Serial.print(", Data (");
  Serial.print(length, DEC);
  Serial.print("): ");

  
  sendFrame = 1;

  // set brightness of the whole strip 
  if (universe == 15)
  {
   // leds.setBrightness(data[0]);
   // leds.show();
  }

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
    universesReceived[universe - startUniverse] = 1;

  for (int i = 0 ; i < maxUniverses ; i++)
  {
    if (universesReceived[i] == 0)
    {
      //Serial.println("Broke");
      sendFrame = 0;
      break;
    }
  }
  ledcWrite(ledGChannel,  data[1]);
    ledcWrite(ledBChannel,  data[2]);
    ledcWrite(ledRChannel, data[0]);
    for (int i = 0; i < sizeof(data)+1; i++){
    Serial.println(data[i]);
    }
 
  previousDataLength = length;     
  
  if (sendFrame)
  {
 //   leds.show();
    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }

}

 
void setup(){
  Serial.begin(115200);
  ConnectWifi();
  // configure LED PWM functionalitites
  ledcSetup(ledRChannel, freq, resolution);
  ledcSetup(ledGChannel, freq, resolution);
  ledcSetup(ledBChannel, freq, resolution);

  // attach the channel to the GPIO to be controlled
  ledcAttachPin(ledRPin, ledRChannel);
  ledcAttachPin(ledGPin, ledGChannel);
  ledcAttachPin(ledBPin, ledBChannel);
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();

}
 
void loop(){
    // we call the read function inside the loop
  artnet.read();

    /*
  for(int dutyCycle = 0; dutyCycle <= 4095; dutyCycle++){   
    // changing the LED brightness with PWM
    // changing the LED brightness with PWM
    ledcWrite(ledRChannel, 0);
    ledcWrite(ledBChannel, 0);
    ledcWrite(ledGChannel, dutyCycle);

    delay(10);
  }

  for(int dutyCycle = 0; dutyCycle <= 4095; dutyCycle++){   
    // changing the LED brightness with PWM
    // changing the LED brightness with PWM
    ledcWrite(ledRChannel, 0);
    ledcWrite(ledBChannel, dutyCycle);
    ledcWrite(ledGChannel,0 );

    delay(10);
  }
    for(int dutyCycle = 0; dutyCycle <= 4095; dutyCycle++){   
    // changing the LED brightness with PWM
    // changing the LED brightness with PWM
    ledcWrite(ledRChannel, dutyCycle);
    ledcWrite(ledBChannel, dutyCycle);
    ledcWrite(ledGChannel,dutyCycle);

    delay(1);
  }
/*
  // decrease the LED brightness
  for(int dutyCycle = 4095; dutyCycle >= 0; dutyCycle--){
    // changing the LED brightness with PWM
    //ledcWrite(ledRChannel, dutyCycle);
    for(int dutyCycle = 4095; dutyCycle >= 0; dutyCycle--){
     // ledcWrite(ledGChannel, dutyCycle);
      for(int dutyCycle = 4095; dutyCycle >= 0; dutyCycle--){
        ledcWrite(ledBChannel, dutyCycle);  
        delay(1);
      }
      delay(1);
    }
    delay(1);
  }
  */
}
