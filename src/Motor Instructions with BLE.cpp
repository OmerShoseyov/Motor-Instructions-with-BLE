#include <Arduino.h>

#include <ArduinoBLE.h>



//----------------------------------------------------------------------------------------------------------------------
// BLE UUIDs
//----------------------------------------------------------------------------------------------------------------------

#define BLE_UUID_MOTOR_INSTRUCTIONS_SERVICE        "9e661242-e694-4262-b393-61744b474383"
#define BLE_UUID_GET_G                             "73cce820-35ee-47f2-bca7-31105f673e20"
#define BLE_UUID_ROTATION_DIRECTION                "aae6924e-fdd1-40d7-9e71-b026bcec5cc9"
#define BLE_UUID_MEASUREMENT_TIME                  "80077d16-c5c6-48c4-8c10-7da0b6fe3e6f"
#define BLE_UUID_START_FINISH                      "40e0f2d5-2327-4930-83ca-c2ca729ca1bb"
#define BLE_UUID_STATUS                            "5720e451-6f72-49f1-8721-079d7e4bb497"   

//----------------------------------------------------------------------------------------------------------------------
// BLE
//----------------------------------------------------------------------------------------------------------------------

BLEService Motor_Instructions_Service( BLE_UUID_MOTOR_INSTRUCTIONS_SERVICE );
BLEStringCharacteristic Get_g_Characteristic( BLE_UUID_GET_G, BLERead | BLEWrite, 20);
BLEStringCharacteristic Rotation_Direction_Characteristic( BLE_UUID_ROTATION_DIRECTION, BLERead | BLEWrite, 20 );
BLEStringCharacteristic Measurment_Time_Characteristic ( BLE_UUID_MEASUREMENT_TIME, BLERead | BLEWrite, 20 );
BLEStringCharacteristic Start_Finish_Characteristic( BLE_UUID_START_FINISH, BLERead | BLEWrite, 20 );
BLEStringCharacteristic Status_Characteristic( BLE_UUID_STATUS, BLERead | BLEWrite, 20 );

const int ledPin = LED_BUILTIN;

String get_g;
String Rotation_Direction;
String Measurment_Time;
String Start_Finish;

float get_g_to_float;
float Rotation_Speed;
float Rotation_Direction_to_float;
long Measurment_Time_to_long;
int Start_Finish_to_int;


float g = 9.81; //9.81 m/s^2
float r = 0.2; //20cm
float g_to_speed = sqrt(g/r)*(5/PI);




bool setupBleMode()
{
  if ( !BLE.begin() )
  {
    Serial.println("Starting BLE failed!");
    digitalWrite(LEDR, LOW);
    delay(1000);
    digitalWrite(LEDR, HIGH);
    return false;
  }

  // set advertised local name and service UUID:
  BLE.setDeviceName( "Odrive Arduino" );
  BLE.setLocalName( "Odrive Arduino" );
  BLE.setAdvertisedService( Motor_Instructions_Service );

  // BLE add characteristics
  Motor_Instructions_Service.addCharacteristic( Get_g_Characteristic );
  Motor_Instructions_Service.addCharacteristic( Rotation_Direction_Characteristic );
  Motor_Instructions_Service.addCharacteristic( Measurment_Time_Characteristic );
  Motor_Instructions_Service.addCharacteristic( Start_Finish_Characteristic );
  Motor_Instructions_Service.addCharacteristic( Status_Characteristic );
  

  // add service
  BLE.addService( Motor_Instructions_Service );

  // set the initial value for the characeristic:
 /* Get_g_Characteristic.writeValue( 0 );
  Rotation_Direction_Characteristic.writeValue( 0 );
  Measurment_Time_Characteristic.writeValue( 0 );
  Start_Finish_Characteristic.writeValue( 0);
  */
  Status_Characteristic.writeValue("Ready");
  // start advertising
  BLE.advertise();

  return true;
}




void setup()
{
  Serial.begin( 115200 );
  //while ( !Serial );

  pinMode( ledPin, OUTPUT );
  digitalWrite(ledPin, HIGH);
  

  if( setupBleMode() )
  {
    digitalWrite( LEDB, LOW );
    delay(1000);
    digitalWrite(LEDB, HIGH);    
    Serial.println("BLE Control ready");
  }
  ///print MAC address
  Serial.print("My BLE MAC:\t\t ");
  Serial.println(BLE.address());
}

  void loop(){
  
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();
  
  if ( central )
  {
    Serial.println( "Connected to central: " );

    while ( central.connected() ){
        digitalWrite( LEDB, LOW );
        get_g = Get_g_Characteristic.value();
        Rotation_Direction = Rotation_Direction_Characteristic.value();
        Measurment_Time = Measurment_Time_Characteristic.value();
        Start_Finish = Start_Finish_Characteristic.value();   
         
    } // while connected

    Serial.print( F( "Disconnected from central: " ) );
    Serial.println( central.address() );
    
/////// convert strings to valid values ///////
    get_g_to_float = get_g.toFloat();
    Rotation_Direction_to_float = Rotation_Direction.toFloat(); 
    Measurment_Time_to_long = Measurment_Time.toInt();
    Start_Finish_to_int = Start_Finish.toInt();

    Rotation_Speed = sqrt(get_g_to_float)*g_to_speed;
    

    Status_Characteristic.writeValue("Going");
    Serial.print("get_g = ");
    Serial.print(get_g_to_float);
    Serial.print(", ");
    Serial.print("Rotation_Speed = ");
    Serial.print(Rotation_Speed);
    Serial.print(", ");
    Serial.print("Rotation_Direction = ");
    Serial.print(Rotation_Direction_to_float);
    Serial.print(", ");
    Serial.print("Measurment_Time = ");
    Serial.print(Measurment_Time_to_long);
    Serial.print(", ");
    Serial.print("Start_Finish = ");
    Serial.print(Start_Finish_to_int);
    Serial.println(" ");
    digitalWrite( LEDB, HIGH );
  } // if central

  if(Start_Finish_to_int == 1){
    float temp = Rotation_Speed*Rotation_Direction_to_float;
    Serial.println(temp);
    Serial.println("go motor");
    Serial.println("out");
    Start_Finish_to_int = 0;
    Status_Characteristic.writeValue("Ready");
    digitalWrite( LEDG, LOW );
    delay(1000);
    digitalWrite(LEDG, HIGH);
  }
  
} // loop

