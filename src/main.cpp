#include <Arduino.h>
#include <AccelStepper.h>
#include <Arduino_FreeRTOS.h>
//#include "avr8-stub.h"
#define Glue 8
#define Glue_sensor 7
#define Stepper_Sensor 9
#define Cutter 10
#define Cutter_Sensor 11
#define manual_pulse 5
#define home 12
#define motor_signal 4
bool motor_sensed= false;
bool lock = false;
bool lock2 = false;
bool lock3 = false;
bool capture_timer=false;
// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, 2, 3,true ); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5
int x =200;
bool start_cutter_timer = true;
long cutter_timer;
static TaskHandle_t stepper_task_handle = NULL;
static TaskHandle_t serial_handle = NULL;
String teststr = "s";
void TaskStepperRun( void *pvParameters );
void TaskGlue( void *pvParameters );
void TaskStepper( void *pvParameters );
void setup()
{ //debug_init();
  pinMode(Glue_sensor, INPUT_PULLUP);
  pinMode(Glue, OUTPUT);
  pinMode(Stepper_Sensor, INPUT_PULLUP);
  pinMode(Cutter, OUTPUT);
  pinMode(Cutter_Sensor, INPUT_PULLUP);
  pinMode(manual_pulse, INPUT_PULLUP);
  pinMode(home, INPUT_PULLUP);
  digitalWrite(Cutter, LOW);
   long initial_homing = 1 ;
  pinMode(motor_signal, OUTPUT);
    stepper.setMaxSpeed(100);
  	stepper.setAcceleration(50000);
    //stepper.setPinsInverted(true,true,true);
  //stepper.setSpeed(100);
  while (digitalRead(home))
  { 
      stepper.moveTo(initial_homing);
      initial_homing++;
      stepper.run();
  }
  stepper.setCurrentPosition(0);
  stepper.setMaxSpeed(4000);
  xTaskCreate(
    TaskStepper
    ,  "Stepper"
    ,  100  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  &stepper_task_handle );
     xTaskCreate(
    TaskGlue
    ,  "Glue"
    ,  100  // Stack size
    ,  NULL
    ,  2  // Priority
    ,  &serial_handle );
   xTaskCreate(
    TaskStepperRun
    ,  "StepperRun"   // A name just for humans
    ,  100  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

 
    
    //vTaskDelete(NULL);
}

void loop()
{
  
  //   if (((digitalRead(Stepper_Sensor)==0) && (motor_sensed == false)) || (digitalRead(manual_pulse)==0 && (motor_sensed==false)))
  // {
  //   stepper.move(400);
    
  //   motor_sensed = true;
  // }
  // if (motor_sensed)
  // { 
  //   if (stepper.distanceToGo()==0)
  //   { 
  //     if (lock2==false)
  //     {
  //     digitalWrite(Cutter,HIGH);
  //     lock2 = true;
  //     }
      
      
  //     if (start_cutter_timer==true)
  //     {
  //     cutter_timer= millis();
  //     start_cutter_timer = false;
  //     lock = true;
  //     }
  //     if (lock)
  //     {
  //       if (millis()-cutter_timer > 20)
  //       {
  //         digitalWrite(Cutter, LOW);
  //         capture_timer = millis();
  //         lock3 = true;
  //         lock = false;
  //       }
        
  //     }
  //     if (lock3)
  //     {
  //       if (millis()-capture_timer>20)
  //       {
  //         motor_sensed = false;
  //         lock3 = false;
  //         lock = false;
  //         lock2 = false;
  //         start_cutter_timer = true;
  //       }
        
  //     }
      
  //   }
  // }
  // if (stepper.isRunning())
  // {
  //   digitalWrite(12,HIGH);
  // }
  // else
  // digitalWrite(12, LOW);
  // //Serial.println("Current Pos:" + stepper.currentPosition());
  //  //Serial.println("Target Pos:" + stepper.targetPosition());
  
  //   stepper.run();
   
   

  
}

void TaskStepperRun(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  while (1)
  {
    
    stepper.run();
    taskYIELD();
  }
}
void TaskGlue(void *pvParameters)  // This is a task.
{ bool pos_to_neg = false;
  (void) pvParameters;
  while (true)
  {
    if((digitalRead(Glue_sensor) == 0) && (pos_to_neg == false))
  {
    
    vTaskDelay( 10 / portTICK_PERIOD_MS );
    digitalWrite(Glue, HIGH);
    vTaskDelay( 10 / portTICK_PERIOD_MS );
    digitalWrite(Glue, LOW);
    pos_to_neg=true;
  }
  if (pos_to_neg == true)
  {
      if (digitalRead(Glue_sensor)==1)
      {
        pos_to_neg = false;
      }
      
  }
  
  // Serial.print(F("unused stepper stack: "));
  // Serial.println(uxTaskGetStackHighWaterMark(stepper_task_handle));
  // Serial.print(F("unused serial stack: "));
  // Serial.println(uxTaskGetStackHighWaterMark(serial_handle));
  vTaskDelay(1);
  }
  
  
}
void TaskStepper(void *pvParameters)
{ 
  (void) pvParameters;
  while (true)
  { 
    if ((digitalRead(Stepper_Sensor)==0 && motor_sensed == false && digitalRead(Cutter_Sensor)==0))
  {
    stepper.move(400);
    
    motor_sensed = true;
  }
  if (motor_sensed)
  { 
    if (stepper.distanceToGo()==0)
    { 
      digitalWrite(Cutter,HIGH);
      vTaskDelay(20 / portTICK_PERIOD_MS);
      digitalWrite(Cutter,LOW);
      vTaskDelay(20 / portTICK_PERIOD_MS);
      motor_sensed = false;
    }
  }
  if (stepper.isRunning())
  {
    digitalWrite(4,HIGH);
  }
  else
  digitalWrite(4, LOW);
  //Serial.println("Current Pos:" + stepper.currentPosition());
   //Serial.println("Target Pos:" + stepper.targetPosition());
 
  vTaskDelay(25 / portTICK_PERIOD_MS);
  }
  
}
