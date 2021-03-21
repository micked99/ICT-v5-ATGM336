//#pragma GCC diagnostic error "-Wconversion"

/*
   HABalloon by KD2NDR, Miami Florida October 25 2018
   Improvements by YO3ICT, Bucharest Romania, April-May 2019 
   Improvements By SA6BSS, Sweden, fall 2020
   You may use and modify the following code to suit
   your needs so long as it remains open source
   and it is for non-commercial use only.
*/
#include <Sodaq_wdt.h>
#include <si5351.h>
#include <JTEncode.h>
#include <TimeLib.h>
#include <TinyGPS++.h>
#include <avr/power.h>

#define WSPR_CTC                5336   // CTC value for WSPR - 10672 @ 16Mhz //5336 @ 8Mhz //2668 @ 4Mhz //1334 @ 2Mhz //667 @ 1Mhz
#define WSPR_TONE_SPACING       146    // 146 ~1.46 Hz

//#define WSPR_FREQ     7040185UL
//#define WSPR_FREQ     10140285UL          
#define WSPR_FREQ     14097192UL      // <<<<< SET TX FREQUENCY HERE

// Enumerations
enum mode {MODE_WSPR};
TinyGPSPlus gps;
Si5351 si5351;
JTEncode jtencode;

// Global variables
unsigned long freq;
char call[] = "LY1XD"; // WSPR Standard callsign
char call_telemetry[7]; // WSPR telemetry callsign
char loc_telemetry[5]; // WSPR telemetry locator
uint8_t dbm_telemetry; // WSPR telemetry dbm
char loc4[5]; // 4 digit gridsquare locator
byte Hour, Minute, Second; // Used for timing
long lat, lon, oldlat, oldlon; // Used for location
uint8_t tx_buffer[255]; // WSPR Tx buffer
uint8_t symbol_count; // JTencode
uint16_t tone_delay, tone_spacing; // JTencode
int alt_meters = 0;
bool telemetry_set = false;
int Sats = 0;
int gps_speed = 0;
volatile bool proceed = false;

#include "TelemFunctions.h" // Various telemetry functions
#include "Timing2.h" // Scheduling

ISR(TIMER1_COMPA_vect)
{ proceed = true; }

void setup()
{
  //clock_prescale_set(clock_div_2);
  //clock_prescale_set(clock_div_4);
  //clock_prescale_set(clock_div_8);

  sodaq_wdt_enable(WDT_PERIOD_8X);
  
  pinMode(3, OUTPUT); digitalWrite(3, HIGH); //gps pin 5 on
  
  pinMode(2, OUTPUT);  // Si5351 off
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);

  pinMode(A0, OUTPUT); // GPS VCC on
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH); 

  Serial.begin(9600);
  delay(1000); 
  Serial.write("$PCAS04,1*18\r\n"); //Sets navsystem of the ATGM to GPS only
  delay(1000); 

  noInterrupts(); // Set up Timer1 for interrupts every symbol period.
  TCCR1A = 0;
  TCNT1  = 0;
  TCCR1B = (1 << CS12) |
           (1 << CS10) |
           (1 << WGM12);
  TIMSK1 = (1 << OCIE1A);
  OCR1A = WSPR_CTC;
  interrupts();
  sodaq_wdt_reset();
  }

  void loop() 
  {
    sodaq_wdt_reset();
    while (Serial.available() > 0)
    if (gps.encode(Serial.read())) // GPS related functions need to be in here to work with tinyGPS Plus library
    if (timeStatus() == timeNotSet) // Only sets time if already not done previously
  { setGPStime(); } // Sets system time to GPS UTC time for sync
    if (gps.location.isValid()) TXtiming(); // Process timing 
  }
