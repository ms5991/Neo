#include <Timer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Sparkfun_Spectrum_Shield.h>

#define MATRIX_PIN 6
#define LED_ROW 8
#define LED_COLUMN 8
#define ALPHA 0.5
#define NOISE_LEVEL 45

int topArray[7] = {0, 0, 0, 0, 0, 0, 0};

Sparkfun_Spectrum_Shield spectrum = Sparkfun_Spectrum_Shield();

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(LED_ROW, LED_COLUMN, MATRIX_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

byte first[] = {25, 25, 112};
byte second[] = {0,0,128};
byte third[] = {0,0,139};
byte fourth[] = {255,0,205};
byte top[] = {0,0,255};

int avgs[] = {0,0,0,0,0,0,0};

Timer t;

int minimum[] = {100,100,100,100,100,100,100};
int maximum[] = {0,0,0,0,0,0,0};

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

void filterNoise()
{
  for(byte i=0;i<7;i++)
  {
    if(avgs[i] < minimum[i])
    {
      minimum[i] = avgs[i];
    }
    if(avgs[i] > maximum[i])
    {
      avgs[i] = avgs[i];
    }

    spectrum.average[i]-=minimum[i];
  }
}


void exAvg()
{
  for(byte i=0;i<7;i++)
  {
    avgs[i] = (ALPHA * spectrum.average[i]) + ((1-ALPHA) * avgs[i]);
  }
}

void printExAvg()
{
  for(byte i=0;i<7;i++)
  {
    Serial.print(avgs[i]);
    Serial.print(" ");
  }
  
  Serial.println();
}

void updateColumnWithResolution(int level, int column)
{
  byte on = 0;
  byte colorLevel = level % 4;
    if(level > 28)
    {
      on = 8;
    }
    else if(level > 24)
    {
      on = 7;
    }
    else if(level > 20)
    {
      on = 6;
    }
    else if(level > 16)
    {
      on = 5;
    }
    else if(level > 12)
    {
      on = 4;
    }
    else if(level > 8)
    {
      on = 3;
    }
    else if(level > 4)
    {
      on = 2;
    }
    else
    {
      on = 1;
    }

  for(byte i=0;i<LED_ROW;i++)
  {
    if(i == (on - 1))
    {
      switch (colorLevel)
      {
        case 0:
         matrix.drawPixel(i, column, matrix.Color(fourth[0], fourth[1], fourth[2]));
         break;
        
        case 1:
         matrix.drawPixel(i, column, matrix.Color(third[0], third[1], third[2]));
          break;
        
        case 2:
         matrix.drawPixel(i, column, matrix.Color(second[0], second[1], second[2]));
          break;
        
        case 3:
         matrix.drawPixel(i, column, matrix.Color(first[0], first[1], first[2]));
          break;
      }
    }
    else if(i < (on - 1))
    {
         matrix.drawPixel(i, column, matrix.Color(fourth[0], fourth[1], fourth[2]));      
    }
    else
    {  
        matrix.drawPixel(i, column, matrix.Color(0,0,0));
    } 
  }
}

void updateColumnWithTop(int on, int column)
{  
  for(byte i=0;i<LED_ROW;i++)
  {
    if(i < (on-1))
    {
      matrix.drawPixel(i, column, matrix.Color(second[0], second[1], second[2]));
    }
    else
    {      
      if(topArray[column] == i)
      {
      matrix.drawPixel(i, column, matrix.Color(fourth[0], fourth[1], fourth[2]));        
      }
      else
      {
        
      matrix.drawPixel(i, column, matrix.Color(0, 0, 0));
      }
    }    
  }
}

void updateMatrixStateWithResolution()
{
  int level;
  for(int i=0;i<7;i++)
  {
    level = map(spectrum.average[i], 0, 1024, 0, 32);
  
    updateColumnWithResolution(level, i);
  }  
}

void updateMatrixStateWithTop()
{
  int on;
  for(int i=0;i<7;i++)
  {
    on = map(spectrum.average[i], 0, 1024, 0, 8);
  
    updateColumnWithTop(on, i);

    if(on > 0)
    {
      topArray[i] = on - 1;
    } 
  }  
}

void dif()
{
  int dif = 0;
  for(byte i=0;i<7;i++)
  {
    dif = spectrum.average[i] -avgs[i];
    Serial.print(dif);
    Serial.print(" ");

    if(spectrum.average[i] < minimum[i])
    {
      minimum[i] = spectrum.average[i];
    }
  }
 
  Serial.println();

  for(byte i=0;i<7;i++)
  {    
      Serial.print(minimum[i]);
      Serial.print(" ");
  }
  Serial.println();
  
  for(byte i=0;i<7;i++)
  {
    dif = spectrum.average[i] - minimum[i];
    Serial.print(dif);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
/*  spectrum.Update();
  spectrum.PrintAverage();
  updateMatrixStateWithTop();
  
  matrix.show();
  t.update();
  delay(50);*/
int i = (analogRead(0) + analogRead(0) / 2);
  Serial.print(i);
  Serial.println();

  delay(100);

}






