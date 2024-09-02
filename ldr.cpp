#include "ldr.h"
#include "Arduino.h"
#include "main.h"

void init_ldr(void)
{
   pinMode(GARDEN_LIGHT, OUTPUT);
   pinMode(LDR_SENSOR ,INPUT);
   
}
void brightness_control(void)
{
  unsigned int adc_val;
   /*to read values from LDR sensor 0 to 1023 */
  adc_val=analogRead(LDR_SENSOR);

  /*scale it down 0 to 255 and reverse it */
  adc_val=255 -(adc_val/4);

  //map(0,1023,255,0)

  analogWrite(GARDEN_LIGHT, adc_val);

  delay(100);
}
