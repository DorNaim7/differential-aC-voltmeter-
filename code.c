bool flag=0;
int cnt=0;
int val=0;
int sample[10];
double mean =0;
double var=0;
bool sw=0;
int SW_POS=8;
//int MEASURE=0;

void setup() {
  Serial.begin(9600);
  // TIMER 1 for interrupt frequency 4 Hz:
  cli(); // stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; // initialize counter value to 0
  // set compare match register for 4 Hz increments
  OCR1A = 62499; // = 16000000 / (64 * 4) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for 64 prescaler
  TCCR1B |= (0 << CS12) | (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei(); // allow interrupts

  pinMode(SW_POS,OUTPUT);
  digitalWrite(SW_POS,LOW);
}

void loop() {
  if (cnt==5 && flag)// determine measure range and switch position
  {
    val=analogRead(A0);
    if (val<103)//val < 0.5*(2^0.5)*1023/5 -> max value for lower measurement range
    {
      sw=1;//voltage is between 0-50 mV, needs to be amped
      digitalWrite(SW_POS,HIGH);
    }
    flag = 0;
  }
  
  if (cnt==20 && flag)//when 5 second measure period exceeds
  {
    mean=0;
    for (int i=0;i<10;i++)
      mean+=sample[i];
    mean/=10;
    var=0;
    for (int i=0;i<10;i++)
      var = var + (sample[i]- mean)*(sample[i]- mean); 
    var/=10;
    Serial.print("Mean value (RMS)= ");
    if (sw)
    {
      //Serial.print("sw1: ");
      mean = mean*5*10;
      mean /= 1023; 
      Serial.print(mean);
      Serial.print("mV; Variation (RMS)= ");
      var = var*5*10;
      var = var/1023;
      Serial.print(var);
      Serial.println("(mV)^2; "); 
    }
    else
    {
      //Serial.print("sw0: ");
      mean = mean*5*100;
      mean /= 1023; 
      Serial.print(mean);
      Serial.print("mV; Variation (RMS)= ");
      var = var*5*100;
      var = var/1023;
      Serial.print(var);
      Serial.println("(mV)^2; ");      
    }
    cnt=0;
    sw=0;
    digitalWrite(SW_POS,LOW);
    flag = 0;
  }

  if (cnt>9 && flag)//measurements during measure period
  {
    sample[cnt-10]=analogRead(A0);
    Serial.print("");
    flag=0;
    if (cnt>10 && abs(sample[cnt-10]-sample[cnt-11])>40)//when changing measured voltage mid work
      {
        cnt=0;
        sw=0;
        digitalWrite(8,LOW);
      }
  }
}

ISR(TIMER1_COMPA_vect){
   cnt++;
   flag=1;
}