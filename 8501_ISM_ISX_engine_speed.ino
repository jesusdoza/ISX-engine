

//GLOBAL VAR

int crankPin=3;
int startPin=12;
long long triggerTime=0; //when should event happen in micros

long startSpacing=100000;//this is starting spacing to start engine slowly
int crankingDifference=500;//used to slowly bring up rpm speed from 0 RPM to wanted RPM


long triggerSpacing=8333; //in micros 2500 micros=1978 RPM, 10,000 micros = 500 RPM 8,333 micros = 600 RPM somewhat formula (100,000/(wanted rpm/50)) = wanted micros for RPM
long triggerLowSpacing=triggerSpacing*.75;//low has to happen before next trigger so will always be less than triggerspacing
bool triggerOccured=false;

bool spacingChange=false;// will keep track if recalculation for spacing at wanted RPM is needed
int wantedRpm=0;//to be used later with formula for RPM and pulse spacing


void setup() {
  // put your setup code here, to run once:
pinMode(crankPin,OUTPUT);
pinMode(startPin,INPUT_PULLUP);
}

void loop() {
//recalculate spacing because rpm was changed
if(spacingChange){
  triggerSpacing=spacingFormula(wantedRpm);
}
  
//if we already triggered a high pulse then recalculate trigger time and set flag false
if(triggerOccured){
  //
  if(triggerSpacing!=startSpacing){  
  triggerTime=reduceTriggerSpacing(crankingDifference);//reduce spacing slowly to simulate starting engine
  triggerOccured=false;
  }
  else{
  triggerTime=micros()+triggerSpacing;
  triggerOccured=false;
  }
}


//if start pin is low and its the trigger time or over 
if((digitalRead(startPin)==LOW) && (micros()>=triggerTime)){
  triggerPulse(crankPin);
  triggerOccured=true;
}
//else check trigger time for it to go low 
else if (micros()>=(triggerTime-triggerLowSpacing)){
  endPulse(crankPin);
}




}

//****************************************************END OF MAIN LOOP*************************************************************




//******************************************************FUNCTIONS BELOW***************************************************************
//returns spacing for wanted RPM
long spacingFormula(int rpm_){
  return (100000/(rpm_/50));   
}

//reduces trigger spacing by desired amount
long reduceTriggerSpacing(int difference_){
  startSpacing=startSpacing-difference_;
  triggerTime=micros()+(startSpacing);
 return triggerTime;
}
void triggerPulse(int pin_){
  digitalWrite(pin_,HIGH);
  return;
}
void endPulse(int pin_){
  digitalWrite(pin_,LOW);
  return;
}
