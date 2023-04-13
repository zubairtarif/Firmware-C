// Timestamp 14:44

//NAVIGATION WORKS
// LAST CHECKED  24 JUNE 2022
// updating 01 mar 2023
// THIS IS CLONED FROM MOCK

#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>

//U8G2_ST7571_128X128_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, 8, 10, 9);//FOR GENERIC
U8G2_ST7571_128X128_1_4W_SW_SPI u8g2(U8G2_R0, 13, 11, A1, A3, A2);//FOR MICROCONTROLLER
//CS : 8 / A1                                         |
//RST : 9 / A2                                       |
//RS : 10 / A3



//#define TextFont8 u8g2_font_helvR08_tf

#define TextFontSmall u8g2_font_6x10_tr
#define TextFont8 u8g2_font_helvB10_tf //u8g2_font_4x6_tf//u8g2_font_helvB10_tf
#define TextFont10 u8g2_font_helvB10_tf
#define TextFontReading u8g2_font_fub25_tn
#define TextFontUnit u8g2_font_helvB18_tf //u8g2_font_luBS19_tf
#define GraphicsFont u8g2_font_unifont_t_symbols


float currentcpm;
int av=60;
long counts[60]={0};
int c=0;
long interval = 1000;
int battery[10]={};

int tim_minute=0;
int tim_second=59;

int unit_pos_y;
int unit_pos_x;

long powercheck=5000;
long start;
long now;
long currentreading=0;  // current reading in cpm, reset during every calculation.
                      // used for alarm threshold and other relevant referencing
long tempsum=0;

int SW;
int pos=0;
int unit=1;
int timervalue=3;
int batteryLevel=0;

float alarmvalue = 60; // CPM
float alarmvalue_uSvhr = 0.1; // CPM

String uSvcount;
String nSvcount;
String cpscount;
String cpmcount;
String xlivecount;
String xtotal;
String xtimervalue;
String livetimervalue;
String xalarmvalue;
String xalarmvalue_cps;
String xalarmvalue_uSvhr;
String xalarmunit;

bool SELECTED=0;
String Hstr;

String xminute;
String xsecond;

#define DISPLAY_EXIT_SELECT { u8g2.setFont(TextFont8); u8g2.drawStr(0,120,"Exit"); u8g2.drawStr(80,120,"Select");} //u8g2_font_luBS10_tf
#define DISPLAY_BACK_SELECT { u8g2.setFont(TextFont8); u8g2.drawStr(0,120,"Back"); u8g2.drawStr(80,120,"Select");}
#define DISPLAY_BACK_SET { u8g2.setFont(TextFont8); u8g2.drawStr(0,120,"Back"); u8g2.drawStr(100,120,"Set");}
#define DISPLAY_BACK_START { u8g2.setFont(TextFont8); u8g2.drawStr(0,120,"Back"); u8g2.drawStr(87,120,"Start");}
#define DISPLAY_BACK { u8g2.setFont(TextFont8); u8g2.drawStr(0,120,"Back");}
#define DISPLAY_TIMER_ACTIVE {u8g2.drawStr(82,10,"Timr"); livetimervalue = String(timervalue);}

long currentmillis=0;
long prevmillis=millis();
long previousmillis=millis();
long backlightmillis=0;
long audiomillis=0;
long beepmillis=0;
float dose=0.0;
float temp_dose=0.0;

int pulses=0;
int var_holdpulses=0;
float pulsehold=0;


int SW1;// HOLD = digitalRead(4);
int SW2;// MENU = digitalRead(5);

int SW3;// AUDIO = digitalRead(6);
int SW4;// BACKLIGHT = digitalRead(7);

int SW5;// DOSE = digitalRead(8);
//int SW6;// MODE = digitalRead(9;)

//There is no semicolon after the #define statement. If you include one,
//the compiler will throw cryptic errors further down the page.
 
#define HOLD_PRESS 4
#define MENU_PRESS 5
#define BACK_PRESS 5
#define AUDIO_PRESS 6
#define UP_PRESS 6
#define LIGHT_PRESS 7
#define DOWN_PRESS 7
#define DOSE_PRESS 8
#define ENTER_PRESS 8
#define TOGGLE_PRESS 9



int SELECT;
int BACK;
int UP;
int DOWN;
int barsize;
  
bool MENUFLAG;
bool ALARMFLAG;
bool HOLDFLAG;
bool BACKLIGHT_FLAG=1;
bool AUDIO_FLAG=1;
bool TIMERFLAG;
bool TIMERFINISH;
bool ALARM_TEXT=0;
bool DOSE_FLAG;
bool TIM_SEL=0;
bool ALARM_UNIT_SEL=0;


long randnumber;
float randnumberf;
float total=0;

int beeprand=750; //initialize first beeptime


//For Menu Text positioning

  int x = 20;
  int g1 = x+18; 
  int g2 = g1+17;
  int g3 = g2+17;
  int g4 = g3+17;
  int g5 = g4+17;

// INITIALIZE ENDS
  
void setup(void) {

  u8g2.begin();
  u8g2.setContrast(255);
  //pinMode(7, OUTPUT); 
  /*pinMode(A0, OUTPUT); // LED BLINK
  pinMode(10, OUTPUT); // BACKLIGHT SIGNAL
  pinMode(12, OUTPUT); // AUDIO DRIVING SIGNAL AND POWER

  digitalWrite(A0, LOW);
  digitalWrite(10, LOW);
  digitalWrite(12, LOW);*/
  attachInterrupt(0, countpulses, RISING);

  pinMode(3, OUTPUT);// TONE
  
  digitalWrite(3, LOW); // TONE, 18 Aug 22

  pinMode(A0, OUTPUT);  // AUDIO OUT
  digitalWrite(A0, HIGH); // AUDIO OUT

  pinMode(1, OUTPUT); // BACKLIGHT FLAG
  digitalWrite(1, HIGH);

  pinMode(A4,OUTPUT); //PWR_CTRL or DRIVE
  digitalWrite(A4,HIGH);
  pinMode(A5,INPUT); // SENSE
  pinMode(A6,INPUT); // Battery Check
  delay(2000);

  
  // BUTTONS
  /*pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT);*/

    // BUTTONS NEW
  pinMode(4,INPUT);
  pinMode(5,INPUT);
  pinMode(6,INPUT);
  pinMode(7,INPUT);
  pinMode(8,INPUT);
  pinMode(9,INPUT);
  
}


void check_power(){
  // POWER SWITCH

if (currentmillis - powercheck > 3000){
 if(digitalRead(A5)){  // If power button pressed
    start = millis();      // store start time
    while(digitalRead(A5)){  //Loop while power button still held
      now = millis();            
      if(now > (start + 3000)){  //If button held for 2000ms
        /*while(digitalRead(A5))*/ //Hold until power button is released
        //digitalWrite(A0, LOW); // AUDIO OUT
        u8g2.firstPage();
          do {
            u8g2.setFont(TextFont10);
            u8g2.drawStr(0,50,"Shutting");
            u8g2.drawStr(0,70,"Down. ");
          } while ( u8g2.nextPage() );
        delay(1000);
        u8g2.firstPage();
          do {
            u8g2.setFont(TextFont10);
            u8g2.drawStr(0,50,"Shutting");
            u8g2.drawStr(0,70,"Down. .");
          } while ( u8g2.nextPage() );
        delay(1000);
        u8g2.firstPage();
          do {
            u8g2.setFont(TextFont10);
            u8g2.drawStr(0,50,"Shutting");
            u8g2.drawStr(0,70,"Down. . .");
          } while ( u8g2.nextPage() );
        delay(2000);
        digitalWrite(A4,LOW);
        // Kill power to the whole circuit
      }
    }
  }
}
}

// POWER SWITCH





void drawBat(int L){
  //u8g2.drawBox(starting X,starting Y,Horizontal length,vertical Height);

  // Battery is 3.3 to 2.9
  // 3.3/1024 = 0.003
  // 0.4/0.003 = 133 steps to lose
  // L is maximum 1023
  // L is miniumum 1023 - 133 = 990
  // 133/18 = 7.38
  // 22 * 6 = 132
  // But we use, 870, so 1023 - 870 / 22 = 
  int H = abs((L-870))/7;

  if (H<=7){
    Hstr= String(H);
    barsize = 3;}
  else if (7<H && H<=18){
    Hstr= String(H);
    barsize = 6;}
  else if (18<H){
    Hstr= String(H);
    barsize = 21;}
  else {
    Hstr= String(H);
    barsize = 0;}
  // So about 22 levels
  u8g2.drawFrame(0,0,25,10); // Battery box
  u8g2.drawBox(25,2,3,6); //Battery Head
  u8g2.drawBox(2,2,barsize,6); // Level, max 18
  //u8g2.setFont(TextFont10);
  //u8g2.drawStr(0,25,Hstr.c_str());
}



void levelExceed(){
  u8g2.setFont(TextFont10);
  u8g2.drawStr(0,115,"Level Exceed!!");}
  
  
void drawLIGHT(){
  //(u8g2_t *u8g2, x0, y0, rad, U8G2_DRAW_ALL)
  //u8g2.drawCircle(50,10,5,U8G2_DRAW_ALL);
  u8g2.setFont(TextFontSmall);
  u8g2.drawStr(35,10,"BL");
  } 

void drawSOUND(){
  //(u8g2_t *u8g2, x0, y0, rad, U8G2_DRAW_ALL)
  //u8g2.drawCircle(30,10,5,U8G2_DRAW_ALL);
  //u8g2.drawCircle(35,20,5,U8G2_DRAW_ALL);
  u8g2.setFont(TextFontSmall);
  u8g2.drawStr(55,10,"AU"); 
  } 



void drawALARM(){
  //u8g2.drawBox(starting X,starting Y,Horizontal length,vertical Height);
  int bellcenterX = 100;
  int bellcenterY = 40; 
  int radius=5;
  int boxlength=10;
  int boxheight=8;
  /*u8g2.drawCircle(bellcenterX,bellcenterY+radius+1,2,U8G2_DRAW_ALL);
  u8g2.drawCircle(bellcenterX,bellcenterY,radius,U8G2_DRAW_UPPER_RIGHT); //semi circle
  u8g2.drawCircle(bellcenterX,bellcenterY,radius,U8G2_DRAW_UPPER_LEFT); //semi circle
  u8g2.drawBox(bellcenterX-radius,bellcenterY,boxlength,boxheight);
  u8g2.drawCircle(bellcenterX,bellcenterY+boxlength-1,4,U8G2_DRAW_ALL);*/
  u8g2.drawFilledEllipse(bellcenterX, bellcenterY, radius, radius, U8G2_DRAW_UPPER_RIGHT);
  u8g2.drawFilledEllipse(bellcenterX, bellcenterY, radius, radius, U8G2_DRAW_UPPER_LEFT);
  u8g2.drawBox(bellcenterX-radius,bellcenterY-radius,boxlength,boxheight);
  }


int pos_alarm_cps = 0;

void renderAlarm(){
  while(1){  
    check_power();
        //xalarmvalue = String(alarmvalue,3); // CPM
        xalarmvalue_cps = String(int(alarmvalue)); // CPM
        xalarmvalue_uSvhr = String(alarmvalue_uSvhr,1);
        if (alarmvalue>999) {
          pos_alarm_cps = 15;}
        else if (alarmvalue>99 && alarmvalue<= 999) {pos_alarm_cps = 25;}
        else if (alarmvalue>9 && alarmvalue<= 99) {pos_alarm_cps = 35;}
        else pos_alarm_cps = 45;
        
        
        u8g2.firstPage();
        do {
            u8g2.setFont(TextFont10);
            u8g2.drawStr(10,x+7,"Set Alarm Level:");

            
            switch (ALARM_UNIT_SEL) {

            case 0:
            xalarmunit = "cps";
            u8g2.setFont(TextFontReading); 
            u8g2.drawButtonUTF8(pos_alarm_cps, x+60, U8G2_BTN_INV, 0,  4,  4, xalarmvalue_cps.c_str());
            u8g2.setFont(TextFont8);
            u8g2.drawStr(24,x+20,xalarmunit.c_str());
            break; 

            case 1:
            xalarmunit = "Sv/h";
            
            u8g2.setFont(TextFontReading);  
            u8g2.drawButtonUTF8(25, x+60, U8G2_BTN_INV, 0,  4,  4, xalarmvalue_uSvhr.c_str());
            u8g2.setFont(TextFont8);
            u8g2.drawUTF8(30,x+20,"\xb5");
            u8g2.drawStr(24+19,x+23,xalarmunit.c_str());
            
            break;          
              
              }


          
            //u8g2.setFont(TextFont10);
            //u8g2.drawStr(0,x,"ALARM LEVEL:");
            //u8g2.drawStr(0,x+20,"<");
            //u8g2.drawStr(15,x+20,xalarmvalue.c_str());
            //u8g2.drawStr(80,x+20,">");
            //u8g2.drawStr(0,x+20+17,"Unit: ");
            //u8g2.drawUTF8(42,x+20+17,"cpm");  

            SW1 = checkBUTTON(TOGGLE_PRESS);
            if (SW1 ==LOW) {
              ALARM_UNIT_SEL = !ALARM_UNIT_SEL;
              }





            DISPLAY_BACK_SET;
            } while ( u8g2.nextPage() );
          BACK = checkBUTTON(BACK_PRESS);
          if (BACK ==LOW) {
            delay(750);
            break;}

          SELECT = checkBUTTON(ENTER_PRESS);
            if (SELECT ==LOW) {
              SELECTED = 1;
              ALARMFLAG = 1;
              
              delay(750);
              break;}    // BREAK OF WHILE
          
          
          UP = checkBUTTON(UP_PRESS);
              if (UP == LOW) {

            switch (ALARM_UNIT_SEL) {

            case 0:
            //cps
            if (alarmvalue > 3500){
            alarmvalue = 5;}
            else alarmvalue = alarmvalue+5;
            break; 

            case 1:
            //uSvhr
            if (alarmvalue_uSvhr > 999.9){
            alarmvalue_uSvhr = 0.1;}
            else alarmvalue_uSvhr = alarmvalue_uSvhr+0.1;
            
            break;          
              
              }
   
             }
    

            DOWN = checkBUTTON(DOWN_PRESS);
            if (DOWN == LOW) {
            switch (ALARM_UNIT_SEL) {

            case 0:
            //cps
            if (alarmvalue <= 5 ){
            alarmvalue = 3500;}
            else alarmvalue = alarmvalue-5;
            break; 

            case 1:
            //uSvhr
            if (alarmvalue_uSvhr <= 0.1){
            alarmvalue_uSvhr = 999.9;}
            else alarmvalue_uSvhr = alarmvalue_uSvhr-0.1;
            
            break;          
              
              }
              }
        }
        
  
  }



void renderTimer(){
  while(1){
        check_power();
        xminute = String(tim_minute);
        xsecond = String(tim_second);
        u8g2.firstPage();
        do {
            u8g2.setFont(TextFont10);
            u8g2.drawStr(20,x+6,"Set Timer:");
            u8g2.setFont(TextFont8);
            u8g2.drawStr(24,x+20,"(min:sec)");
            
            switch (TIM_SEL) {

            case 0:
            u8g2.setFont(TextFontReading); 
            u8g2.drawButtonUTF8(10, x+60, U8G2_BTN_INV, 0,  4,  4, xminute.c_str());
            u8g2.drawStr(50, x+60,":");
            u8g2.drawStr(70, x+60,xsecond.c_str());
            break; 

            case 1:
            u8g2.setFont(TextFontReading);  
            u8g2.drawStr(10, x+60, xminute.c_str());
            u8g2.drawStr(50, x+60,":");
            u8g2.drawButtonUTF8(70, x+60, U8G2_BTN_INV, 0,  4,  4, xsecond.c_str());
            break;          
              
              }
            
            
            
            SW1 = checkBUTTON(TOGGLE_PRESS);
            if (SW1 ==LOW) {
              TIM_SEL = !TIM_SEL;}
            
            
            
            //u8g2.drawStr(5,x+17,"<");
            
            
            /*u8g2.drawStr(0,x+17,"<");
            u8g2.drawStr(30,x+17,xtimervalue.c_str());
            u8g2.drawStr(65,x+17,"sec");
            u8g2.drawStr(100,x+17,">");*/
            DISPLAY_BACK_START;
            } while ( u8g2.nextPage() );
            BACK = checkBUTTON(BACK_PRESS);
            if (BACK ==LOW) {
              delay(750);
              break;}

            SELECT = checkBUTTON(ENTER_PRESS);
            if (SELECT ==LOW) {
              SELECTED = 1;
              TIMERFLAG=1;
              TIMERFINISH=0;
              unit=0;
              currentcpm=0;
              delay(750);
              break;}



            UP = checkBUTTON(UP_PRESS);
            if (UP == LOW) {

            switch (TIM_SEL) {
            case 0: 
            if (tim_minute >= 59){
                tim_minute = 0;}
              else tim_minute = tim_minute+1;
            break;

            case 1: 
            if (tim_second >= 59){
                tim_second = 0;}
              else tim_second = tim_second+1;
            break;          
              
              }

  
             }
    

            DOWN = checkBUTTON(DOWN_PRESS);
            if (DOWN == LOW) {

            switch (TIM_SEL) {
            case 0: 
            if (tim_minute <= 0){
                tim_minute = 59;}
              else tim_minute = tim_minute-1;
            break;

            case 1: 
            if (tim_second <= 0){
                tim_second = 59;}
              else tim_second = tim_second-1;
            break;          
              
              }

  



              }
                  }
         
    
  }





void renderMenu() {
  
  
u8g2.firstPage();
do {
  u8g2.setFont(TextFont8);
  u8g2.drawStr(0, x, "****** Menu ******");
  u8g2.setFont(TextFont10);
  u8g2.setFont(TextFont10);
  u8g2.drawStr(10, g1, "Alarm");
  u8g2.drawStr(10, g2, "Timer");
  u8g2.drawStr(10, g3, "Battery Level");
  u8g2.setFont(TextFont10);
  u8g2.drawStr(10, g4, "Reboot");
  u8g2.drawStr(0, g1 + pos * 17, ">");

  DISPLAY_EXIT_SELECT;

} while (u8g2.nextPage());

BACK = checkBUTTON(BACK_PRESS);
if (BACK == LOW) {
  MENUFLAG = LOW;
  pos = 0;
}

UP = checkBUTTON(UP_PRESS);
if (UP == LOW) {
  if (pos == 0) {
    pos = 3;
  }
  else {
    pos = pos - 1;
  }
}

DOWN = checkBUTTON(DOWN_PRESS);
if (DOWN == LOW) {
  if (pos == 3) {
    pos = 0;
  }
  else {
    pos = pos + 1;
  }
}

SELECT = checkBUTTON(ENTER_PRESS);
if (SELECT == LOW) {
  switch (pos) {
    case 0:
      renderAlarm();
      break;
    
    case 1:
      renderTimer();
      break;
    
    case 2:
      while (1) {
        u8g2.firstPage();
        do {
          u8g2.setFont(TextFont10);
          u8g2.drawStr(0, x, "Battery: 89%");
          DISPLAY_BACK;
        } while (u8g2.nextPage());
        BACK = checkBUTTON(BACK_PRESS);
        if (BACK == LOW) {
          delay(750);
          break;
        }
      }
      break;
      
    case 3:
      while (1) {
        u8g2.firstPage();
        do {
          u8g2.setFont(TextFont10);
          u8g2.drawStr(0, x, "Reboot Function");
          u8g2.drawStr(0, x + 17, "Disabled.");
          DISPLAY_BACK;
        } while (u8g2.nextPage());
        BACK = checkBUTTON(BACK_PRESS);
        if (BACK == LOW) {
          delay(750);
          break;
        }
      }
      break;
  }
}



}


void display_reading(String reading) {
  
    u8g2.setFont(TextFontReading); // FONT FOR READING
    u8g2.drawStr(0,60,reading.c_str());
    
  
  }


void display_unit(int unit_input){

    unit_pos_y = 90;
    unit_pos_x = 0;
    u8g2.setFont(TextFontUnit); // FONT FOR UNIT
    switch(unit_input){

      case 0: if (dose <= 1000) {
              u8g2.drawUTF8(unit_pos_x,unit_pos_y,"n");}
              else u8g2.drawUTF8(unit_pos_x,unit_pos_y,"\xb5");
              u8g2.drawStr(unit_pos_x+13,unit_pos_y,"Sv");
              break;

      case 1: u8g2.drawStr(unit_pos_x,unit_pos_y,"cps");
              break;

      case 2: u8g2.drawStr(unit_pos_x,unit_pos_y,"cpm");
              break;

      case 3: u8g2.drawUTF8(unit_pos_x,unit_pos_y,"\xb5");
              u8g2.drawStr(unit_pos_x+13,unit_pos_y,"Sv/h");
              break;      
                      }
      }   


 void timer_update(){  
 // TIMER UPDATE EVERY SECOND
 
    if (TIMERFLAG == 1) {
      timervalue = timervalue -1;
      if (timervalue == -1){
        timervalue = 3;
        TIMERFLAG = 0;
        TIMERFINISH = 1;
      }
        }
 }

void display_timer () {
  u8g2.setFont(TextFontSmall);
    DISPLAY_TIMER_ACTIVE;
    u8g2.drawStr(82,27,livetimervalue.c_str());
 
  }    


void check_alarm(){
  // currentreading is in cpm
  //the alarm flag says that the alarm has been set

  while (ALARMFLAG ==1) {
    if(currentreading > alarmvalue*60){
      if (digitalRead(3 == LOW)){
        digitalWrite(3, HIGH);} // TONE
      ALARM_TEXT == 1;} 
    
    else if (currentreading <= alarmvalue*60) {
      if (digitalRead(3 == HIGH)){
        digitalWrite(3, LOW);// TONE
      }
      ALARM_TEXT == 0;}

  }
  
  }



long deadtime_correction(long value){

    //Dead time correction
    long base = value;
    long A = 361*base*base;
    A=A/1000;
    A=A/1000;
    long B = 11759*base;
    B = B/10000;
    return A + B;
    //Dead Time correction
  
  }



void holdpulses () {
  var_holdpulses = pulses; 
  
  }

void releasepulses () {
  pulses = var_holdpulses;
  }


void loop(void) {
currentmillis=millis();



check_power();



batteryLevel = analogRead(A6);





// STORE READING START

if(currentmillis - previousmillis > interval)
{
pulsehold=pulses;
pulses=0;      
previousmillis = currentmillis;

    for (c=60;c>=2;c--){
    counts[c-1]=counts[c-2];
    }
    counts[0]=pulsehold;
    pulses=0; // RESET THE INTERRUPT VARIABLE
    total=total+pulsehold;

    if (counts[0]<3){ // cps : counts obtained during the interval // 3 CPS = 180 CPM
      av=60;}
    if (counts[0]>=3 && counts[0]<15 ){
      av=6;}
    if (counts[0]>=15){
      av=3;}

    pulsehold=0;


    // CALCULATE cpm value
    for (c=0;c<av;c++){
    currentcpm=currentcpm+counts[c]; // value if av = 60
    }
    tempsum = currentcpm; 
    currentcpm = deadtime_correction(tempsum);   
    
    
    if (av == 6)
      {currentcpm = currentcpm*10;}
    else if (av == 3)
      {currentcpm = currentcpm*20;} 
    else
      currentcpm = currentcpm;
    
    currentreading = long(currentcpm);

    
    // don't need xlivecount = String(currentcpm); // currentcpm is the value of CPM

    // currentcpm is the value that is processed for different readings.
    
    
    if (DOSE_FLAG == 1)
        { temp_dose = float(currentcpm)/108.0;
          temp_dose = temp_dose/3.6;
          dose = dose + temp_dose;}


 void timer_update();  // TIMER UPDATE EVERY SECOND
      
}

// STORE READING FINISH










// RENDER DEFAULT DISPLAY

if (currentmillis-prevmillis>1200 || HOLDFLAG==1 || TIMERFINISH == 1){ // UPDATE WHEN HOLD IS PRESSED
    
    prevmillis=currentmillis;
    
// Decide the Value based on unit set
    
    switch (unit){
    case 0: uSvcount = String(dose/1000.0,3); // DOSE
            nSvcount = String(dose,3);
            if (dose <= 0.001) {
              xlivecount=nSvcount;}
            else xlivecount=uSvcount;
            break;
    
    case 1: xlivecount=String(counts[0]); // CPS
            break;
    
    case 2: xlivecount=String(int(currentcpm)); // CPM
            break;
    
    case 3: xlivecount=String((currentcpm/108.0),3); // uSV/HR
            break;

  }



// ALARM TONE
check_alarm();

//RESET currentcpm
currentcpm=0;



  u8g2.firstPage();
  do {    
    display_reading(xlivecount);
    display_unit(unit);


    
    drawBat(batteryLevel);
    
    if (HOLDFLAG == 1) 
    {u8g2.setFont(TextFontSmall);
    u8g2.drawStr(82,10,"Paused");}

    if (TIMERFLAG == 1) 
    {display_timer();}

    if (TIMERFINISH == 1) 
    {u8g2.setFont(TextFont10);
    u8g2.drawStr(50,12,"Timer Ended");
    u8g2.drawStr(0,115,"Press BACK to exit");}   
    
    if(ALARMFLAG == 1){
      drawALARM();} // ALARM ICON - means alarm is set
    
    if(ALARM_TEXT == 1){
      levelExceed();} // ALARM TEXT only when level exceed
 
    if (BACKLIGHT_FLAG == 1){
      drawLIGHT();}

    if (AUDIO_FLAG == 1){
      drawSOUND();}

  } while ( u8g2.nextPage() );
  
} //if current millis ends 





// run this after setting the hold flag, so paused text is printed.
if (HOLDFLAG==1) {
delay(2000);
while(1){
  SW1 = checkBUTTON(HOLD_PRESS);
  if (SW1 == LOW) {
    delay(500);
    HOLDFLAG=0;
    break;
}
  }
}



// run this after setting the hold flag, so paused text is printed.
if (TIMERFINISH==1) {
delay(2000);
while(1){
  SW2 = checkBUTTON(BACK_PRESS);
  if (SW2 == LOW) { // low means pressed
    delay(500);
    TIMERFINISH=0;
    break;
}
  }
}






SW1 = checkBUTTON(HOLD_PRESS); // HOLD
if (SW1 == LOW){
HOLDFLAG=1;
delay(500);
}



SW2 = checkBUTTON(MENU_PRESS); // MENU OR BACK
if (SW2 == LOW){
  MENUFLAG = HIGH;
  while (MENUFLAG == HIGH)
  {renderMenu();
  
  if (SELECTED == 1) {
  SELECTED = 0;
  MENUFLAG = LOW;
  break;}
  }




} // if SW2 closes 

 

SW3 = 0; // audio
if (checkBUTTON(AUDIO_PRESS) == LOW){
  if (AUDIO_FLAG == 0) 
  {AUDIO_FLAG = 1;
  digitalWrite(A0, HIGH);
  audiomillis=millis();
  delay(200);}
  
  if (currentmillis-audiomillis>750){
    
  if (AUDIO_FLAG == 1) 
  {AUDIO_FLAG = 0;
  digitalWrite(A0, LOW);
  delay(200);}} } 



SW4 = 0;//checkBUTTON(LIGHT_PRESS); // BACKLIGHT
if (checkBUTTON(LIGHT_PRESS) == LOW){

  if (BACKLIGHT_FLAG == 0) 
  {BACKLIGHT_FLAG = 1;
  digitalWrite(1, HIGH);
  backlightmillis=millis();
  delay(200);}
  
  // if we want to manually switch off the backlight. this loop only enters when SWITCH IS ALREADY PRESSED ONCE AND BACKLIGHT IS ON
  // so we give a buffer time of 0.75 sec before switch can be pressed to turn off
  if (currentmillis-backlightmillis>750){
  if (BACKLIGHT_FLAG == 1) 
  {BACKLIGHT_FLAG = 0;
  digitalWrite(1, LOW);
  delay(200);}}
   

}


SW5 = checkBUTTON(DOSE_PRESS); //DOSE
if (SW5 == LOW){
DOSE_FLAG = 1;
dose = 0;
unit=0;}    


//SW6 = 0; //checkBUTTON(TOGGLE_PRESS); // MODE

if (checkBUTTON(TOGGLE_PRESS) == LOW){
unit++;
if (unit>3) {unit=1;}  
}

}




//Function Definitions

int checkBUTTON(int x)
{SW = digitalRead(x);
if (SW == LOW) // first loop
{ delay(50);  
  }
if (SW == LOW) {return SW;}

}





void countpulses() {
  pulses++;
}
