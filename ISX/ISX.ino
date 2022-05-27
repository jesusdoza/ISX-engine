

//GLOBAL VAR

int crankPin=32;  //CRANK PC5    _______________PINS I USED
int camPin=30;  //CAM  PC7
//GLOBAL VAR
int exitPin =22;
int enterPin = 23;
int selectPin= 24;
int rpmUp= 25;// make RPM go up
int rpmDown=26;// make RPM go down


void setup() {
 
  // put your setup code here, to run once:
pinMode(crankPin,OUTPUT);// used led green
pinMode(camPin,OUTPUT);//used led white
pinMode(enterPin,INPUT_PULLUP);  //pin for enter
pinMode(exitPin,INPUT_PULLUP); //exit pin setup
pinMode(selectPin,INPUT_PULLUP); 
pinMode(rpmUp,INPUT_PULLUP); 
pinMode(rpmDown,INPUT_PULLUP); 
Serial.begin(115200);
}







//CRANK SETTINGS****************************************CRANK SETTINGS******************************************************************************
bool doLow=false;
unsigned long crankCounter=0;
unsigned long crankRatio=58;// how many normal crank pulses to one crank sync pulse using 

unsigned long crankTrigger=0;//stores time in micros of next pulse HIGH

int crankPulseInterval=600;// in micros, pulse HIGH every #
int crankLowInterval=600;//in micros, pulse LOW every #

int crankSyncSkips=0;//how many times to left we can ignore the LOW state will vary depending on crank sync length
int crankSyncLength=3;//how many cycles should we skip the low state in order to create the HIGH SYNC PULSE

long doSyncLogicGuard=123421;// used to make sure crank sync block happens only once, random number to start with will get corrected later just need something that will never be at begining

//CRANK SETTINGS****************************************CRANK SETTINGS ABOVE******************************************************************************






//CAM SETTINGS****************************************CAM SETTINGS******************************************************************************
unsigned int camCounter=0;//keeps track of how many cam pulses have occured
int camRatio=7;// ratio of normal cam pulses to one cam sync again wont work at 6 so i  used 7
unsigned int camTrigger=28;//________________________# you want -1 CHANGE TO MAKE CAM PULSES START AT SPECIFIC CRANK COUNT_______!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int medCamTrigger=0;// will hold value that has to match camcrankcounter to trigger medium pulse settings

 //pulse length in terms of crank pulses
int crankToCamRatio=18; //how many crank normal pulses to CAM normal pulse lasts was 18
int crankToCamMedRatio=13;//medium pulse lasts this many crank pulses was 13
int crankToCamSyncRatio=3;//how many crank normal pulses to CAM SYNC pulse was 2

int camLowSkips=3;//how many crank pulses to stay in LOW state in between pulses
int camPulseLength=0;//will hold either crankToCamRatio  or   crankToCamSyncRatio or crankToCamMedRatio used later to control next pulse trigger
unsigned int camCrankCounter=1;// cam keeps track of crank pulses including factoring in the syncs which take up more than one

//CAM SETTINGS****************************************CAM SETTINGS ABOVE******************************************************************************







//*******************************************RPM SETTINGS*******************************************
int rpmIncrement=50;// increment to change RPM by
int currentRPM=800;















//***********************************************************LOOP START****************************************************************
void loop() {


rpmInput();//checks to see if rpm buttons have been pushed

//
//Serial.println("crankskips: ");
//Serial.println(crankSyncSkips);




//______________________________________________________START CRANK TIMING LOGIC__________________________________________
//crank SYNC if its time to sync, if its a multiple of my ratio
//if(crankCounter%(crankRatio+crankSyncLength)==0||crankCounter==0){//____________________________________OLD starts with SYNC on CRANK
if(crankCounter%(crankRatio)==0&&crankCounter!=0&&camCounter!=doSyncLogicGuard){//if crankcounter is multiple of crankratio and dont start with sync, crankcounter is not 0,
 crankSyncSkips=crankSyncLength-1;// zero counts so subtract 1
 doSyncLogicGuard=camCounter;//making sure crank sync does not happen during the exact same cam pulse again
}
//else if (crankSyncSkips>0){//else if set skips, to whatever is still in them, more for just documentation 
// crankSyncSkips=crankSyncSkips;
//}
//else{// do nothing

//}
//______________________________________________________END CRANK TIMING LOGIC__________________________________________



//______________________________________________________START CAM TIMING LOGIC__________________________________________
//CAM sync - if its time to SYNC, assign value to camtrigger to be used later for timing according to camcrankcounter
if (camCounter%camRatio==0){//if the cam counter is a multiple of camratio
 camPulseLength=crankToCamSyncRatio-1; //zero counts so subtract 1
 medCamTrigger=camCounter+1;//set medium trigger just after current trigger since MEDIUM always follows SYNC
}
else if((camCounter==medCamTrigger)){//medium comes after small pulse which was set above
  camPulseLength=crankToCamMedRatio-1;//set pulse length to medium 
}
else{
  camPulseLength=crankToCamRatio-1;  // use normal CAM length for all other pulses
}

//______________________________________________________END CAM TIMING LOGIC__________________________________________





//__________________________________________________START PULSES_______________________________________________________ 
//if its time to begin HIGH pulse of CRANK 
if((micros()>=crankTrigger)){//if time is greater or equal to the cranktrigger time
  
doLow=true;//used to make sure the LOW pulse for crank happens only once inside its LOW window
 //crankCounter++;

    if(camCrankCounter==camTrigger){//turn both crank and cam at same time if counter and camTrigger match      
          PORTC=(1<<PC7)|(1<<PC5);//turn both pins on without delay using pin registers
          camTrigger=camLowSkips+camPulseLength+camCrankCounter;//set new camtrigger for future next pulse using camtrigger interval from above
          camCounter++;//
        
    }
    //____________________________________________________________________CAM PULSE END
    //else just do the CRANK its not time yet for cam
    else{    
      digitalWrite(crankPin,HIGH);
      crankTrigger=(micros()+(crankPulseInterval+crankLowInterval)); //set new trigger for CRANK PULSE in the future
     // crankCounter++;
    }

   // crankCounter++;
}


//else if its time for CRANK TO GO LOW and also make cam go low or HIGH depending on counter
else if(micros()>=(crankTrigger-crankLowInterval) && doLow==true ){//making sure falling edge is before next crank trigger by subtracting low state from trigger
  
  doLow=false;//disable this else if block, to make sure it only runs once
   //NORMAL CRANK PULSE has 0 skips left 
  //if we are not in a crank sync pulse go LOW and increment counter CRANK
  if(crankSyncSkips<=0){ //
   
    digitalWrite(crankPin,LOW); 
   
     
     
     //CAM GO LOW LOGIC inside crank normal pulse____________START CAM LOGIC
     
     //if camcrankcounter matches camtrigger # - camlow skips "way  to activate low with math"
    if(camCrankCounter==camTrigger-camLowSkips){
      //camCounter++;
      digitalWrite(camPin,LOW); 
    }
    //___________________________________________________________END CAM LOGIC
    crankCounter++;
  }

  //crank sync pulse has more than 0 skips
  //else if there are still skips left do nothing to pin "leave it high" for CRANK ONLY. WE ARE still in CRANK SYNC PULSE
  else if(crankSyncSkips>0){
    crankSyncSkips--; //decrement skips for next go around
    //digitalWrite(crankPin,HIGH); leave it high
   
    
     //CAM go LOW LOGIC inside crank SYNC pulse____________START CAM LOGIC
      if(camCrankCounter==(camTrigger-camLowSkips)){//making sure falling edge is before the trigger set in future substract camlowskips
      //cam is set LOW
      digitalWrite(camPin,LOW);
      
    }
     //___________________________________________________________END CAM LOGIC
     
    
    //crankCounter++;//crank counter used with crank only for sync pulse, skeewed for crank _____________________THIS IS not accurate _____________OLD
    
  }
doLow=false;//disable this else if block, to make sure it only runs once
  camCrankCounter++;//for cam to keep accurate crank counter not skewed by crank
}

//__________________________________________________END PULSES_______________________________________________________ 









}// _____________________________________END  OF VOID LOOP























//adds or subtracts from crank pulse intervals at specified increments
void rpmInput(){
if(millis()%1500==0){  
  if(digitalRead(rpmUp)==LOW){
    setRPM(currentRPM+rpmIncrement);
//    Serial.print("RPM up: ");
//    Serial.print(rpmCalc());
//    Serial.println();
  }
  else if (digitalRead(rpmDown)==LOW){
    setRPM(currentRPM-rpmIncrement);   
//    Serial.print("RPM down: ");
//    Serial.print(rpmCalc());
//    Serial.println();
  } 
}
else{
  return;
}
return;
}



//calculates RPM for current settings
float rpmCalc(){ 
  float rpm=0;// will hold rpm
  float sixtyPulses=0;// stores how much time it takes to do 60 pulses 
  long secToMicro=1000000;// one second is this many micros
  float pulseTime=crankPulseInterval+crankLowInterval;//takes this long to make one pulse HIGH and LOW
  
  sixtyPulses=pulseTime*(crankRatio+crankSyncLength); //one pulse time period multiplied by however many pulses for one revolution, crankratio is 57 and cranksync takes up 3
  rpm=(secToMicro/sixtyPulses)*60;//divide one sec in micros by sixty pulses time period, and multiply by 60 seconds for RPM minute 
  
  return rpm ;
}

//calculates the required pulse spacing for desired RPM
float getSpacingForRpm(int RPM_){
  float revsPerSec=0;
  int wantedSpacing=0;
  
  revsPerSec=(RPM_/60.00);// change RPM minute to RPS revs per second
  wantedSpacing=((1000000.00/revsPerSec)/60.00)/2.0;//divide a SEC in micros by the revs per second, then divide that by how many pulses in full crank revolution 60, and divide by 2 for the HIGH and LOW of each pulse
  
  return wantedSpacing;
    
}


//changes timing to desired timing on both HIGH and LOW 
void changeTimingCrank(int NEWSPACING_){
 crankPulseInterval=NEWSPACING_;
 crankLowInterval=NEWSPACING_;

  return;
}

//changes rpm settings to get the wanted rpm output wave
void setRPM(int wantedRpm_){
  if(wantedRpm_>0){// if wanted rpm is above 0 then its ok to change rpm
  int spacing=getSpacingForRpm(wantedRpm_);//call getspacingforrpm function 
  changeTimingCrank(spacing);//call changeTimingCrank function
  currentRPM=wantedRpm_;//update currentRPM global variable
  return;
  }
  else{//else wanted rpm is below 0 and cannot be done print message and return
//    Serial.print("RPM to low :");
//    Serial.print(wantedRpm_);
//    Serial.println();
    return;
  }
}
