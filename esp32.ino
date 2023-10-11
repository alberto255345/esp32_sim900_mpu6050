#include <SoftwareSerial.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

unsigned long currentmillis;
unsigned long previousmillis;
const unsigned long interval = 5000;

SoftwareSerial SIM900(14, 12); // configure software serial port

Adafruit_MPU6050 mpu;

// Define your APN, USER, and PWD values here
const char* APN = "zap.vivo.com.br";
const char* USER = "vivo";
const char* PWD = "vivo";

void setup() {     
  Wire.begin();  
  SIM900.begin(19200);
  Serial.begin(19200); 
  while (!Serial)
    delay(10);
  Serial.println("power up" );
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }


  if (!SIM900.available()){
    Serial.println("SIM900 failed");
    while(!SIM900.available()){
      delay(500);
      SIM900.println("AT+CSQ"); // Signal quality check
      delay(500);
      Serial.write(char (SIM900.read()));
    }
  }
  
  Serial.println("SIM900 ok");
  Serial.println("");
  delay(5000);

}

 
void loop()
{
  Serial.println("loop init");
  currentmillis = millis();
  delay(200);
  Serial.println("");
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");

  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");

  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");

  Serial.println("");
  delay(500);

  // String jsonData = "{";
  // jsonData += "\"acceleration_x\":" + String(a.acceleration.x) + ",";
  // jsonData += "\"acceleration_y\":" + String(a.acceleration.y) + ",";
  // jsonData += "\"acceleration_z\":" + String(a.acceleration.z) + ",";
  // jsonData += "\"gyro_x\":" + String(g.gyro.x) + ",";
  // jsonData += "\"gyro_y\":" + String(g.gyro.y) + ",";
  // jsonData += "\"gyro_z\":" + String(g.gyro.z) + ",";
  // jsonData += "\"temperature\":" + String(temp.temperature);
  // jsonData += "}";
  String jsonData = "acceleration_x=" + String((int)a.acceleration.x) + "&";
  jsonData += "acceleration_y=" + String((int)a.acceleration.y) + "&";
  jsonData += "acceleration_z=" + String((int)a.acceleration.z) + "&";
  jsonData += "gyro_x=" + String((int)g.gyro.x) + "&";
  jsonData += "gyro_y=" + String((int)g.gyro.y) + "&";
  jsonData += "gyro_z=" + String((int)g.gyro.z) + "&";
  jsonData += "temperature=" + String((int)temp.temperature);

  if (SIM900.available())
  {
    if ( currentmillis - previousmillis > interval )
    {
      previousmillis = currentmillis;
      Serial.println("SIM 900 is ready");
      Serial.println("SubmitHttpRequest - started" );
      SubmitHttpRequest(jsonData);
      Serial.println("SubmitHttpRequest - finished" );
      delay(5000);  
    }
  }
  else
  {
    Serial.println("SIM 900 is not ready");
    if ( currentmillis - previousmillis > interval )
    {
      previousmillis = currentmillis;
      delay(500);
      SIM900.println("AT+CSQ"); // Signal quality check
      delay(500);
      Serial.write(char (SIM900.read()));
    }
  }

}

void StartProcesso()
{
  SIM900.println("AT+CSQ"); // Signal quality check

  delay(500);
 
  ShowSerialData();// this code is to show the data from gprs shield, in order to easily see the process of how the gprs shield submit a http request, and the following is for this purpose too.
  
  SIM900.println("AT+CGREG?"); //   

  delay(500);
 
  ShowSerialData();//

  SIM900.println("AT+CGREG=2"); // 

  delay(500);
 
  ShowSerialData();//

  SIM900.println("AT+CGATT?"); //Attach or Detach from GPRS Support
  delay(500);
 
  ShowSerialData();
  SIM900.println("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");//setting the SAPBR, the connection type is using gprs
  delay(500);
 
  ShowSerialData();
 
  delay(1000);
  SIM900.println("AT+SAPBR=3,1,\"APN\",\"" + String(APN) + "\"");
  
  delay(500);
  SIM900.println("AT+SAPBR=3,1,\"USER\",\"" + String(USER) + "\"");
  
  delay(500);
  SIM900.println("AT+SAPBR=3,1,\"PWD\",\"" + String(PWD) + "\"");
  
  delay(500);
 
  ShowSerialData();
 
  SIM900.println("AT+SAPBR=1,1");//setting the SAPBR
  delay(500);
 
  ShowSerialData();
 
}

void SubmitHttpRequest(String jsonData)
{
  StartProcesso();
  SIM900.println("AT+HTTPINIT"); //init the HTTP request
  delay(500); 
  
  SIM900.println("AT+HTTPPARA=\"CID\",1"); // Configure o contexto HTTP
  delay(1000);
  
  ShowSerialData();

  String urlfinal = "AT+HTTPPARA=\"URL\",\"";
  urlfinal += "http://54.39.90.0:3000/test?";
  urlfinal += jsonData;
  urlfinal += "\"";

  Serial.println(urlfinal);
 
  SIM900.println(urlfinal);// setting the httppara, the second parameter is the website you want to access
  delay(1000);

  // // Configurar o cabeçalho HTTP para um POST
  // SIM900.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  // delay(1000);

  ShowSerialData();

  // // Definir o comprimento dos dados POST
  // SIM900.print("AT+HTTPDATA=");
  // SIM900.println(jsonData.length());
  // delay(1000);

  // ShowSerialData();

  // // Enviar os dados JSON POST
  // SIM900.print(jsonData);
  // delay(1000);

  // ShowSerialData();
 
  SIM900.println("AT+HTTPACTION=0");//submit the request 
  delay(2000);//the delay is very important, the delay time is base on the return from the website, if the return datas are very large, the time required longer.
  //while(!SIM900.available());
 
  ShowSerialData();
 
  SIM900.println("AT+HTTPREAD");// read the data from the website you access
  delay(150);
  ShowSerialData();
 
  SIM900.println("");
  delay(50);
}

 
void ShowSerialData()
{
  while(SIM900.available()!=0)
    Serial.write(char (SIM900.read()));
}
