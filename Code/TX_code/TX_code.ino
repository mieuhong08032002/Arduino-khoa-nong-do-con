

//nrf
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(8, 9); // CE, CSN
const byte diachi[6] = "12345"; //Mảng kí tự dạng chuỗi có 6 kí tự
///mq03
#define MQ03 4
#define acl 1000

void setup()
{ 
  //nrf
  Serial.begin(9600);

  if (!radio.begin())
  {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  }

  radio.openWritingPipe(diachi);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(80);
  radio.setDataRate(RF24_250KBPS);
  radio.stopListening(); //Cài đặt module là TX

  if (!radio.available())
  {
    Serial.println("Chưa kết nối được với RX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  }
 //mq03
 pinMode(MQ03,INPUT);
}

void loop() {
//int  value=analogRead(MQ03);
//delay(1000);
 //Serial.println(value);
if (digitalRead(4)==0) {
    const char text[] = "sos"; //Mảng chứa chuỗi kí tự
    radio.write(&text, sizeof(text));
    Serial.println("đã gửi");
//    delay(10000);
//    const char text2[] = "no"; //Mảng chứa chuỗi kí tự
//    radio.write(&text2, sizeof(text));
//    Serial.println("đã gửi");
    delay(1000);
    while(digitalRead(4)==0);
 }
}
