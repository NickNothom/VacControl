#include <LiquidCrystal.h>

//Pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Vales used by the LCD
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

//Interval for Vacuum / Venting
int intervalVac =  600000;
int intervalATM =  600000 * 6;
unsigned long nextSwitchTime = 0;
unsigned long startTime = 0;

//Setup Status
bool vac = false;
bool started = false;

void setup()
{
  // Start the library
  lcd.begin(16, 2);
}

void loop()
{
  //Check if trigger point has been reached
  if ((started) && (millis() > nextSwitchTime)) {
    toggleStates();
  }
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
      intervalATM++;
      break;
    }
    case btnLEFT:
    {
      intervalATM--;
      break;
    }
    case btnUP:
    {
      intervalVac++;
      break;
    }
    case btnDOWN:
    {
      intervalVac--;
      break;
    }
    case btnSELECT:
    {
      //Start timer
      start();
      break;
    }

    case btnNONE:
    {
      break;
    }
  }
}


//Render the screen
void redraw(){
  unsigned long currentTimeSeconds = 0;
  //If the timer has not yet started, leave the counter at 0
  if (started)
    currentTimeSeconds = ((millis() - startTime) / 1000);

  //Draw upper line
  lcd.setCursor(0,0);
  lcd.print("VAC  ATM  TIME");

  //Draw Vac Time
  lcd.setCursor(0,1);
  lcd.print(intervalVac);

  //Draw ATM Time
  lcd.setCursor(5,1);
  lcd.print(intervalATM);

  //Draw timer
  lcd.setCursor(10,1);
  lcd.print(currentTimeSeconds);
}

//Start the sequence
void start() {
  //Only Allow Starting Once
  if (started){
    return;
  }

  //Timing Setup
  startTime = millis();
  nextSwitchTime = millis() + intervalVac;

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
  //Switch on pump

  //Close valve

  //Set trigger point
  nextSwitchTime = millis() + intervalVac;
  vac = true;
}

//Deactivate pump, open valve
void stopVac(){
  //Switch off pump

  //Open valve

  //Set trigger point
  nextSwitchTime = millis() + intervalATM;
  vac = false;
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
