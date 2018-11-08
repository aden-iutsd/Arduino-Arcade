#include "Adafruit_ILI9341.h"
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "math.h"

#define TFT_DC 9
#define TFT_CS 10

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0
#define WHITE    0xFFFF
// Rotation
#define PORTRAIT 0
#define LANDSCAPE 1

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Points pour le dessin à l'aide des primitives de la librairie graphique
int x = 130;
int y = 90;
int r = 20;
double a = PI / 4;
int x1 = x + r * cos(a);
int x2 = x + r * cos(a);
int y1 = y - r * sin(a);
int y2 = y + r * sin(a);
int xR = x1 + 1;
int yR = y1 + 1;
int w = (x + r) - x1;
int h  = y2 - y1;

// Image bitmap
const uint8_t pacman[] PROGMEM = {
  0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x00,
  0x7f, 0xff, 0xfe, 0x00, 0x01, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xff, 0xc0, 0x07, 0xff,
  0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff,
  0xfe, 0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x3f, 0xff, 0xff, 0xf8, 0x00, 0x7f, 0xff, 0xff, 0xf0,
  0x00, 0x7f, 0xff, 0xff, 0xe0, 0x00, 0x7f, 0xff, 0xff, 0xc0, 0x00, 0xff, 0xff, 0xff, 0x80, 0x00,
  0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00, 0x00, 0xff,
  0xff, 0xf8, 0x00, 0x00, 0xff, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xff, 0xfc, 0x00, 0x00, 0xff, 0xff,
  0xfe, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 0xff,
  0xc0, 0x00, 0x7f, 0xff, 0xff, 0xe0, 0x00, 0x7f, 0xff, 0xff, 0xf0, 0x00, 0x3f, 0xff, 0xff, 0xf8,
  0x00, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x1f, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00,
  0x0f, 0xff, 0xff, 0xff, 0x80, 0x07, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xff, 0xff, 0xff, 0xc0, 0x01,
  0xff, 0xff, 0xff, 0x80, 0x00, 0x7f, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xfc, 0x00, 0x00, 0x0f,
  0xff, 0xf0, 0x00, 0x00, 0x01, 0xff, 0x80, 0x00
};

void setup() {

  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(BLACK);
  tft.setRotation(LANDSCAPE);

}

// Comparaison du temps nécéssaire au dessin du pacman avec chacune des deux méthodes
void loop() {

  // Dessin du Pacman à l'aide des primitives
  unsigned long StartTime = millis();
  tft.fillCircle(x, y, r, YELLOW);
  tft.fillTriangle(x, y, x1, y1, x2, y2, BLACK);
  tft.fillRect(xR, yR, w, h, BLACK);
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;

  Serial.print("Draw primitives : ");
  Serial.println(ElapsedTime);
  delay(1000);

  // Dessin du Pacman à l'aide d'un bitmap
  StartTime = millis();
  tft.drawBitmap(110, 70, pacman, 40, 40, YELLOW);
  CurrentTime = millis();
  ElapsedTime = CurrentTime - StartTime;

  Serial.print("Bitmap : ");
  Serial.println(ElapsedTime);
  delay(1000);

  // Spoiler : les primitives sont plus rapides que le bitmap
}