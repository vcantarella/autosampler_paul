#include <AccelStepper.h>

// --- Pin Definitions ---
#define enPin 8
// Motor X
#define stepPin1 2
#define dirPin1 5
// Motor Y
#define stepPin2 3
#define dirPin2 6
// Motor Z
#define stepPin3 4
#define dirPin3 7
// Motor A
#define stepPin4 12
#define dirPin4 13

// --- Global Motor Objects ---
AccelStepper stepperX(AccelStepper::DRIVER, stepPin1, dirPin1);
AccelStepper stepperY(AccelStepper::DRIVER, stepPin2, dirPin2);
AccelStepper stepperZ(AccelStepper::DRIVER, stepPin3, dirPin3);
AccelStepper stepperA(AccelStepper::DRIVER, stepPin4, dirPin4);

// Define the states for our state machine
enum State {
  WAITING,
  MOVING
};

// The global lock variable
bool isAMotorMoving = false;

// --- Create a structure to hold all data for a single motor ---
struct Motor {
  AccelStepper &stepper;
  const char* name;
  State currentState;
  unsigned long waitStartTime;
  long currentPosition;
};

// All motors start in the WAITING state
Motor motors[] = {
  //{stepperX, "Paul X", WAITING, 0, 0},
  //{stepperY, "Paul Y", WAITING, 0, 0},
  {stepperZ, "Paul Z", WAITING, 0, 0},
  //{stepperA, "Paul A", WAITING, 0, 0}
};
const int numMotors = sizeof(motors) / sizeof(motors[0]);

// --- Global Interval ---
const unsigned long interval = 120*1000L;//6L * 60L * 60L * 1000L; // 2 hours
//const unsigned long interval = 20L * 1000L; // 20 seconds


void check_motor_status(Motor &motor) {
  switch (motor.currentState) {
    case WAITING:
      // A motor can only start if its wait time is over AND the lock is free.
      if ((millis() - motor.waitStartTime >= interval) && !isAMotorMoving) {
        // --- This motor gets the lock! ---
        isAMotorMoving = true; // Set the lock
        
        motor.currentPosition += 20;
        Serial.print(motor.name);
        Serial.print(" | Interval finished. Taking lock & starting move to: ");
        Serial.println(motor.currentPosition);
        motor.stepper.moveTo(motor.currentPosition);
        motor.currentState = MOVING;
      }
      break;

    case MOVING:
      // Check if this motor has reached its target
      if (motor.stepper.distanceToGo() == 0) {
        // --- This motor releases the lock! ---
        isAMotorMoving = false; // Release the lock
        
        Serial.print(motor.name);
        Serial.println(" | Move complete. Releasing lock & starting wait timer...");
        motor.waitStartTime = millis(); // Start this motor's timer for the next cycle
        motor.currentState = WAITING;
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW); // Enable all motors
  long interval_hours = interval / (1000L * 60L * 60L);
  Serial.print("Rotation interval set to: ");
  Serial.print(interval_hours);
  Serial.println(" hours.");

  Serial.println("Setup done! Starting initial motor sequence...");

  // Configure all motors
  for (int i = 0; i < numMotors; i++) {
    motors[i].stepper.setMaxSpeed(4);
    motors[i].stepper.setAcceleration(3);
    motors[i].stepper.setCurrentPosition(0);
    
    // --- THIS IS THE ONLY CHANGE ---
    // By subtracting the interval, we make the timers "expired" from the start.
    motors[i].waitStartTime = millis() - interval;
  }
}

void loop() {
  // Loop through each motor to check its status and run it
  for (int i = 0; i < numMotors; i++) {
    check_motor_status(motors[i]);
    motors[i].stepper.run();
  }
}