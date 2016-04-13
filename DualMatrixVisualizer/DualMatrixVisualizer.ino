#include <Timer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Sparkfun_Spectrum_Shield.h>

#define MATRIX_ONE_PIN 6
#define MATRIX_TWO_PIN 7
#define LED_ROW 8
#define LED_COLUMN 8

int rightTopArray[7] = {0, 0, 0, 0, 0, 0, 0};
int leftTopArray[7] = {0, 0, 0, 0, 0, 0, 0};

Sparkfun_Spectrum_Shield spectrum = Sparkfun_Spectrum_Shield();

Adafruit_NeoMatrix rightMatrix = Adafruit_NeoMatrix(LED_ROW, LED_COLUMN, MATRIX_ONE_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

Adafruit_NeoMatrix leftMatrix = Adafruit_NeoMatrix(LED_ROW, LED_COLUMN, MATRIX_TWO_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

byte color[] = {25, 25, 112};
byte topColor[] = {0,0,255};

Timer t;

void setup() {
  spectrum.Init();
  rightMatrix.begin();
  rightMatrix.setBrightness(40);
  rightMatrix.setTextColor(rightMatrix.Color(255, 0, 0));
  rightMatrix.setTextWrap(false);
  
  leftMatrix.begin();
  leftMatrix.setBrightness(40);
  leftMatrix.setTextColor(leftMatrix.Color(255, 0, 0));
  leftMatrix.setTextWrap(false);

  t.every(150, lowerTop);
  Serial.begin(9600);
}

void lowerTop()
{
  for(byte i=0;i<7;i++)
  {
    if(rightTopArray[i] > 0)
    {
      rightTopArray[i]--;
    }
    
    if(leftTopArray[i] > 0)
    {
      leftTopArray[i]--;
    }
  }
}

void updateColumnSeparateChannels(int rightOn, int leftOn, int column)
{  
  for(byte i=0;i<LED_ROW;i++)
  {
    if(i < (rightOn-1))
    {
      rightMatrix.drawPixel(i, column, rightMatrix.Color(color[0], color[1], color[2]));
    }
    else
    {      
      if(rightTopArray[column] == i)
      {
        rightMatrix.drawPixel(i, column, rightMatrix.Color(topColor[0], topColor[1], topColor[2]));        
      }
      else
      {        
        rightMatrix.drawPixel(i, column, rightMatrix.Color(0, 0, 0));
      }
    }
    
    if(i < (leftOn-1))
    {
      leftMatrix.drawPixel(i, column, leftMatrix.Color(color[0], color[1], color[2]));
    }
    else
    {      
      if(leftTopArray[column] == i)
      {
        leftMatrix.drawPixel(i, column, leftMatrix.Color(topColor[0], topColor[1], topColor[2]));        
      }
      else
      {        
        leftMatrix.drawPixel(i, column, leftMatrix.Color(0, 0, 0));
      }
    }
  }
}

void updateMatrixStateSeparateChannels()
{
  int rightOn, leftOn;
  for(int i=0;i<7;i++)
  {
    rightOn = map(spectrum.right_channel[i], 0, 1024, 0, 8);
    leftOn = map(spectrum.left_channel[i], 0, 1024, 0, 8);
  
    updateColumnSeparateChannels(rightOn, leftOn, i);

    if(rightOn > 0)
    {
      rightTopArray[i] = rightOn - 1;
    } 
    
    if(leftOn > 0)
    {
      leftTopArray[i] = leftOn - 1;
    } 
  }  
}

void loop() {
  spectrum.Update();
  updateMatrixStateSeparateChannels();
  
  rightMatrix.show();
  leftMatrix.show();
  t.update();
  delay(50);

}

