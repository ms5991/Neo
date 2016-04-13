#include <Timer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Sparkfun_Spectrum_Shield.h>

#define MATRIX_PIN 6
#define LED_ROW 8
#define LED_COLUMN 8

int topArray[7] = {0, 0, 0, 0, 0, 0, 0};

Sparkfun_Spectrum_Shield spectrum = Sparkfun_Spectrum_Shield();

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(LED_ROW, LED_COLUMN, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

byte color[] = {25, 25, 112};
byte topColor[] = {0,0,255};

Timer t;

void setup() {
  spectrum.Init();
  matrix.begin();
  matrix.setBrightness(40);
  matrix.setTextColor(matrix.Color(255, 0, 0));
  matrix.setTextWrap(false);

  t.every(150, lowerTop);
  Serial.begin(9600);
}

void lowerTop()
{
  for(byte i=0;i<7;i++)
  {
    if(topArray[i] > 0)
    {
      topArray[i]--;
    }
  }
}

void updateColumn(int on, int column)
{  
  for(byte i=0;i<LED_ROW;i++)
  {
    if(i < (on-1))
    {
      matrix.drawPixel(i, column, matrix.Color(color[0], color[1], color[2]));
    }
    else
    {      
      if(topArray[column] == i)
      {
      matrix.drawPixel(i, column, matrix.Color(topColor[0], topColor[1], topColor[2]));        
      }
      else
      {
        
      matrix.drawPixel(i, column, matrix.Color(0, 0, 0));
      }
    }    
  }
}

void updateMatrixState()
{
  int on;
  for(int i=0;i<7;i++)
  {
    on = map(spectrum.average[i], 0, 1024, 0, 8);
  
    updateColumn(on, i);

    if(on > 0)
    {
      topArray[i] = on - 1;
    } 
  }  
}

void loop() {
  spectrum.Update();
  updateMatrixState();
  
  matrix.show();
  t.update();
  delay(50);

}

