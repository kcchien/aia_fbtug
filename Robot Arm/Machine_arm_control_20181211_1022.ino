
// 狀態LED 
const int ctrl_LED = 8; // Low動作
// motor 正轉
const int m1P_clamp_close = 7; 
const int m2P = 6;
const int m3P = 5;
const int m4P = 4;
const int m5P_left = 3;
// motor 反轉
const int m1N_clamp_open = 13; 
const int m2N = 12;
const int m3N = 11;
const int m4N = 10;
const int m5N_right = 9;

const int elbow_factor=1300; // 0 + 1300 = 1
const int wrist_factor=600;
const int left_factor = 850;
// test parameter 
const int para = 1000;

  const int x_max = 800;
  const int y_max = 600;
  const int x_mid = x_max/2;
  const int y_mid = y_max/2;

  const int right_max = 3500 ; 
  const int left_max = 7500;
  
void setup() {
  // put your setup code here, to run once:

   Serial.begin(9600);      // open the serial port at 9600 bps:
  pinMode(ctrl_LED, OUTPUT);
  // 正轉馬達控制
  pinMode(m1P_clamp_close, OUTPUT);
  pinMode(m2P, OUTPUT);
  pinMode(m3P, OUTPUT);
  pinMode(m4P, OUTPUT);
  pinMode(m5P_left, OUTPUT);
  // 反轉馬達控制
  pinMode(m1N_clamp_open, OUTPUT);
  pinMode(m2N, OUTPUT);
  pinMode(m3N, OUTPUT);
  pinMode(m4N, OUTPUT);
  pinMode(m5N_right, OUTPUT);
           
  //點亮LED
  digitalWrite(ctrl_LED, LOW);


  move_command(1, 33); //第二個參數非0, 非1, 設置為LOW
  move_command(2, 33); //第二個參數非0, 非1
  move_command(3, 33); //第二個參數非0, 非1
  move_command(4, 33); //第二個參數非0, 非1
  move_command(5, 33); //第二個參數非0, 非1

}
char temp_chr[10];
bool key_ctrl=0;

              
void loop() {
  
    detect();
    
   // Serial.println(detect());
    if(temp_chr[0] == 'A' && temp_chr[1] == 'I' && temp_chr[2] == 'A' ){  //如果接受到AIA
        Serial.println("-----Start command confirmed-----");
        key_ctrl=1;
        //move_right();
        reset_array(temp_chr);
      }else{}

    if(key_ctrl){
//      Serial.println("");
//      delay(500);
        if(temp_chr[0] == 'o' && temp_chr[1] == 'v' && temp_chr[2] == 'e' && temp_chr[3] == 'r'){ // input == over
            key_ctrl=0;
            Serial.println("-----Over command confirmed-----");
          }

        if(temp_chr[0] == 'a' && temp_chr[1] == 'u' && temp_chr[2] == 't' && temp_chr[3] == 'o'){
            bool door=0;

             reset_array(temp_chr);
             detect();
            Serial.println("auto mode\n---------------");
                  int x_value;
                  int y_value;
            while(!door){

              detect();
              delay(50);
            //==============================================================================
              if( temp_chr[0] == 'o' && temp_chr[1] == 'u' && temp_chr[2] == 't'){
                door=1;
              }
            //===============================================================================
              if( (temp_chr[0] == '1') || (temp_chr[0] == '2')  || (temp_chr[0] == '3')  || (temp_chr[0] == '4')  || (temp_chr[0] == '5')  || (temp_chr[0] == '6') \
              || (temp_chr[0] == '7') || (temp_chr[0] == '8')  || (temp_chr[0] == '9')  ){
                  int var;

                  var = atoi(temp_chr);

                   //Serial.println(var/10);
                   //Serial.print("math");
                  if(x_value == 0){
                    x_value = var;
                    Serial.print("X: ");
                    Serial.println(x_value);
                    }
                   else if (y_value == 0){
                    y_value = var;
                    Serial.print("Y: ");
                    Serial.println(y_value);
                    }else{}
                } 
                if( (x_value != 0 )&& (y_value!= 0)){
                  //------------ move on 
                  auto_move(x_value, y_value);
                  //------------------------ End
                  x_value = 0 ;
                  y_value = 0 ; 
                  }

            //-------------------------
              reset_array(temp_chr);
            }//--------while door
            door=0;
          }
        if(temp_chr[0] == 'r' && temp_chr[1] == 'e' && temp_chr[2] == 's'){
            reset_array(temp_chr);
            Serial.println("reset over");
          }
        if(temp_chr[0] == 'a' && temp_chr[1] == 'c' && temp_chr[2] == 't' && temp_chr[3] == '1'){
            move_left(left_max);
          }
        if(temp_chr[0] == 'a' && temp_chr[1] == 'c' && temp_chr[2] == 't' && temp_chr[3] == '2'){
          
              move_elbow(0, (1500+elbow_factor));
              move_elbow(1, 1500);
            
          }
        if(temp_chr[0] == 'a' && temp_chr[1] == 'c' && temp_chr[2] == 't' && temp_chr[3] == '3'){
              move_right(right_max);
          }
        if(temp_chr[0] == 'a' && temp_chr[1] == 'c' && temp_chr[2] == 't' && temp_chr[3] == '4'){
            //move_shoulder(1, 1500);

                          int i;
            for(i=0; i<2; i++){
              move_wrist(0, 1500+wrist_factor);
              move_wrist(1, 1500);
            }

          }
        if(temp_chr[0] == 'a' && temp_chr[1] == 'c' && temp_chr[2] == 't' && temp_chr[3] == '5'){
              move_right(2000);
              move_elbow(0, 2000);
              move_elbow(1,2000);
              move_left(2000);
              //move_clamp(1);
          }
        else {Serial.println("...");}
      }









       reset_array(temp_chr);
  delay(1000);
}

//==================================================   函式區   ================================================================

void auto_move(int x_value, int y_value){
  if ( x_value < x_mid){
   // float fac = x_value / x_mid;
    move_left(left_max);
    }
  if ( x_value > x_mid ){
   // float fac = x_value / x_mid;
    move_right(right_max );
    }
  }
  

void reset_array(char arr[10]){ //清除 array
    int i;
      for(int i=0; i<9; i++){
        arr[i]="";
      }
  }

void special(){
    move_command(4, 1);delay(1500);move_command(4, 33);
    move_command(3, 0);delay(1500);move_command(3, 33);
    move_command(2, 0);
    delay(1500);move_command(2, 33);
    move_command(4, 0);delay(1500);move_command(4, 33);
    move_command(3, 1);delay(1500);move_command(3, 33);
    move_command(2, 1);
    delay(1500);move_command(2, 33);

  }


//===================================================================   機械手控制函式區   ===========================================================================

// clamp, wrist, elbow, shoulder, right, left

//====================================================================================
//-----------------------------------------------------------------------------------
void move_shoulder(int way, int value){  //這個負載很重, 很容易up不起來
    if(way == 0){
      Serial.println("move shoulder up\n");  // 0 for close, 1 for open 
      move_command(4, 0); // 
      delay(value);
      move_command(4, 33); // stop
      Serial.println("move shoulder up complete\n");
      }
    if(way == 1){
      Serial.println("move shoulder down\n");  // 0 for close, 1 for open 
      move_command(4, 1); // 
      delay(value);
      move_command(4, 33); // stop
      Serial.println("move shoulder down complete");
      }
  
  }

//-----------------------------------------------------------------------------------
//====================================================================================

//====================================================================================
//-----------------------------------------------------------------------------------
void move_elbow(int way, int value){
    if(way == 0){
      Serial.println("move elbow up\n");  // 0 for close, 1 for open 
      move_command(3, 0); // clamp close 
      delay(value+elbow_factor);
      move_command(3, 33); // stop
      Serial.println("move elbow up complete\n");
      }
    if(way == 1){
      Serial.println("move elbow down\n");  // 0 for close, 1 for open 
      move_command(3, 1); // clamp open
      delay(value);
      move_command(3, 33); // stop
      Serial.println("move elbow down complete");
      }
  
  }

//-----------------------------------------------------------------------------------
//====================================================================================

//====================================================================================
//-----------------------------------------------------------------------------------
void move_wrist(int way, int value){
    if(way == 0){
      Serial.println("move wrist up\n");  // 0 for close, 1 for open 
      move_command(2, 0); // clamp close 
      delay(value);
      move_command(2, 33); // stop
      Serial.println("move wrist up complete\n");
      }
    if(way == 1){
      Serial.println("move wrist down\n");  // 0 for close, 1 for open 
      move_command(2, 1); // clamp open
      delay(value+wrist_factor);
      move_command(2, 33); // stop
      Serial.println("move wrist down complete");
      }
  
  }

//-----------------------------------------------------------------------------------
//====================================================================================
  

//====================================================================================
//-----------------------------------------------------------------------------------
void move_clamp(int way){
    if(way == 0){
      Serial.println("move clamp close\n");  // 0 for close, 1 for open 
      move_command(1, 0); // clamp close 
      delay(3000);
      move_command(1, 33); // stop
      Serial.println("move clamp close complete\n");
      }
    if(way == 1){
      Serial.println("move clamp open\n");  // 0 for close, 1 for open 
      move_command(1, 1); // clamp open
      delay(2000);
      move_command(1, 33); // stop
      Serial.println("move clamp open complete\n");
      }
  
  }
//-----------------------------------------------------------------------------------
//====================================================================================


//====================================================================================
//-----------------------------------------------------------------------------------
void move_right(int value){
    Serial.println("move right start\n");
 
    move_command(5, 1); //turn right 
    delay(value);
    move_command(5, 33);
    Serial.println("move right complete\n");
  }

void move_left(int value){
    Serial.println("move left start\n");
    move_command(5, 0); //turn left
    delay(value+ left_factor);
    move_command(5, 33);
    Serial.println("move left complete\n");
  }
//-----------------------------------------------------------------------------------
//====================================================================================
//                     下方為基礎控制函式區
//-----------------------------------------------------------------------------------
//====================================================================================
void move_command(int number, int next_state){

// ==================  m1  =======================

    if (number == 1){
      digitalWrite(m1P_clamp_close, LOW);
      digitalWrite(m1N_clamp_open, LOW);
      
      if(next_state == 0){
        digitalWrite(m1P_clamp_close, HIGH);
        }else if(next_state == 1){
        digitalWrite(m1N_clamp_open, HIGH);
        }else{}
      }
// ==================  m2  =======================
    if (number == 2){
      digitalWrite(m2P, LOW);
      digitalWrite(m2N, LOW);
      
      if(next_state == 0){
        digitalWrite(m2P, HIGH);
        }else if(next_state == 1){
        digitalWrite(m2N, HIGH);
        }else{} 
      }

// ==================  m3  =======================
    if (number == 3){
      digitalWrite(m3P, LOW);
      digitalWrite(m3N, LOW);
      
      if(next_state == 0){
        digitalWrite(m3P, HIGH);
        }else if(next_state == 1){
        digitalWrite(m3N, HIGH);
        }else{}
      }

// ==================  m4  =======================
   if (number == 4){
      digitalWrite(m4P, LOW);
      digitalWrite(m4N, LOW);
      
      if(next_state == 0){
        digitalWrite(m4P, HIGH);
        }else if(next_state == 1){
        digitalWrite(m4N, HIGH);
        }else{}
      }

// ==================  m5  =======================
   if (number == 5){
      digitalWrite(m5P_left, LOW);
      digitalWrite(m5N_right, LOW);
      
      if(next_state == 0){
        digitalWrite(m5P_left, HIGH);
        }else if(next_state == 1){
        digitalWrite(m5N_right, HIGH);
        }else{}
      }

   else{}

  }

bool detect(){
  if(Serial.available()){
      int i=0;
      char chr;
      while(Serial.available()){
        chr = Serial.read();
        temp_chr[i]=chr;
        i++;
        //Serial.println(chr);  // 這邊的chr就是我們輸入的值
      }//Serial.println("-------------");
      Serial.println(temp_chr);
      // 清除array 
      //reset_array(temp_chr);
      return 1;
      
    }else{return 0;}
  }
