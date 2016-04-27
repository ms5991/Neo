#include <stdint.h>
#include <ffft.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Timer.h>

#define  IR_AUDIO  0 // ADC channel to capture


volatile  byte  position = 0;
volatile  long  zero = 0;

int16_t capture[FFT_N];      /* Wave captureing buffer */
complex_t bfly_buff[FFT_N];   /* FFT buffer */
uint16_t spektrum[FFT_N/2];   /* Spectrum output buffer */
int values[8] = {0,0,0,0,0,0,0,0};
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, 6,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

Timer t;
int topArray[8] = {0, 0, 0, 0, 0, 0, 0, 0};

void setup()
{
  Serial.begin(57600);
  matrix.begin();
  matrix.setBrightness(40);
  matrix.setTextColor(matrix.Color(255, 0, 0));
  matrix.setTextWrap(false);
  adcInit();
  adcCalb();
  t.every(75, lowerTop);
 // establishContact();  // send a byte to establish contact until Processing respon
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
  for(byte i=0;i<8;i++)
  {
    if(i < (on-1) || i < topArray[i])
    {
      matrix.drawPixel(i, column, matrix.Color(0, 0, 255));
    }
    else
    {       
      matrix.drawPixel(i, column, matrix.Color(0, 0, 0));
    }    
  }
}

void updateMatrixState()
{
  int on;
  for(int i=0;i<8;i++)
  {
    on = map(values[i], 0, 500, 0, 8);
  
    updateColumn(on, i);

    if(on > 0)
    {
      topArray[i] = on - 1;
    } 
  }  
}

void loop()
{
  if (position == FFT_N)
  {
    fft_input(capture, bfly_buff);
    fft_execute(bfly_buff);
    fft_output(bfly_buff, spektrum);

    for(byte i=0;i<8;i++)
    {
      for(byte j=0;j<8;j++)
      {
        values[i] += spektrum[j + (i * 8)];
      }

      values[i] /= 8;
    }

    for(byte i=0;i<8;i++)
    {
      Serial.print(values[i]);
      Serial.print(" ");
    }

    updateMatrixState();
    matrix.show();
    
    Serial.println();
   position = 0;
  }
}

void establishContact() {
 while (Serial.available() <= 0) {
      Serial.write('A');   // send a capital A
      delay(300);
  }
}

// free running ADC fills capture buffer
ISR(ADC_vect)
{
  if (position >= FFT_N)
    return;
  
  capture[position] = ADC + zero;
  if (capture[position] == -1 || capture[position] == 1)
    capture[position] = 0;

  position++;
}
void adcInit(){
  /*  REFS0 : VCC use as a ref, IR_AUDIO : channel selection, ADEN : ADC Enable, ADSC : ADC Start, ADATE : ADC Auto Trigger Enable, ADIE : ADC Interrupt Enable,  ADPS : ADC Prescaler  */
  // free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion 
  ADMUX = _BV(REFS0) | IR_AUDIO; // | _BV(ADLAR); 
//  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) //prescaler 64 : 19231 Hz - 300Hz per 64 divisions
  ADCSRA = _BV(ADSC) | _BV(ADEN) | _BV(ADATE) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
  sei();
}
void adcCalb(){
  Serial.println("Start to calc zero");
  long midl = 0;
  // get 2 meashurment at 2 sec
  // on ADC input must be NO SIGNAL!!!
  for (byte i = 0; i < 2; i++)
  {
    position = 0;
    delay(100);
    midl += capture[0];
    delay(900);
  }
  zero = -midl/2;
  Serial.println("Done.");
}
