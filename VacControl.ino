#include <LiquidCrystal.h>

//Pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
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
bool sucking = false;
bool started = false;

void setup()
{
  lcd.begin(16, 2);              // Start the library
}

void loop()
{
  if ((started) && (millis() > nextSwitchTime)) {
    toggleStates();
  }
  getInput();
  redraw();
}


//Internal Functiouns
/////////////////////

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




void redraw(){
  if (started) {
    unsigned long currentTimeSeconds = ((millis() - startTime) / 1000);
  }
  else {
    unsigned long currentTimeSeconds = 0;
  }

  lcd.setCursor(0,0);            // Move cursor to line 1, char 1
  lcd.print("VAC  ATM  TIME");

  //Draw Vac Time
  lcd.setCursor(0,1);
  lcd.print(intervalVac);
  //Draw ATM Time
  lcd.setCursor(5,1);
  lcd.print(intervalATM);

  lcd.setCursor(10,1);
  lcd.print(currentTimeSeconds);
}

void start() {
  //Only Allow Starting Once
  if (started){
    return;
  }

  //Timing Setup
  startTime = millis();
  nextSwitchTime = millis() + intervalVac;

  startSucking();

  started = true;
}

void toggleStates(){
  if (sucking){
    stopSucking();
    nextSwitchTime = millis() + intervalATM;
  }
  else {
    startSucking();
    nextSwitchTime = millis() + intervalVac;
  }
}

void startSucking(){
  //Switch on pump
  //Close valve
  sucking = true;
}

void stopSucking(){
  //Switch off pump
  //Open valve
  sucking = false;
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
