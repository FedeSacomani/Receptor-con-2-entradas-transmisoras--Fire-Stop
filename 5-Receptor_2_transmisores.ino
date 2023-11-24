#include "DHT.h"
#include <stdlib.h>
#include "Arduino.h"
#define E32_TTL_1W          //It is necessary to define before including the LoRa library, to indicate which module is used
#include "LoRa_E32.h"
#include "SoftwareSerial.h"
#include "LCD_I2C.h"
#include "DefinicionesIOT.h"
#include "config.h"
#include "certificates.h"
#include <PromLokiTransport.h>
#include <PrometheusArduino.h>

PromLokiTransport transport;
PromClient client(transport);
// Create a write request for 2 series.
WriteRequest req(12, 2048);

//TimeSeries definition
//We need to find a way to define the device tag based on the device names and distribution 


TimeSeries ts1(12, "temperature_degrees", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts2(12, "humidity", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts3(12, "velocity", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts4(12, "position", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts5(12, "monoxide", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts6(12, "battery", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");


TimeSeries ts7(12, "temperature_degrees3", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts8(12, "humidity3", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts9(12, "velocity3", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts10(12, "position3", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts11(12, "monoxide3", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");

TimeSeries ts12(12, "battery3", "{job=\"Fire-stop\",host=\"esp32\",device=\"esp32-rep-siria\"}");



//E32 print parameters function
void printParameters(struct Configuration configuration);

//Declaracion LCD
LCD_I2C lcd(0x27, 16, 2);

//Declaracion modulo LoRa
LoRa_E32 e32ttl(&Serial2);

//Variables
char resp [8]={0};
int numero=0;
int device, temperature, humidity, velocity, pos, temperature2, humidity2, velocity2, pos2, temperature3, humidity3, velocity3, pos3;
int diox, diox2, diox3;
int battery = 0;
int battery2 = 0;
int battery3 = 0;
int loopCounter = 0;

int alert = 0;

void setup() 
{
  Serial.begin(9600);

  // Wait 5s for serial connection or continue without it
    // some boards like the esp32 will run whether or not the 
    // serial port is connected, others like the MKR boards will wait
    // for ever if you don't break the loop.
    uint8_t serialTimeout;
    while (!Serial && serialTimeout < 50) {
        delay(100);
        serialTimeout++;
    }
    //LCD initialization
    lcd.begin();
    lcd.backlight();

    // Configure and start the transport layer
    transport.setUseTls(true);
    transport.setCerts(grafanaCert, strlen(grafanaCert));
    transport.setWifiSsid(WIFI_SSID);
    transport.setWifiPass(WIFI_PASSWORD);
    transport.setDebug(Serial);  // Remove this line to disable debug logging of the client.
    
    // it will loop until is able to successfully connect to the Network
    transport.begin();
    lcd.setCursor(0, 0);
    lcd.print("Prometheus transport layer initialized");  

    // Configure the client
    client.setUrl(GC_URL);
    client.setPath((char*)GC_PATH);
    client.setPort(GC_PORT);
    client.setUser(GC_USER);
    client.setPass(GC_PASS);
    client.setDebug(Serial);  // Remove this line to disable debug logging of the client.
    
    if (!client.begin()) {
        Serial.println(client.errmsg);
        // We need to see the best way to handle this situation
        lcd.clear();
        lcd.setCursor(0, 0);
        //lcd.print("ERROR: "+client.errmsg); 
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Prometheus client initialized successfully");

    // Add our TimeSeries to the WriteRequest
    req.addTimeSeries(ts1);
    req.addTimeSeries(ts2);
    req.addTimeSeries(ts3);
    req.addTimeSeries(ts4);
    req.addTimeSeries(ts5);
    req.addTimeSeries(ts6);
    req.addTimeSeries(ts7);
    req.addTimeSeries(ts8);
    req.addTimeSeries(ts9);
    req.addTimeSeries(ts10);
    req.addTimeSeries(ts11);
    req.addTimeSeries(ts12);
    
    req.setDebug(Serial);  // Remove this line to disable debug logging of the write request serialization and compression.

  //Lora module initialization
  e32ttl.begin();
  e32ttl.resetModule();

  ResponseStructContainer c; //Estructura de parametros del dispositivo
  c = e32ttl.getConfiguration(); //Obtencions de configuracion
  Configuration configuration = *(Configuration*) c.data;
  configuration.ADDH = 0x00;  //Direccion High
  configuration.ADDL = 0x00;  //Direccion Low
  configuration.CHAN = 0x17;  //Canal
  
  configuration.OPTION.fixedTransmission = FT_FIXED_TRANSMISSION; //Modo de transmisión
  configuration.OPTION.wirelessWakeupTime = WAKE_UP_250;          //Tiempo de wake up
  configuration.OPTION.fec = FEC_1_ON;                            //Corrección de errores FEC
  configuration.OPTION.transmissionPower = POWER_30;              //Potencia de transmision
  configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;//Modo de funcionamiento
  
  configuration.SPED.airDataRate = AIR_DATA_RATE_001_12;          //Tasa de transmision
  configuration.SPED.uartBaudRate = UART_BPS_9600;                //Velocidad de comunicacion UART
  configuration.SPED.uartParity = MODE_00_8N1;                    //Paridad de la UART
  e32ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE); //Seteo de configuracion anterior
  printParameters(configuration);                                 //Impresion de parametros configurados
  c.close();

  //Impresion primera fila LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Dv"); //0-1
  lcd.setCursor(3, 0);
  lcd.print("T"); //2-3-4
  lcd.setCursor(5, 0);
  lcd.print("Hr"); //5-6-7
  lcd.setCursor(9, 0);
  lcd.print("B"); //8-9
  lcd.setCursor(13, 0); //10-11-12-13 14-15
  lcd.print("Vel");

  
  Serial.println();
  Serial.println("Start listening!");
  
}

void loop() 
{
  // TimeStamp that we'll be added to each Time Series Sample
  int64_t time;
  time = transport.getTimeMillis();
  //Waiting for messages
  if (e32ttl.available()  > 1){
    
    ResponseContainer rs = e32ttl.receiveMessage();
    // Getting the payload message 
    String message = rs.data;
    //Converting the message
    convertParameters(message);
    
    Serial.println(rs.status.getResponseDescription());
    Serial.println(message);
    

    


    
    //Sending response to devices
    if(device == 2){
      ts1.addSample(time, temperature2);
      ts2.addSample(time, humidity2);
      ts3.addSample(time, velocity2);
      ts4.addSample(time, pos2);
      ts5.addSample(time, diox2);
      ts6.addSample(time, battery2);
      if( alert == 1 ){
        sprintf(resp, "99999");
        ResponseStatus rs = e32ttl.sendFixedMessage(0x00, 0x00, 0x17, resp);
        Serial.println("Alert sent"); 
        }
      if (alert == 0) { 
        sprintf(resp, "22222");
        ResponseStatus rs = e32ttl.sendFixedMessage(0x00, 0x00, 0x17, resp);
        Serial.println("Check sent");
      }   
    }
    
    if(device == 3){

      ts7.addSample(time, temperature3);
      ts8.addSample(time, humidity3);
      ts9.addSample(time, velocity3);
      ts10.addSample(time, pos3);
      ts11.addSample(time, diox3);
      ts12.addSample(time, battery3);      
      if( alert == 1 ){
        sprintf(resp, "99999");
        ResponseStatus rs = e32ttl.sendFixedMessage(0x00, 0x00, 0x17, resp);
        Serial.println("Alert sent"); 
        }
      if (alert == 0) {
        sprintf(resp, "33333");
        ResponseStatus rs = e32ttl.sendFixedMessage(0x00, 0x00, 0x17, resp);
        Serial.println("Check sent"); 
        }   
    }


     loopCounter++;

    if (loopCounter >= 1) {
      // Send data
      loopCounter = 0;
      PromClient::SendResult res = client.send(req);
      if (!res == PromClient::SendResult::SUCCESS) {
          Serial.println(client.errmsg);
          // Note: additional retries or error handling could be implemented here.
          // the result could also be:
          // PromClient::SendResult::FAILED_DONT_RETRY
          // PromClient::SendResult::FAILED_RETRYABLE
      }
      // Batches are not automatically reset so that additional retry logic could be implemented by the library user.
      // Reset batches after a succesful send.
      ts1.resetSamples();
      ts2.resetSamples();
      ts3.resetSamples();
      ts4.resetSamples();
      ts5.resetSamples();
      ts6.resetSamples();
      ts7.resetSamples();
      ts8.resetSamples();
      ts9.resetSamples();
      ts10.resetSamples();
      ts11.resetSamples();
      ts12.resetSamples();
    }
      
    Serial.println("Channel update successful.");
      
    displayPrint(); 
  }


}

//Function to convert the received message 
void convertParameters(String message){
    device = (String(message[0]).toInt());
    //Depends on how parameters are sent on the original message -- protocol
    // In this case data is being sent on the following way: "d tt hh vvv ppp m b.bb"
    //d: Device [0], tt:temperature [2] y [3], hh: Humidity [5] y [6], vvv: velocity , ppp: position, m: diox, b.bb: Batery
    Serial.println(message);
    temperature=(String(message[2]).toInt())*10+(String(message[3]).toInt());
    humidity=(String(message[5]).toInt())*10+(String(message[6]).toInt());
    velocity=(String(message[8]).toInt())*100+(String(message[9]).toInt())*10+(String(message[10]).toInt());
    pos=(String(message[12]).toInt())*100+(String(message[13]).toInt())*10+(String(message[14]).toInt());
    battery=(String(message[16]).toInt())*100+(String(message[17]).toInt())*10+(String(message[18]).toInt());
    diox=(String(message[20]).toInt())*1000+(String(message[21]).toInt())*100+(String(message[22]).toInt())*10+(String(message[23]).toInt());
    Serial.println("Variables values are: ");


    if(device==2)
    {
       temperature2 = temperature;
       humidity2 = humidity;
       velocity2 = velocity;
       pos2 = pos;
       battery2 = battery;
       diox2 = diox;
       Serial.println(device);
       Serial.println(temperature2);
       Serial.println(humidity2);
       Serial.println(velocity2);
       Serial.println(pos2);
       Serial.println(battery2);
       Serial.println(diox2);

       temperature3 = 11;
       humidity3 = 11;
       velocity3 = 11;
       pos3 = 11;
       battery3 =11;
       diox3 = 11;     
    }
    
    if(device==3)
    {
       temperature3 = temperature;
       humidity3 = humidity;
       velocity3 = velocity;
       pos3 = pos;
       battery3 = battery;
       diox3 = diox;
       Serial.println(device);
       Serial.println(temperature3);
       Serial.println(humidity3);
       Serial.println(velocity3);
       Serial.println(pos3);
       Serial.println(battery3);
       Serial.println(diox3);

       temperature2 =25;
       humidity2 = 30;
       velocity2 = 2;
       pos2 = 45;
       battery2 = 225;
       diox2 = 1777;
    }    
   
 }

 //Function to print values on LCD display
void displayPrint () {  
    lcd.setCursor(0, 1);
    lcd.print("                ");
    //lcd.setCursor(0, 1);
    //lcd.print(device); //0-1
    lcd.setCursor(2, 1);
    lcd.print(temperature); //2-3-4
    lcd.setCursor(5, 1);
    lcd.print(humidity); //5-6-7
    lcd.setCursor(8, 1);
                                                lcd.print(battery); //8-9
    lcd.setCursor(13, 1); //10-11-12-13 14-15
    lcd.print(velocity);
    lcd.setCursor(0, 1); //14-15
    if((pos==22) || (pos==45) || (pos==67)) lcd.print("NE"); //14-15
    if(pos==360) lcd.print("N "); //14-15
    if((pos==337) || (pos==315) || (pos==292)) lcd.print("NO"); //14-15
    if(pos==270) lcd.print("0 "); //14-15
    if((pos==270) || (pos==225) || (pos==202)) lcd.print("SO"); //14-15
    if(pos==180) lcd.print("S "); //14-15
    if((pos==112) || (pos==157) || (pos==135)) lcd.print("SE"); //14-15
    if(pos==90) lcd.print("E "); //14-15
  
  }

 //Function to print parameters of the LoRa Device
void printParameters(struct Configuration configuration) {
    Serial.println("----------------------------------------");

    Serial.print(F("HEAD : "));  Serial.print(configuration.HEAD, BIN);Serial.print(" ");Serial.print(configuration.HEAD, DEC);Serial.print(" ");Serial.println(configuration.HEAD, HEX);
    Serial.println(F(" "));
    Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, DEC);
    Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, DEC);
    Serial.print(F("Chan : "));  Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); Serial.println(configuration.getChannelDescription());
    Serial.println(F(" "));
    Serial.print(F("SpeedParityBit     : "));  Serial.print(configuration.SPED.uartParity, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte  : "));  Serial.print(configuration.SPED.uartBaudRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getUARTBaudRate());
    Serial.print(F("SpeedAirDataRate   : "));  Serial.print(configuration.SPED.airDataRate, BIN);Serial.print(" -> "); Serial.println(configuration.SPED.getAirDataRate());

    Serial.print(F("OptionTrans        : "));  Serial.print(configuration.OPTION.fixedTransmission, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFixedTransmissionDescription());
    Serial.print(F("OptionPullup       : "));  Serial.print(configuration.OPTION.ioDriveMode, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getIODroveModeDescription());
    Serial.print(F("OptionWakeup       : "));  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
    Serial.print(F("OptionFEC          : "));  Serial.print(configuration.OPTION.fec, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getFECDescription());
    Serial.print(F("OptionPower        : "));  Serial.print(configuration.OPTION.transmissionPower, BIN);Serial.print(" -> "); Serial.println(configuration.OPTION.getTransmissionPowerDescription());

    Serial.println("----------------------------------------");

}
