#define rLED 6      // rote LED
#define gLED 7      // grüe LED


void maskeErkannt(char wert)
{
  if(wert == 'g')
  {
    digitalWrite(rLED, LOW);
    digitalWrite(gLED, HIGH);
  }
  else if(wert == 'r')
  {
    digitalWrite(rLED, HIGH);
    digitalWrite(gLED, LOW);
  }
  else
  {
    digitalWrite(rLED, LOW);
    digitalWrite(gLED, LOW);
  }

  return;
}

void setup() 
{
  pinMode(rLED, OUTPUT);
  pinMode(gLED, OUTPUT);
  Serial.begin(9600);

}

void loop() 
{
    
  
  if(Serial.available())
  {
    
    
    char erkennung = Serial.read();
    maskeErkannt(erkennung);
  }

}
