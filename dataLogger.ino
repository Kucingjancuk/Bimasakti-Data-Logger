/*
Program ini dibuat untuk Arduino UNO + Ethernet Arduino Shield (ada build in SD Slot)
Ethernet Arduino Shield punya konfigurasi SPI pada pin 11, 12, dan 13 untuk MOSI, MISO, dan SCK dan pin 4 untuk CS (chip select)

Sensor suhu hasil pembacaannya diinput ke pin A0, A1, dan A2 pada arduino. 

Hasil pembacaan juga ditampilkan pada serial monitor dan LCD 2x16

oleh: Jonathan, Elins 2014
Sumber referensi dari arduino.cc
*/

#include <SPI.h>                                          //untuk komunikasi SPI ke SD Card
#include <SD.h>                                           //library arduino untuk menggunakan SD Card
#include <LiquidCrystal.h>                                //Penampilan data realtime

#define chipSelect 4                                      //pin yang terhubung pin CS pada SD card yang dipilih
#define ARef_LM35 3.3
LiquidCrystal lcd(9, 8, 7, 6, 5, 3);
unsigned long counter;
unsigned long menit;

void setup() {
  Serial.begin(9600);                                     //Memulai serial monitor sesuai baudrate ATMega16U2 (programmer arduino)
  lcd.begin(16,2);
  lcd.print("Temp. Monitor"); 
  lcd.setCursor(0,1); 
  lcd.print("Proto Bimasakti"); 
  delay(2000);

  Serial.println("Mencari SD Card terkoneksi...");        //Instalasi SD Card dari pin SPI dan CS, 
  lcd.clear(); 
  lcd.print("Search SD Card.."); 
  lcd.setCursor(0,1);
  
  if (!SD.begin(chipSelect)) {                          
    Serial.println("SD Card tidak ditemukan");        
    lcd.print("No SD Card");
    return;                                               //Jika SD Card tidak ada, maka pesan ini muncul
  }
  else{
    Serial.print("SD Card terdeteksi");                   //Jika SD Card ditemukan, maka pesan ini muncul
    lcd.print("SD Card is ready"); delay(2000); 
    lcd.clear(); lcd.print("Temp. Monitor");
  }
}

void loop()
{
  //--------------Bagian penulisan sepaket data suhu (pada variabel dataString)-----------------------------------------------------

  String dataString = "";                                 //variabel untuk mengumpulkan data hasil sekali loop 3 sensor suhu
                                                          //pada loop berikutnya akan ditimpa dengan data baru
  bool dangerStat=0;
                                                      
  for (int anPin = 0; anPin < 3; anPin++) {               //menggilir pembacaan sensor
    switch (anPin){
      case 0: dataString += "P: ";  break;                 //P: Pendingin Mesin, O: Oli, R: Ruang Bakar
      case 1: dataString += " | O: "; break;
      case 2: dataString += " | R: "; break;
    }
    int suhu = analogRead(anPin);                         //inisiasi data mentah dari LM35
    suhu = ((suhu*ARef_LM35/1024) - 0.5) * 100;           //konversi data mentah LM35 ke satuan Celcius
    if (suhu>200) dangerStat=1;
    if (suhu>=0 && suhu<10) dataString += String("  ");
    else if ((suhu>(-10) && suhu<0) || (suhu>=10 && suhu<100)) dataString += String(" ");
    dataString += String(suhu);                           //input nilai ke variabel dataString yang mengumpulkan data semua sensor 
  }
  if(dangerStat) dataString += String("  !!!  ");        //peringatan bahaya jika suhu > 200 derajat Celcius
  else dataString += String("  ---  ");

  //--------------Penampilan data di LCD------------------------------------------------------------------------------------------

  lcd.setCursor(0,1); lcd.print(dataString);              //menampilkan dataString di LCD


  //--------------Penyimpanan data di SD Card-------------------------------------------------------------------------------------

  File dataFile = SD.open("datalog.txt", FILE_WRITE);     //membuka file datalog.txt yang menyimpan logging dari semua data suhu
  
//  if (dataFile) {
    dataFile.print(dataString); dataFile.print(" ");      //input 1 kali pembacaan ketiga sensor
    
    counter++;
    if(counter%5==0){
      dataFile.print(" "); dataFile.println(counter/5);   //setiap 5 pembacaan, data ganti baris
      Serial.print(" "); Serial.println(counter/5);
    }

    if(millis()>(menit*60000)){
      dataFile.print("\n\n==Menit ke "); 
      dataFile.println(menit);
      Serial.print("\n\n==Menit ke "); 
      Serial.println(menit);
      menit++; counter=0;
    }
    
    dataFile.close();                                     //menutup file
    
    Serial.print(dataString);                             //menampilkan pada serial monitor    
//  }
//  else {
//    Serial.println("File \"datalog.txt\" tidak ditemukan");  //pesan error jika file tidak ditemukan
//  }

  delay(1000);                                            //delay 1 detik supaya data tidak terlalu banyak

}

