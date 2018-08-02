void sd_init(){                     //init for the sd card
  if (!SD.begin(10)) {
    Serial.println("init failed!");
    return;
  }
  Serial.println("init oke");
}

String waterpoolRead(){           //save data to sdcard
  String waterRead="";            //make file for the data to be saved
  myFile= SD.open("test.txt");    //open the file
  if(myFile){                     //if file exist
    while(myFile.available()){    //as long as the data is exist
     char c = myFile.read();      //read every byte and save it to 'c'
     waterRead += c;              //combine every char read and saved it to 'waterRead'
    }
    Serial.print("READ: ");       //print data read to serial monitor
    Serial.println(waterRead);
    myFile.close();               //close the file
  }
  else{
    Serial.println("Open read failed"); //if file doesnt exist
  }
  return waterRead;               //return the waterread value
}

void waterpoolWrite(int waterpool){           //write data to sdcard
  myFile = SD.open("test.txt", FILE_WRITE);   //open or create if the file doesnt exist
  if(myFile){                                 //if file can be opened
    myFile.println(waterpool);                //print waterpool into sdcard
    Serial.print("WRITE: ");                  //print waterpool to serial monitor
    Serial.println(waterpool);
    myFile.close();                           //file closed
  }
  else{
    Serial.println("Open write failed");      //if open file fail
  }
}

