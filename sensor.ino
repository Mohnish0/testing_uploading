#include <LiquidCrystal.h>
LiquidCrystal lcd(23, 22, 21, 19, 18, 5);  // rs, en, d4, d5, d6, d7
#include <ESP32_Servo.h>


#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>

#define SSID "ADMIN"
#define PASS "admin1234"
#define ID 25

#define SERVER "http://computersshiksha.info/student-projects/"
HTTPClient http;
WiFiClient wifiClient;


Servo servo_ent, servo_ext;
void display3val(int d) {
  unsigned int temparray[3], temp;
  for (temp = 3; temp > 0; temp--) {
    temparray[temp - 1] = d % 10;
    d = d / 10;
  }

  for (temp = 0; temp < 3; temp++) {
    lcd.write(temparray[temp] + 48);
  }
}

#define SERVO_ENT 4
#define SERVO_EXT 15

int s1 = 27, s2 = 14, s3 = 34, s4 = 13;

int Sn[4]={27,14,34,13};
int Ld[4] ={25,33,2,26};

int stable_ang = 180, open_ang = 0, close_ang = 180;
bool save_ent = false;
bool save_ext = false;



void setup() {
  lcd.begin(20, 4);
  Serial.begin(9600);
  lcd.setCursor(0, 0);
  lcd.print("IOT AND ANDROID APP ");
  lcd.setCursor(0, 1);
  lcd.print("BASED SECURE CAR ");
  lcd.setCursor(0, 2);
  lcd.print("PARKING SYSTEM");
  // initWiFi();
  delay(3000);
  pinMode(s1, INPUT_PULLUP);
  pinMode(s2, INPUT_PULLUP);
  pinMode(s3, INPUT_PULLUP);
  pinMode(s4, INPUT_PULLUP);

  pinMode(2, OUTPUT);  
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(33, OUTPUT);

  digitalWrite(25,LOW); //2
  digitalWrite(33,LOW); //1
  digitalWrite(26,LOW); //4
  digitalWrite(2,LOW); //3

  
  servo_ent.attach(SERVO_ENT);
  servo_ent.write(stable_ang);
  servo_ext.attach(SERVO_EXT);
  servo_ext.write(stable_ang-180);

  lcd.clear();
  Lprint(0,0,0,0); 

  lcd.setCursor(0, 2);
  lcd.print("ENTRY GATE: CLOSE  ");
  
  lcd.setCursor(0, 3);
  lcd.print("EXIT GATE: CLOSE  ");
}

int Val1 = 0,Val2=0;
int v1 = 0, v2 = 0, v3 = 0, v4 = 0,Vs=0;
void loop() {
 
  
  v1 = 1-digitalRead(s1);
  v2 = 1-digitalRead(s2);
  v3 = 1-digitalRead(s3);  
  v4 = 1-digitalRead(s4);
    
  // Serial.print(v1);
  // Serial.print(v2);
  // Serial.print(v3);
  // Serial.println(v4);
  Lprint(v1,v2,v3,v4); 
  get_from_server();  
  delay(500);
}


void servo_entry(bool action, int Opt) {
  if (action) {
    if (!servo_ent.attached()) {
      servo_ent.attach(SERVO_ENT);
    }
    if (Opt == 1) {
      servo_ent.write(open_ang);
      delay(200);
    }
    if (Opt == 2) {
      servo_ent.write(close_ang);
      delay(200);
    }
    save_ent = true;
  } else if (save_ent) {
    if (!servo_ent.attached()) {
      servo_ent.attach(SERVO_ENT);
    }
    servo_ent.write(stable_ang);
    delay(200);
    servo_ent.detach();
    save_ent = false;
  }
}


void servo_exit(bool action, int Opt) {
  if (action) {
    if (!servo_ext.attached()) {
      servo_ext.attach(SERVO_EXT);
    }
    if (Opt == 1) {
      servo_ext.write(open_ang+180);
      delay(200);
    }
    if (Opt == 2) {
      servo_ext.write(close_ang-180);
      delay(200);
    }
    save_ext = true;
  } else if (save_ext) {
    if (!servo_ext.attached()) {
      servo_ext.attach(SERVO_EXT);
    }
    servo_ext.write(stable_ang);
    delay(200);
    servo_ext.detach();
    save_ext = false;
  }
}


void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASS);
  unsigned long s_time = millis();
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    if (millis() - s_time > 5000)
      break;
    delay(500);
  }
  Serial.print("WiFi Connected ");
  Serial.println(WiFi.localIP());
}

void get_from_server() {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(SERVER) + "iot/get_values.php?id=" + String(ID);
    // Serial.println(url);
    http.begin(wifiClient, url);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      JSONVar json_object = JSON.parse(payload);
      if (JSON.typeof(json_object) == "undefined") {
        Serial.println("Parsing input failed!");
      }
      // Serial.println(json_object);
       
      char marray[5];
      for (int g1=1;g1<5;g1++){ 
        strcpy(marray, json_object["field"+String(g1)]);
        Val1 = stoi(marray);
        strcpy(marray, json_object["field"+String(g1+6)]);
        Val2 = stoi(marray);
        Vs=1-digitalRead(Sn[g1-1]);  
        if (Val1==2){
          digitalWrite(Ld[g1-1],HIGH);
        }else{
          digitalWrite(Ld[g1-1],LOW);
        }
        
        if ((Val1==2) && (Val2==0) && (Vs==1)){          
            send_to_server1("field"+String(g1+6),String(Vs));            
        }
        
      else if((Val1==2) && (Val2==1) && (Vs==0)){
         send_to_server1("field"+String(g1+6),String(Vs)); 
         send_to_server1("field"+String(g1),String(Vs));
      }
               
        if ((Val1==0) || (Val1==1) )
        {
         send_to_server1("field"+String(g1),String(Vs));             
        }   
      }        
            

      
          
      if ((v1==0) || (v2==0) || (v3==0) || (v4==0)){
        strcpy(marray, json_object["field5"]);
        Val1 = stoi(marray);
        if (Val1 == 31) {
          servo_entry(true, 1);
          lcd.setCursor(0, 2);
          lcd.print("ENTRY GATE: OPEN  ");
          send_to_server1("field5","C") ;
          delay(2000); 
          servo_entry(true, 2);
          lcd.setCursor(0, 2);
          lcd.print("ENTRY GATE: CLOSE");
          Val1=0;
        }
      } 
      else{
        send_to_server1("field5","C") ;
      }

      

      strcpy(marray, json_object["field6"]);
      Val2 = stoi(marray);
      if (Val2 == 31) {
        servo_exit(true, 1);
        lcd.setCursor(0, 3);
        lcd.print("EXIT GATE: OPEN  ");
        send_to_server1("field6","C")    ;           
        delay(2000);         
        servo_exit(true, 2);
        lcd.setCursor(0, 3);
        lcd.print("EXIT GATE: CLOSE");
        Val2=0;
      }

      
      
      // Serial.println(Val);
      http.end();
    } else {
      // Serial.println(code);
    }
    http.end();
  } else {
    initWiFi();
  }
}


int stoi(char *string) {
  int num = 0;
  for (int i = 0; string[i] != '\0'; i++) {
    num = num * 10 + (string[i] - 48);
  }
  return num;
}

void send_to_server(String field1, String field2, String field3, String field4) {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(SERVER) + "iot/set_values.php?id=" + String(ID) + "&field1=" + field1 + "&field2=" + field2 + "&field3=" + field3 + "&field4=" + field4;
    http.begin(wifiClient, url);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      // Serial.println(payload);
      http.end();
    } else {
      // Serial.println(code);
    }
    http.end();
  } else {
    initWiFi();
  }
}

void send_to_server1(String fieldn,String field1) {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(SERVER) + "iot/set_values.php?id=" + String(ID) + "&" + fieldn + "=" + field1 ;
    http.begin(wifiClient, url);
    int code = http.GET();
    if (code == 200) {
      String payload = http.getString();
      // Serial.println(payload);
      http.end();
    } else {
      // Serial.println(code);
    }
    http.end();
  } else {
    initWiFi();
  }
}

void Lprint(int v1, int v2, int v3, int v4) {

  lcd.setCursor(0, 0);
  lcd.print("S1:");
  if (v1 == 0) {
    lcd.setCursor(3, 0);
    lcd.print("EMPTY ");
  } else if (v1 == 2) {
    lcd.setCursor(3, 0);
    lcd.print("BOOKED");
  } else {
    lcd.setCursor(3, 0);
    lcd.print("PARKED");
  }



  lcd.setCursor(10, 0);
  lcd.print("S2:");
  if (v2 == 0) {
    lcd.setCursor(13, 0);
    lcd.print("EMPTY ");
  } else if (v2 == 2) {
    lcd.setCursor(3, 0);
    lcd.print("BOOKED");
  } else {
    lcd.setCursor(13, 0);
    lcd.print("PARKED");
  }



  lcd.setCursor(0, 1);
  lcd.print("S3:");
  if (v3 == 0) {
    lcd.setCursor(3, 1);
    lcd.print("EMPTY ");
  } else if (v3 == 2) {
    lcd.setCursor(3, 1);
    lcd.print("BOOKED");
  } else {
    lcd.setCursor(3, 1);
    lcd.print("PARKED");
  }



  lcd.setCursor(10, 1);
  lcd.print("S4:");
  if (v4 == 0) {
    lcd.setCursor(13, 1);
    lcd.print("EMPTY ");
  } else if (v4 == 2) {
    lcd.setCursor(3, 1);
    lcd.print("BOOKED");
  } else {
    lcd.setCursor(13, 1);
    lcd.print("PARKED");
  }
}