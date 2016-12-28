#include <LiquidCrystal.h>

//Valve on Pin 14
//Pump on Pin 15

//Pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Vales used by the LCD
int lcd_key     =        0;
int adc_key_in  =        0;
#define btnRIGHT         0
#define btnUP            1
#define btnDOWN          2
#define btnLEFT          3
#define btnSELECT        4
#define btnNONE          5

//Devices
#define VAC_ON           255
#define VAC_OFF          0
#define VALVE_OPEN       255
#define VALVE_CLOSED     0

//Interval for Vacuum / Venting
int intervalVac =  10;
int intervalATM =  60;
unsigned long nextSwitchMillis = 0;
unsigned long startMillis = 0;

//Setup Status
bool vac = false;
bool started = false;
bool buttonDown = false;
char vacIndicator = 32;

void setup()
{
  //Start the library
  lcd.begin(16, 2);

  //Set up output pins
  pinMode(A15, OUTPUT);
  pinMode(A14, OUTPUT);

  //Atmospheric state
  analogWrite(A15, VAC_OFF);
  analogWrite(A14, VALVE_OPEN);
}

void loop()
{
  //Check if trigger point has been reached
  if ((started) && (millis() > nextSwitchMillis))
    toggleStates();

  //Process any user interaction
  getInput();
  //Render the screen
  redraw();
}

//Get user keypresses
void getInput(){

  lcd.setCursor(0,1);
  lcd_key = read_LCD_buttons();

  switch (lcd_key)
  {
    case btnRIGHT:
    {
      if (buttonDown)
        return;
      buttonDown = true;

      intervalATM++;
      break;
    }
    case btnLEFT:
    {
      if (buttonDown)
        return;
      buttonDown = true;

      //Prevent going to -1
      if (intervalATM == 0)
        return;

      intervalATM--;
      break;
    }
    case btnUP:
    {
      if (buttonDown)
        return;
      buttonDown = true;

      intervalVac++;
      break;
    }
    case btnDOWN:
    {
      if (buttonDown)
        return;
      buttonDown = true;

      //Prevent going to -1
      if (intervalVac == 0)
        return;

      intervalVac--;
      break;
    }
    case btnSELECT:
    {
      if (buttonDown)
        return;
      buttonDown = true;

      start();
      break;
    }
    case btnNONE:
    {
      buttonDown = false;
      break;
    }
    default:
    {
      buttonDown = false;
      break;
    }
  }
}


//Render the screen
void redraw(){
  //Buffers to store the top and bottom lines
  char upperBuffer[16];
  char lowerBuffer[16];

  unsigned long currentTimeSeconds = 0;
  //If the timer has not yet started, leave the counter at 0
  if (started)
    currentTimeSeconds = ((millis() - startMillis) / 1000);

  //Draw upper line
  lcd.setCursor(0,0);
  lcd.print("VAC  ATM  TIME");

  //Draw vacuum indicator
  lcd.setCursor(15,0);
  lcd.print((char)vacIndicator);

  //Create bottom line
  sprintf(lowerBuffer, "%3d  %3d  %6d", intervalVac, intervalATM, currentTimeSeconds);

  //Draw lower line
  lcd.setCursor(0,1);
  lcd.print(lowerBuffer);
}

//Start the sequence
void start() {
  //Only Allow Starting Once
  if (started){
    return;
  }

  //Timing Setup
  startMillis = millis();

  startVac();

  started = true;
}

//Switch between vacuum and atmosphere
void toggleStates(){
  if (vac)
    stopVac();
  else
    startVac();
}

//Activate pump, close valve
void startVac(){
  //Close valve
  analogWrite(A14, VALVE_CLOSED);

  //Switch on pump
  analogWrite(A15, VAC_ON);

  //Set trigger point
  nextSwitchMillis = millis() + minToMillis(intervalVac);

  //Set indicator to a caret symbol
  vacIndicator = 94;
  vac = true;
}

//Deactivate pump, open valve
void stopVac(){
  //Switch off pump
  analogWrite(A15, VAC_OFF);

  //Open valve
  analogWrite(A14, VALVE_OPEN);

  //Set trigger point
  nextSwitchMillis = millis() + minToMillis(intervalATM);

  //Set indicator to a space character
  vacIndicator = 32;

  vac = false;
}

unsigned long minToMillis(unsigned long minutes){
  return(minutes * 60000);
}

int read_LCD_buttons() {
  adc_key_in = analogRead(0);
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 195)  return btnUP;
  if (adc_key_in < 380)  return btnDOWN;
  if (adc_key_in < 555)  return btnLEFT;
  if (adc_key_in < 790)  return btnSELECT;
  return btnNONE;
}
