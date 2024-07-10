#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <String.h>
#include <Sim800l.h> 

Sim800l Sim800l;
SoftwareSerial gpsSerial(6, 5);  // RX, TX
TinyGPSPlus gps;
RF24 radio(7, 8); // CE, CSN
const byte diachi[6] = "12345";
SoftwareSerial mySerial(3, 2); // CHÂN TX NỐI VỚI CHÂN 3, CHÂN RX NỐI VỚI CHÂN 4
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup(){
  pinMode(4, OUTPUT);
  gpsSerial.begin(9600); //set serial gps
  mySerial.begin(9600); // set serial sim
  Serial.begin(9600);   // set serial main
  // set LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("demo");
  //set_rf
  Serial.begin(9600);
  if (!radio.begin()){
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }
  radio.openReadingPipe(1, diachi); //set_RF
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  if ( !radio.available() ){
    Serial.println("Chưa kết nối được với TX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
    } 
}
float kinh_do = 0;
float vi_do = 0;
int trang_thai=0;
int times=0;
String textSms, numberSms;
void loop()
{ trang_thai=trang_thai%2;
  get_gps();
  set_phone();
  send_gps();
  if (radio.available()){
    char nhan[30] = ""; //30 là số kí tự trong mảng
    radio.read(&nhan, sizeof(nhan));
    if (String(nhan) == "sos" && trang_thai==0){
        times=millis();
        lcd.setCursor(0, 1);
        lcd.print("waring");
        delay(2000);
        lcd.setCursor(0, 1);
        lcd.print("get GPS   ");
        digitalWrite(4,trang_thai++);
        delay(500);
        SendSMS(kinh_do, vi_do);
      }
     if((millis()-times)==300000 && trang_thai==1)digitalWrite(4,trang_thai++);
    }
  }


void get_gps() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      kinh_do = gps.location.lat();
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      vi_do = gps.location.lng();
      Serial.print("Altitude (meters): ");
      Serial.println(gps.altitude.meters());
      Serial.print("Speed (km/h): ");
      Serial.println(gps.speed.kmph());
      Serial.println();
    }
  }
}
void SendSMS(float latitude, float longitude  )
{ lcd.setCursor(0, 1);
  lcd.print("sen sms   ");
  delay (1000);
  mySerial.print("AT\r");
  delay(1000);
  char inchar = (char)Serial.read();
  mySerial.println();
  mySerial.println("AT+CMGF=1\r"); // sets the SMS mode to text
  delay(1200);
  mySerial.print("AT+CMGS=\""); // send the SMS number
  mySerial.print(numberSms);
  mySerial.println("\"");
  delay(1000);
  mySerial.print("Location:"); // noi dung SMS
  mySerial.print("www.google.com.vn/maps/place/");
  mySerial.print(latitude, 5);
  mySerial.print(",");
  mySerial.print(longitude, 5);
  mySerial.print("\r");
  mySerial.print("Em say roi \r\n");
  mySerial.print("Don em voi! please...");
  delay(500);
  mySerial.println((char)26);//o código ASCII do ctrl + z é 26
  delay(500);
  mySerial.write(0x0D);
  mySerial.write(0x0A);
  delay(15000);
}
void set_phone(){
  textSms = Sim800l.readSms(1);
  textSms.toUpperCase();
  if (textSms.indexOf("GET_SDT")!=-1)numberSms=Sim800l.getNumberSms(1);
  Sim800l.delAllSms();
  }
void send_gps(){
  textSms = Sim800l.readSms(1);
  textSms.toUpperCase();
  if (textSms.indexOf("GET_GPS")!=-1)SendSMS(kinh_do, vi_do);
  Sim800l.delAllSms();
  }
