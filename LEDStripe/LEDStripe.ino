/**
 *  SSID: LED
 *  http://192.168.4.1
 *
 */

// Taken from https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library
#include <Adafruit_NeoPixel.h>

#include <MFRC522.h>
#include <SPI.h>

// LED Stripe
#define NUM_LEDS      5
#define PIN_LED_STRIP 9

// RFID reader
#define SS_PIN        10
#define RST_PIN       3

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);

MFRC522 rfid(SS_PIN, RST_PIN); // RFID reader
uint32_t color;

const struct {
   unsigned char r;
   unsigned char g;
   unsigned char b;
} c[] = {
  { 255,   0,   0},
  {   0, 255,   0},
  {   0,   0, 255},
  { 255, 255,   0},
  { 255,   0, 255}
};

void setup() {
  Serial.begin(9600);
  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522
  
  strip.begin();
  strip.setBrightness(11);
  for(int i = 0; i < strip.numPixels(); ++i){
    strip.setPixelColor(i, c[i].r, c[i].g, c[i].b);  
  }
  strip.show();
  color = strip.Color(255, 255, 0); // hell Blau
  Serial.println(F("Hello world!"));
}

void loop() {

  for (int cycle=0;  cycle<10; cycle++) {  // 10 Durchläufe
    for (int step=0; step<3; step++) {
      for (uint16_t pixno=0; pixno<strip.numPixels(); pixno += 3) {
        strip.setPixelColor(pixno+step, color); // jedes 3. Pixel an
      }
      strip.show();

      delay(200);

      for (uint16_t pixno=0; pixno<strip.numPixels(); pixno += 3) {
        strip.setPixelColor(pixno+step, 0); // jedes 3. Pixel wieder aus
      }
      if (rfid.PICC_IsNewCardPresent())
      {
         color = readRFID();
      }
    }
  }

}

uint32_t readRFID() {
  String uidString;
  
  rfid.PICC_ReadCardSerial();
  Serial.print(F("\nPICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check if the RFID is Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
  piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
  piccType != MFRC522::PICC_TYPE_MIFARE_4K )  {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return strip.Color(255,255,255);
  }

  Serial.println(F("Scanned PICC's IOD:"));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  uidString = String(rfid.uid.uidByte[0]) + " " 
            + String(rfid.uid.uidByte[1]) + " "
            + String(rfid.uid.uidByte[2]) + " "
            + String(rfid.uid.uidByte[3]);
  return strip.Color(rfid.uid.uidByte[0], rfid.uid.uidByte[1], rfid.uid.uidByte[2]);
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
