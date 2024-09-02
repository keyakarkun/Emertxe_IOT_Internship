/*************************************************************
Title         :   Home automation using blynk
Description   :   To control light's brigntness with brightness,monitor temperature , monitor water level in the tank through blynk app
Pheripherals  :   Arduino UNO , Temperature system, LED, LDR module, Serial Tank, Blynk cloud, Blynk App.
 *************************************************************/

// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPL3bp6Fvh84"
#define BLYNK_TEMPLATE_NAME "internship"
#define BLYNK_AUTH_TOKEN "TvERAjFqijvgePmnmSdZB9EvQ3ahE8pj"
 
// Comment this out to disable prints 
//#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "main.h"
#include "temperature_system.h"
#include "ldr.h"
#include "serial_tank.h"

char auth[] = BLYNK_AUTH_TOKEN;
bool heater_sw,cooler_sw,inlet_sw,outlet_sw;
unsigned int tank_volume;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// This function is called every time the Virtual Pin 0 state changes
/*To turn ON and OFF cooler based virtual PIN value*/
BLYNK_WRITE(COOLER_V_PIN)
{
  cooler_sw = param.asInt();
  if(cooler_sw)
  {
    cooler_control(ON);
    lcd.setCursor(8,0);
    lcd.print("CLR_ON ");
  }
  else
  {
    cooler_control(OFF);
    lcd.setCursor(8,0);
    lcd.print("CLR_OFF");
  }
    
}

/*To turn ON and OFF heater based virtual PIN value*/
BLYNK_WRITE(HEATER_V_PIN )
{
  heater_sw = param.asInt();
   if(heater_sw)
  {
    heater_control(ON);
    lcd.setCursor(8,0);
    lcd.print("HTR_ON ");
  }
  else
  {
    heater_control(OFF);
    lcd.setCursor(8,0);
    lcd.print("HTR_OFF");
  }
  
}


/*To turn ON and OFF inlet vale based virtual PIN value*/
BLYNK_WRITE(INLET_V_PIN)
{
  /*Reading the value present on the virtual pin INLET_V_PI and storing it in inlet_sw*/
  inlet_sw = param.asInt();
  if(inlet_sw)
  {/*turn on the inlet valve*/
    enable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL_ON ");
  }
  else
  {
    disable_inlet();
    lcd.setCursor(7,1);
    lcd.print("IN_FL_OFF");
  }  
}
/*To turn ON and OFF outlet value based virtual switch value*/
BLYNK_WRITE(OUTLET_V_PIN)
{
  outlet_sw = param.asInt();
  if(outlet_sw)
  {
    enable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL_ON ");
  }
  else
  {
    disable_outlet();
    lcd.setCursor(7,1);
    lcd.print("OT_FL_OFF");
  }  
  
}
/* To display temperature and water volume as gauge on the Blynk App*/  
void update_temperature_reading()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  /*Sending temperature reading to temperature gauge for every 1 sec*/
   Blynk.virtualWrite(TEMPERATURE_GAUGE,read_temperature());
  /*Send volume of water in the tank for every 1 sec*/
}

/*To turn off the heater if the temperature raises above 35 deg C*/
void handle_temp(void)
{
    if(read_temperature() > (float) 35 && heater_sw)
    {
      heater_sw = 0;
      heater_control(OFF);
      Blynk.virtualWrite(HEATER_V_PIN,OFF);
      /*send notification to Dashboard*/
      lcd.setCursor(8,0);
      lcd.print("HTR_OFF");
    }

  
}

/*To control water volume above 2000ltrs*/
void handle_tank(void)
{
  /*Check if vol < 2000 and inlet valve is off*/
  if ((tank_volume < 2000) && (inlet_sw == 0))
  {
    enable_inlet();
    inlet_sw = 1;
     /*Reflect the status ON on the widget button of inlet valve*/
    Blynk.virtualWrite(INLET_V_PIN, ON);
    /* To print notification on dashboard*/
    lcd.setCursor(7,1);
    lcd.print("IN_FL_ON");

   
    }

  /*If volume of the water is 3000 and if inlet valver is ON disable inflow*/

  if ((tank_volume == 3000 && inlet_sw == 1))
  {
    disable_inlet();
    inlet_sw = 0;
    /*Reflect the status OFF on the widget button of inlet valve*/
    Blynk.virtualWrite(INLET_V_PIN, OFF);
    /* To print notification on dashboard*/
    lcd.setCursor(7,1);
    lcd.print("IN_FL_OFF"); 
  }
}


void setup(void)
{
  
  /*initialize the lcd*/
  lcd.init();
  /*turn the backlight */
  lcd.backlight();
  /*clear the clcd*/
  lcd.clear();
  /*cursor to the home */
  lcd.home();

  /*to display temperature*/
  lcd.setCursor(0,0);
  lcd.print ("Home Automation");
  delay(1000);
  lcd.clear();
  /*display volume*/
  lcd.setCursor(0,0);
  lcd.print ("T=");
  lcd.setCursor(0,1);
  lcd.print ("V=");
  Blynk.begin(BLYNK_AUTH_TOKEN);
  init_ldr();

  /*Initialization of temperature syatem*/
  init_temperature_system();
   init_serial_tank();
  /*Initializing the serial tank*/
  // init_serial_tank();  
  /*update temperature to Blynk app every 1 sec */
  timer.setInterval(1000, update_temperature_reading);
}
String temp;
void loop(void) 
{
    /* To run Blynk related functions*/
    /*To control the garden lights based on Light Intensity*/
    brightness_control();
    /*read temperature and display it on the dashboard*/
    temp = String (read_temperature(), 2);
    lcd.setCursor(2,0);
    lcd.print(temp);
    /*read the volume of water in the tank and display it on the dashboard or CLCD*/
     tank_volume = volume();
     lcd.setCursor(2,1);
    lcd.print(tank_volume);
     /*to connect the device continuously to the cloud*/
    Blynk.run();
   /*turn On timer*/
   timer.run();

    /* To control threshhold temperature of 35 degree celcius*/
    handle_temp(); 
    // /*To control volume of the water in the tank*/
    // handle_tank();         
}