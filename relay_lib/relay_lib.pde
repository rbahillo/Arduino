#include <GSM_Shield.h>

#include <LiquidCrystal.h>

#define ALLOWED_NUMBER_SIZE 1
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

GSM gprs;
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int relay_a=2;
int relay_b=3;

//Variables for SMS
byte type_sms=SMS_UNREAD;      //Type of SMS
char number_incoming[20];
char sms_rx[122]; //Received text SMS
byte del_sms=1;  
int error;
char* allowedNumbers[ALLOWED_NUMBER_SIZE]  = {"+34672289728"};

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;


void setup()
{
  gprs.TurnOn(19200);          //module power on
  gprs.InitParam(PARAM_SET_1);//configure the module  
  gprs.Echo(0); 
  lcd.begin(16, 1);
  lcd.print( "Arrancando" );
  delay(5000);
  pinMode( relay_a, OUTPUT ); 
  pinMode( relay_b, OUTPUT ); 
  digitalWrite( relay_a, HIGH ); 
  digitalWrite( relay_b, HIGH );  
  lcd.clear();
  lcd.print("Caldera Apagada");
  
}

void loop()
{
  //Comprobamos si hay sms a procesar
  Check_SMS();
  Check_Buttons();
  
}

void displayText ( String text ){
  lcd.print(text);
  for (int positionCounter = 0; positionCounter < text.length() ; positionCounter++){
    lcd.scrollDisplayLeft();
    delay(1000);
  }
}

void Check_Buttons()  //Check if there is an sms 'type_sms'
 {
   lcd_key = read_LCD_buttons(); 
   switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     break;
     }
   case btnLEFT:
     {
     break;
     }
   case btnUP:
     {
     ProcessSms( "ona" );
     break;
     }
   case btnDOWN:
     {
     ProcessSms( "offa" );
     break;
     }
   case btnSELECT:
     {
     
     break;
     }
     case btnNONE:
     {
     
     break;
     }
 }
 }

void Check_SMS()  //Check if there is an sms 'type_sms'
 {
     char pos_sms_rx;  //Received SMS position     
     pos_sms_rx=gprs.IsSMSPresent(type_sms);
     if (pos_sms_rx!=0)
     {
       //Read text/number/position of sms
       gprs.GetSMS(pos_sms_rx,number_incoming,sms_rx,120);
       if(isAllowedNumber(number_incoming)==1){
         ProcessSms(sms_rx);
       }
       if (del_sms==1)  //If 'del_sms' is 1, i delete sms 
       {
         error=gprs.DeleteSMS(pos_sms_rx);
       }
     }
     return;
 }
 
 int isAllowedNumber(char* number_incoming){
 
   int allowed = 0;
   String numberS = number_incoming;
   for(int i=0; i<ALLOWED_NUMBER_SIZE ;i++){
     String allowedNumber = allowedNumbers[i];
     if(numberS.indexOf(allowedNumber)>=0){
       allowed=1;
       break;
     }
   }
   return allowed;
 }

 void ProcessSms( String sms ){
  
  if( sms.indexOf("ona") >= 0 ){
    digitalWrite( relay_a, LOW );
    lcd.clear();
    lcd.print("Caldera Encendida");
  }
  if( sms.indexOf("offa") >= 0 ){
    digitalWrite(  relay_a, HIGH );
    lcd.clear();
    lcd.print("Caldera Apagada");
  }
  if( sms.indexOf("offb") >= 0 ){
    digitalWrite(  relay_b, HIGH );
  }
  if( sms.indexOf("onb") >= 0 ){
    digitalWrite(  relay_b, HIGH );
  }
}

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1500) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 700)  return btnLEFT; 
 if (adc_key_in < 1000)  return btnSELECT;   
 return btnNONE;  // when all others fail, return this...
}

