#define FLOW_SENSOR_INLET 16  // GPIO pin for Inlet Flow Sensor
#define FLOW_SENSOR_OUTLET 17 // GPIO pin for Outlet Flow Sensor
#define BUZZER_PIN 26         // GPIO pin for Buzzer

#include <WiFi.h>
#define BLYNK_TEMPLATE_ID "TMPL3JI_PRD9f"
#define BLYNK_TEMPLATE_NAME "Water Leakage Detection"
#define BLYNK_AUTH_TOKEN "TMir1HpFson8y1Yp_jzdT_OVBRfKBIZM"

#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h> 

// Wi-Fi credentials
char ssid[] = "Redmi 12 5G";
char pass[] = "sh123456";

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Blynk authentication
char auth[] = "TMir1HpFson8y1Yp_jzdT_OVBRfKBIZM";

volatile int pulseCountInlet = 0;  // Pulse count for Inlet
volatile int pulseCountOutlet = 0; // Pulse count for Outlet

float flowRateInlet = 0.0;  // Inlet flow rate
float flowRateOutlet = 0.0; // Outlet flow rate
float flowCalibrationFactor = 4.5; // Calibration factor for flow sensors
float leakageThreshold = 1.0;  // Threshold for leakage
float minimumFlowRate = 0.5;   // Minimum flow rate to consider

BlynkTimer timer;  // Blynk timer for sending data periodically
unsigned long lastLeakageCheck = 0; // Timestamp of the last leakage check
unsigned long bufferTime = 2000; // Time buffer in milliseconds (2 seconds)

bool isLeakageDetected = false; // Keeps track of the leakage status

// Interrupt Service Routines (ISR)
void IRAM_ATTR pulseInlet() {
  pulseCountInlet++;
  Serial.println("Inlet pulse detected");  // Debugging statement
}

void IRAM_ATTR pulseOutlet() {
  pulseCountOutlet++;
  Serial.println("Outlet pulse detected"); // Debugging statement
}

void sendDataToBlynk() {
  // Convert pulses to flow rate
  flowRateInlet = pulseCountInlet * flowCalibrationFactor;
  flowRateOutlet = pulseCountOutlet * flowCalibrationFactor;

  Serial.print("Inlet Flow Rate: ");
  Serial.print(flowRateInlet);
  Serial.print(" L/min, Outlet Flow Rate: ");
  Serial.print(flowRateOutlet);
  Serial.println(" L/min");

  // Update LCD with both values
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Inlet: ");
  lcd.print(flowRateInlet, 2);
  lcd.setCursor(0, 1);
  lcd.print("Outlet: ");
  lcd.print(flowRateOutlet, 2);

  // Check for leakage
  unsigned long currentTime = millis();
  if (currentTime - lastLeakageCheck >= bufferTime) {
    if (abs(flowRateInlet - flowRateOutlet) > leakageThreshold &&
        flowRateInlet > minimumFlowRate && flowRateOutlet > minimumFlowRate) {
      if (!isLeakageDetected) { 
        Serial.println("Warning: Water Leakage Detected!");
        Blynk.virtualWrite(V2, "Leakage Detected");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Warning:");
        lcd.setCursor(0, 1);
        lcd.print("Leakage Detected!");
        Blynk.logEvent("leakage_warning", "Water Leakage Detected!");

        // Activate the buzzer
        digitalWrite(BUZZER_PIN, HIGH);
        isLeakageDetected = true;
      }
    } else {
      if (isLeakageDetected) { // Reset state if no leakage
        Serial.println("No Leakage Detected");
        Blynk.virtualWrite(V2, "No Leakage");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Inlet: ");
        lcd.print(flowRateInlet, 2);
        lcd.setCursor(0, 1);
        lcd.print("Outlet: ");
        lcd.print(flowRateOutlet, 2);

        // Turn off buzzer
        digitalWrite(BUZZER_PIN, LOW);
        isLeakageDetected = false;
      }
    }
    lastLeakageCheck = currentTime; // Update timestamp
  }

  // Send flow data to Blynk
  Blynk.virtualWrite(V0, flowRateInlet);
  Blynk.virtualWrite(V1, flowRateOutlet);

  // Reset pulse counts for next cycle
  pulseCountInlet = 0;
  pulseCountOutlet = 0;
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.print("Initializing...");
  
  // Connect to WiFi and Blynk
  Blynk.begin(auth, ssid, pass);

  // Configure flow sensors
  pinMode(FLOW_SENSOR_INLET, INPUT_PULLUP);
  pinMode(FLOW_SENSOR_OUTLET, INPUT_PULLUP);

  // Configure buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW); // Ensure buzzer is off initially

  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_INLET), pulseInlet, RISING);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_OUTLET), pulseOutlet, RISING);

  // Set up Blynk timer to send data every second
  timer.setInterval(1000L, sendDataToBlynk);

  Serial.println("System Initialized");
  lcd.clear();
  lcd.print("System Ready");
}

void loop() {
  Blynk.run();
  timer.run();
}
