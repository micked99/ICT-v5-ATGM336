/*
   Telemetry functions
*/

void setGPStime() // Sets the system time from the GPS
  {
  if (gps.time.isValid())
  {
    Hour = gps.time.hour();
    Minute = gps.time.minute();
    Second = gps.time.second();
    setTime(Hour, Minute, Second, 1, 6, 2021); // (hr,min,sec,day,month,yr)
  }

void loc4calc() // Determine the locator from the GPS data
{
  lon = (gps.location.lng() * 100000) + 18000000L;
  lat = (gps.location.lat() * 100000) +  9000000L;
  char MH[4] = {'A', 'A', '0', '0'};
  MH[0] +=  lon / 2000000;
  MH[1] +=  lat / 1000000;
  MH[2] += (lon % 2000000) / 200000;
  MH[3] += (lat % 1000000) / 100000;
  String loc4_txt = "";
  int i = 0;
  while (i < 4) {
    loc4_txt += MH[i];
    i++;
  }
  loc4_txt.toCharArray(loc4, 5);
  }

void call_telem() // Determine the telemetry callsign
  {
  alt_meters = gps.altitude.meters();
  char MH[2] = {'A', 'A'};
  MH[0] += ((lon % 200000) / 8333);
  MH[1] += ((lat % 100000) / 4166);
  call_telemetry[0] = 'Q'; // telemetry channel 11
  call_telemetry[2] = '0';
  int a = MH[0] - 'A';
  int b = MH[1] - 'A';
  int c = a * 24 + b;
  int d = int(alt_meters / 20);
  long e = (long)(1068L * c + d);
  long f = float(e / 17576L);
  if (f < 10)
  call_telemetry[1] = f + '0';
  else
  call_telemetry[1] = f - 10 + 'A';
  long g = e - f * 17576L;
  int h = int(g / 676);
  call_telemetry[3] = h + 'A';
  long i = g - h * 676L;
  int j = int(i / 26);
  call_telemetry[4] = j + 'A';
  long k = i - j * 26L;
  int l = int(k / 1);
  call_telemetry[5] = l + 'A';
}

void loc_dbm_telem() // Determine the locator and dBm value for the telemetry transmission
{
  Sats =  gps.satellites.value();
  gps_speed = gps.speed.knots();
  int wADC = 0;
  int temp = 0;
  float volt = 0;
  int sensorVolt = 0;
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
  ADCSRA |= _BV(ADEN);
  delay(20);
  for (int i=0;i<5;i++)
  {
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));
    wADC = wADC + ADCW;
    delay(20);
  }
  
  wADC = wADC / 5;
  temp = (wADC - 304.21 ) / 1.124;
  delay(20);
  analogReference(INTERNAL);
  for (int i=0;i<5;i++)
  {
    sensorVolt = sensorVolt + analogRead(3); //analogRead(0) for the old boards
  }
  sensorVolt = sensorVolt / 5;
  volt = sensorVolt * 1.1f;
  volt = volt / 1023.0f;
  volt = volt * 4.18f;
  if (volt < 3.0) volt = 3.0;
  if (volt > 4.95) volt = 4.95;
  if (temp < -49) temp = -49;
  if (temp > 39) temp = 39;
  int GPS = 0;
  if ((lon != oldlon) || (lat != oldlat))
  GPS = 1;
  else
  GPS = 0;
  oldlon = lon;
  oldlat = lat;
  if (Sats < 5) Sats = 0; else Sats = 1;
  int temp_raw = (int)(1024L * (temp * 0.01 + 2.73)) / 5;
  temp_raw = (int)(temp_raw - 457) / 2;
  float batt_raw = (int)(1024L * volt) / 5;
  batt_raw = (batt_raw - 614) / 10;
  float round_number = round(batt_raw);
  if (round_number > batt_raw) round_number = round_number - 1;
  batt_raw = (int)round_number;
  long x = (temp_raw * 40L ) + batt_raw;
  long y = (x * 42L) + (int)gps_speed / 2;
  long z = (y * 2L) + (int)GPS;
  long xy = (z * 2L) + (int)Sats;
  long a = (int)(xy / 34200L);
  loc_telemetry[0] = a + 'A';
  long b = xy - (34200L * a);
  long c = (int)(b / 1900L);
  loc_telemetry[1] = c + 'A';
  long d = b - (1900L * c);
  long e = (int)(d / 190L);
  loc_telemetry[2] = e + '0';
  long f = d - (190L * e);
  long g = (int) (f / 19L);
  loc_telemetry[3] = g + '0';
  dbm_telemetry = f - (g * 19L);
  if (dbm_telemetry == 0) dbm_telemetry = 0;
  else if (dbm_telemetry == 1) dbm_telemetry = 3;
  else if (dbm_telemetry == 2) dbm_telemetry = 7;
  else if (dbm_telemetry == 3) dbm_telemetry = 10;
  else if (dbm_telemetry == 4) dbm_telemetry = 13;
  else if (dbm_telemetry == 5) dbm_telemetry = 17;
  else if (dbm_telemetry == 6) dbm_telemetry = 20;
  else if (dbm_telemetry == 7) dbm_telemetry = 23;
  else if (dbm_telemetry == 8) dbm_telemetry = 27;
  else if (dbm_telemetry == 9) dbm_telemetry = 30;
  else if (dbm_telemetry == 10) dbm_telemetry = 33;
  else if (dbm_telemetry == 11) dbm_telemetry = 37;
  else if (dbm_telemetry == 12) dbm_telemetry = 40;
  else if (dbm_telemetry == 13) dbm_telemetry = 43;
  else if (dbm_telemetry == 14) dbm_telemetry = 47;
  else if (dbm_telemetry == 15) dbm_telemetry = 50;
  else if (dbm_telemetry == 16) dbm_telemetry = 53;
  else if (dbm_telemetry == 17) dbm_telemetry = 57;
  else if (dbm_telemetry == 18) dbm_telemetry = 60;
  //Serial.println(dbm_telemetry);
}

void setModeWSPR()
{
  symbol_count = WSPR_SYMBOL_COUNT;
  tone_spacing = WSPR_TONE_SPACING;
  memset(tx_buffer, 0, 255); // Clears Tx buffer from previous operation.
  jtencode.wspr_encode(call, loc4, 10, tx_buffer);
}

void setModeWSPR_telem()
{
  symbol_count = WSPR_SYMBOL_COUNT;
  tone_spacing = WSPR_TONE_SPACING;
  memset(tx_buffer, 0, 255); // Clears Tx buffer from previous operation.
  jtencode.wspr_encode(call_telemetry, loc_telemetry, dbm_telemetry, tx_buffer);
}

void encode() // Loop through the string, transmitting one character at a time
{
  uint8_t i;
  for (i = 0; i < symbol_count; i++) // Now transmit the channel symbols
{
    //si5351.output_enable(SI5351_CLK0, 1); // Turn off the CLK0 output
    //si5351.set_freq_manual((freq * 100) + (tx_buffer[i] * tone_spacing),87500000000ULL,SI5351_CLK0);
    si5351.set_freq((freq * 100) + (tx_buffer[i] * tone_spacing),SI5351_CLK0);
    proceed = false;
    while (!proceed);
    sodaq_wdt_reset();
}
  si5351.output_enable(SI5351_CLK0, 0); // Turn off the CLK0 output
  si5351.set_clock_pwr(SI5351_CLK0, 0);  // Turn off the CLK0 clock
}

void rf_on() // Turn on the high-side switch, activating the transmitter
{
  digitalWrite(2, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  delay(2);
  si5351.init(SI5351_CRYSTAL_LOAD_0PF, TCXO_REF_FREQ, 0); // TCXO 26MHz
  si5351.set_clock_pwr(SI5351_CLK1, 0);  // Turn off the CLK1 clock
  si5351.output_enable(SI5351_CLK1, 0);  // Turn off the CLK1 output
  si5351.set_clock_pwr(SI5351_CLK2, 0);  // Turn off the CLK2 clock
  si5351.output_enable(SI5351_CLK2, 0);  // Turn off the CLK2 output
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA); // Set for max power if desired. Check datasheet.
  si5351.output_enable(SI5351_CLK0, 0);  // Disable the clock initially
}

void rf_off() // Turn off the high-side switch
{
  digitalWrite(2, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
}

void GPS_VCC_on()
  {
  Serial.begin(9600);
  delay(2);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  }

void GPS_VCC_off()
  {
  Serial.end();
  delay(2);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  }
