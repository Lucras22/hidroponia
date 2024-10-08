//https://www.arduitronics.com/product/2337/water-flow-sensor-1-30l-min-%E0%B9%81%E0%B8%A3%E0%B8%87%
//E0%B8%94%E0%B8%B1%E0%B8%99%E0%B9%84%
//E0%B8%A1%E0%B9%88%E0%B9%80%E0%B8%81%E0%B8%B4%E0%B8%99
//-1-75mpa-%E0%B8%82%E0%B8%99%E0%B8%B2%E0%B8%94%E0%B8%97%E0%B9%88%E0%B8%AD
//-1-4%E2%80%9D-yf-s201c-%E0%B9%81%E0%B8%9A%E0%B8%9A%E0%B9%83%E0%B8%AA


volatile int NbTopsFan; //measuring the rising edges of the signal
int Calc;
int hallsensor = 2; //The pin location of the sensor

void rpm() //This is the function that the interupt calls
{
NbTopsFan++; //This function measures the rising and falling edge of the hall effect sensors signal
}

void setup()
{
 pinMode(hallsensor, INPUT); //initializes digital pin 2 as an input
Serial.begin(9600); //This is the setup function where the serial port is initialised,
attachInterrupt(0, rpm, RISING); //and the interrupt is attached
}

void loop()
{
NbTopsFan = 0; //Set NbTops to 0 ready for calculations
sei(); //Enables interrupts
delay(1000); //Wait 1 second
cli(); //Disable interrupts
Calc = (NbTopsFan * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour
Serial.print(Calc, DEC); //Prints the number calculated above
Serial.print(" L/hour\r\n"); //Prints "L/hour" and returns a new line
}   
