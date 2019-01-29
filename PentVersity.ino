#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


// #################################################
// #################################################
// #####  MAX AND MIN VARIABLES FOR RSSI  ##########

int Ll = 110;
int Rl = 110;
int Bl = 110;
int Ml = 110;
int Cl = 110;

int Lu = 280;
int Ru = 280;
int Bu = 280;
int Mu = 280;
int Cu = 280;

int Lc = 150;
int Rc = 150;
int Bc = 150;
int Mc = 150;
int Cc = 150;

String P = "";

// Set the default video feed
int Vf = 4;

// #################################################
// #################################################
// #################################################


// Reset button is on Pin 7
const int ResetPin = 7;
int ResetState = 0;

// Channel buttons are on 8, 9 and 10
const int ChanOne = 8;
int OneState = 0;
const int ChanTwo = 9;
int TwoState = 0;
const int ChanThree = 10;
int ThreeState = 0;

Servo Hservo;  // create servo object to control a servo for horizontal movement
int Hval;    // variable to store the horizontal position
Servo Vservo;  // create servo object to control a servo for vertical movement
int Vval;    // variable to store the Vertical position

Servo VfAsw;  // create "servo" object to control video switcher - video feed A
int VfA;    // variable to store the horizontal position
Servo VfBsw;  // create servo object to control video switcher - video feed B
int VfB;  

// LED's to be replaced by OLED to free up some pins
// Set the pins LED's 
int LeftPin = 2;
int RightPin = 3;
int BottomPin = 4;
int MiddlePin = 6;
int RHCP = 5;

// the setup routine runs once when you press reset:
void setup() {

  Hval = 90; // Set Horiz servo to the middle
  Vval = 90; // Set Vert servo to the middle



  // Set the Pins for the Servos
  Hservo.attach(11);
  Vservo.attach(12);

  // Set the pin for thr Reset button
  pinMode(ResetPin, INPUT);

  // Set channel selection pins to inputs
  pinMode(ChanOne, INPUT);
  pinMode(ChanTwo, INPUT);
  pinMode(ChanThree, INPUT);

  // Define LED Pins as output
  pinMode(LeftPin, OUTPUT);
  pinMode(RightPin, OUTPUT);
  pinMode(BottomPin, OUTPUT);
  pinMode(MiddlePin, OUTPUT);
  pinMode(RHCP, OUTPUT);

  // Setup LCD
  lcd.begin(16, 2);
  lcd.noAutoscroll();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pentversity Beta");
  lcd.setCursor(0, 1);
  lcd.print("Initialising ...");
  delay(5000);
}

int Perc(double MaxVal, double MinVal, double CurVal) {
  double result;
  result = ((CurVal - MinVal) / (MaxVal - MinVal)) * 100;
  return result;

}


// Sub To turn on LEDs depending on RSSI value
void Lights(String Location) {

  // Turn all LED's off in preperation for turning the correct one on
  digitalWrite(LeftPin, LOW);
  digitalWrite(RightPin, LOW);
  digitalWrite(BottomPin, LOW);
  digitalWrite(MiddlePin, LOW);
  digitalWrite(RHCP, LOW);

  //Turn correct LED on

  if (Location == "LEFT") {
    digitalWrite(LeftPin, HIGH);
  }
  else if (Location == "RIGHT") {
    digitalWrite(RightPin, HIGH);
  }
  else if (Location == "BOTTOM") {
    digitalWrite(BottomPin, HIGH);
  }
  else if (Location == "MIDDLE") {
    digitalWrite(MiddlePin, HIGH);
  }
  else if (Location == "RHCP") {
    digitalWrite(RHCP, HIGH);
  }
  else if (Location == "ALL") {
    digitalWrite(LeftPin, HIGH);
    digitalWrite(RightPin, HIGH);
    digitalWrite(BottomPin, HIGH);
    digitalWrite(MiddlePin, HIGH);
    digitalWrite(RHCP, HIGH);
  }
}


void VidFeed(int Feed) {
  if (Feed == 0) {
    VfA = 0;
    VfB = 0;
  }
  else if (Feed == 1) {
    VfA = 90;
    VfB = 0;
  }
  else if (Feed == 2) {
    VfA = 180;
    VfB = 0;
  }
  else if (Feed == 3) {
    VfA = 0;
    VfB = 90;
  }
  else if (Feed == 4) {
    VfA = 0;
    VfB = 180;
  }
  VfAsw.write(VfA);
  VfBsw.write(VfB);
}



//  Sub to move servo by name and value
void Moveit(String ServoName, int Move) {
  if (ServoName = "X") {
    if (Hval > 0 and Hval < 180) {
      Hval = Hval + Move;

      if (Hval <= 0) {
        Hval = 0;
        Lights("ALL");
      }

      if (Hval >= 180) {
        Hval = 180;
        Lights("ALL");
      }
      Hservo.write(Hval);
    }
  }

  else if (ServoName = "Y") {

    if (Vval > 0 and Vval < 180) {

      Vval = Vval + Move;
      if (Vval <= 0) {
        Vval = 0;
        //Lights("ALL");
      }

      if (Vval >= 180) {
        Vval = 180;
        //Lights("ALL");
      }

      Vservo.write(Vval);
    }
  }
}

// the loop routine runs over and over again forever:
void loop() {

  int Rssi[14];

  int MaxRssiCOREVal;
  int MaxRssiCOREArr;

  int MaxRssiSECVal;
  int MaxRssiSECArr;

  int MaxRssiTHIVal;
  int MaxRssiTHIArr;

  int MaxRssiFOUVal;
  int MaxRssiFOUArr;

  ResetState = digitalRead(ResetPin);

  OneState = digitalRead(ChanOne);
  TwoState = digitalRead(ChanTwo);
  ThreeState = digitalRead(ChanThree);


  // Reset the Max values ready to seek the new highest

  MaxRssiCOREVal = 0;
  MaxRssiCOREArr = 0;
  MaxRssiSECVal = 0;
  MaxRssiSECArr = 0;
  MaxRssiTHIVal = 0;
  MaxRssiTHIArr = 0;
  MaxRssiFOUVal = 0;
  MaxRssiFOUArr = 0;

  // #### INSERT CODE FOR BUTTON RESET!!!! ####


  if (ResetState == HIGH) {
    lcd.setCursor(0, 1);
    lcd.print("Reset button!");
    Hval = 90;
    Vval = 90;
    Lights("ALL");
    delay(1000);
  }


  // ### Detect what channel is selected ###

  // ###  Can this be changed to SPI to get full range of frequencies??? maybe in v2 ###
  
  lcd.clear();
  lcd.setCursor(0, 0);
  if (OneState == LOW and TwoState == LOW and ThreeState == LOW) {
    // 5705
    F = padding("FR = 5705 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == HIGH and TwoState == LOW and ThreeState == LOW) {
    // 5685
    F = padding("FR = 5685 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == LOW and TwoState == HIGH and ThreeState == LOW) {
    // 5665
    F = padding("FR = 5665 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == HIGH and TwoState == HIGH and ThreeState == LOW) {
    //  5645
    F = padding("FR = 5645 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == LOW and TwoState == LOW and ThreeState == HIGH) {
    //  5885
    F = padding("FR = 5885 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == HIGH and TwoState == LOW and ThreeState == HIGH) {
    //  5905
    F = padding("FR = 5905 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == LOW and TwoState == HIGH and ThreeState == HIGH) {
    // 5925
    F = padding("FR = 5925 GHz", 16);
    lcd.print(F);
  }
  else if (OneState == HIGH and TwoState == HIGH and ThreeState == HIGH) {
    //  5945
    F = padding("FR = 5945 GHz", 16);
    lcd.print(F);
  }






  // Try Self Calibration of the maximum and minimum values
  // in theory, starting up the p-versity receiver without a transmitter running should give a good min value
  // starting up the p-versity receiver with a transmitter running on the correct channel should give a good max value
  Lc = analogRead(A0);  //  Left
  if (Lc < Ll) {
    Ll = Lc;
  }
  if (Lc > Lu) {
    Lu = Lc;
  }
  Rc = analogRead(A1);  //  Right
  if (Rc < Rl) {
    Rl = Rc;
  }
  if (Rc > Ru) {
    Ru = Rc;
  }
  Bc = analogRead(A2);  //  Bottom
  if (Bc < Bl) {
    Bl = Bc;
  }
  if (Bc > Bu) {
    Bu = Bc;
  }
  Mc = analogRead(A3);  //  Middle
  if (Mc < Ml) {
    Ml = Mc;
  }
  if (Mc > Mu) {
    Mu = Mc;
  }
  Cc = analogRead(A4);  // RHCP
  if (Cc < Cl) {
    Cl = Cc;
  }
  if (Cc > Cu) {
    Cu = Cc;
  }

  // Read the RSSI values from 5 pins - Take an average of 5 readings
  for (int i = 1; i <= 5; i++) {
    Rssi[0] = (Rssi[0] + Perc(Lu, Ll, analogRead(A0))) / i; //  Left
    Rssi[1] = (Rssi[1] + Perc(Ru, Rl, analogRead(A1))) / i; //  Right
    Rssi[2] = (Rssi[2] + Perc(Bu, Bl, analogRead(A2))) / i; //  Bottom
    Rssi[3] = (Rssi[3] + Perc(Mu, Ml, analogRead(A3))) / i; //  Middle
    Rssi[4] = (Rssi[4] + Perc(Cu, Cl, analogRead(A4))) / i; //  RHCP
    delay (25);  // Give the RX5808's a chance to have a think about the RSSI
  }

  //  Calculate the mean RSSI values so we can figure out what direction the signal is coming from

  Rssi[5] = ((Rssi[0] + Rssi[1]) / 2);  //  Left, Right Average - NORTH
  Rssi[6] = ((Rssi[1] + Rssi[2]) / 2 );  //  Right, Bottom Average - SOUTH EAST
  Rssi[7] = ((Rssi[2] + Rssi[0]) / 2 );  //  Bottom, Left Average - SOUTH WEST

  Rssi[8] = ((Rssi[3] + Rssi[0]) / 2 );  //  Middle, Left Average
  Rssi[9] = ((Rssi[3] + Rssi[1]) / 2 );  //  Middle, Right Average
  Rssi[10] = ((Rssi[3] + Rssi[2]) / 2 );  //  Middle, Bottom Average

  Rssi[11] = ((Rssi[3] + Rssi[5]) / 2 );  //  Middle, NORTH
  Rssi[12] = ((Rssi[3] + Rssi[6]) / 2 );  //  Middle, SOUTH EAST
  Rssi[13] = ((Rssi[3] + Rssi[7]) / 2);  //  Middle, SOUTH WEST


  // Loop through the array and check for the highest value of the core readings.

  for (int i = 0; i <= 4; i++) {

    // Check if the current array item is higher than the buffer item.
    if (Rssi[i] > MaxRssiCOREVal) {
      MaxRssiCOREVal = Rssi[i];
      MaxRssiCOREArr = i;
    }
  }

  for (int i = 5; i <= 7; i++) {

    // Check if the current array item is higher than the buffer item.
    if (Rssi[i] > MaxRssiSECVal) {
      MaxRssiSECVal = Rssi[i];
      MaxRssiSECArr = i;
    }
  }

  for (int i = 8; i <= 10; i++) {

    // Check if the current array item is higher than the buffer item.
    if (Rssi[i] > MaxRssiTHIVal) {
      MaxRssiTHIVal = Rssi[i];
      MaxRssiTHIArr = i;
    }
  }

  // Find the max value for RSSI from the 5 recievers

  for (int i = 11; i <= 13; i++) {

    // Check if the current array item is higher than the buffer item.
    if (Rssi[i] > MaxRssiFOUVal) {
      MaxRssiFOUVal = Rssi[i];
      MaxRssiFOUArr = i;
    }
  }

  // Do some stuff depending on best RSSI
  // Core recievers represent the outer limits of the RSSI therefore a large movement to bring the signal back to center is required.
  // Some code to find the average RSSI of all the recievers in an attempt to work out a rough distance - distance can be used to alter the amount the servos move.
  // it is likely this wont be required as the second, third and fourth average readings should sort this

  if (MaxRssiCOREArr == 0) {
    // LEFT - Move Large Left 3
    VidFeed(0);
    lcd.setCursor(0, 1);
    lcd.print("RSSI L ~ " + (String)Rssi[MaxRssiCOREArr] + "%");
    Lights("LEFT");
    Moveit("X", -20);
    goto QuickExit;
  }
  else if (MaxRssiCOREArr == 1) {
    // RIGHT - Move Large Right 1
    VidFeed(1);
    lcd.setCursor(0, 1);
    lcd.print("RSSI R ~ " + (String)Rssi[MaxRssiCOREArr] + "%");
    Lights("RIGHT");
    Moveit("X", 20);
    goto QuickExit;
  }
  else if (MaxRssiCOREArr == 2) {
    // BOTTOM - Move large bottom 2
    VidFeed(2);
    lcd.setCursor(0, 1);
    lcd.print("RSSI B ~ " + (String)Rssi[MaxRssiCOREArr] + "%");
    Lights("BOTTOM");
    Moveit("Y", -20);
    goto QuickExit;

  }
  else if (MaxRssiCOREArr == 3) {
    // MIDDLE - Just Light LED 4
    VidFeed(4);
    lcd.setCursor(0, 1);
    lcd.print("RSSI M ~ " + (String)Rssi[MaxRssiCOREArr] + "%");
    Lights("MIDDLE");
  }
  else if (MaxRssiCOREArr == 4) {
    // RHCP - Light LED 5
    // Code to continue tracking but dont change the video output
    VidFeed(5);
    lcd.setCursor(0, 1);
    lcd.print("RSSI CP ~ " + (String)Rssi[MaxRssiCOREArr] + "%");
    Lights("RHCP");
  }
  // End of the core recievers


  // Secondary average measurments - these represent a more central reading and therefore less movement is required than core.

  if (MaxRssiSECArr == 5) {
    // NORTH
    lcd.setCursor(0, 1);
    lcd.print("RSSI N ~ " + (String)Rssi[MaxRssiSECArr] + "%");
    Moveit("Y", 5);
  }
  else if (MaxRssiSECArr == 6) {
    // SOUTH EAST
    lcd.print("RSSI SE ~ " + (String)Rssi[MaxRssiSECArr] + "%");
    Moveit("Y", -5);
    Moveit("X", 5);
  }
  else if (MaxRssiSECArr == 7) {
    // SOUTH WEST
    lcd.setCursor(0, 1);
    lcd.print("RSSI SW ~ " + (String)Rssi[MaxRssiSECArr] + "%");
    Moveit("Y", -5);
    Moveit("X", -5);
  }

  // Thertiary measurment - Signal is pretty close to central and therefore minimal movement is required


  if (MaxRssiTHIArr == 8) {
    //  Middle, LEFT Average
    lcd.setCursor(0, 1);
    lcd.print("RSSI ML ~ " + (String)Rssi[MaxRssiTHIArr] + "%");
    Moveit("Y", -2);
    Moveit("X", 2);
  }
  else if (MaxRssiTHIArr == 9) {
    //  Middle, RIGHT Average
    lcd.print("RSSI MR ~ " + (String)Rssi[MaxRssiTHIArr] + "%");
    Moveit("Y", -2);
    Moveit("X", -2);
  }
  else if (MaxRssiTHIArr == 10) {
    //  Middle, BOTTOM Average
    lcd.setCursor(0, 1);
    lcd.print("RSSI MB ~ " + (String)Rssi[MaxRssiTHIArr] + "%");
    Moveit("Y", -2);
  }

  // Final tweak - pretty central and the middle RSSI is strong therefore very minimal movement of 1 degree required

  if (MaxRssiFOUArr == 11) {
    //  Middle, North Average
    lcd.setCursor(0, 1);
    lcd.print("RSSI MN ~ " + (String)Rssi[MaxRssiFOUArr] + "%");
    Moveit("Y", 1);
  }
  else if (MaxRssiFOUArr == 12) {
    //  Middle, South East Average
    lcd.setCursor(0, 1);
    lcd.print("RSSI MSE~ " + (String)Rssi[MaxRssiFOUArr] + "%");
    Moveit("Y", -1);
    Moveit("X", 1);
  }
  else if (MaxRssiFOUArr == 13) {
    //  Middle, South West Average
    lcd.setCursor(0, 1);
    lcd.print("RSSI MSW~ " + (String)Rssi[MaxRssiFOUArr] + "%");
    Moveit("Y", -1);
    Moveit("X", -1);
  }


QuickExit:

  delay(1000);        // delay between reads for stability - Can probably be decreased to improve tracking accuracy and speed at some point.
}







String padding(String str, int padlength) {
  String T;
  T = str;
  for (int x = str.length(); x < padlength + 1; x++) {
    T = T + " ";
  }
  return T;
}
