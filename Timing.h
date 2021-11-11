void TXencode() {
    if (telemetry_set) {
      call_telem();
      loc_dbm_telem();
      setModeWSPR_telem();
      telemetry_set = false;
    }
    else {
      loc4calc();
      setModeWSPR();
    }
    GPS_VCC_off(); delay(10);
    rf_on();
    freq = WSPR_FREQ;
    encode();
    rf_off(); delay(5);
    GPS_VCC_on();
}

void TXtiming() // Timing
{
  if ((timeStatus() == timeSet) && (second() == 0) && (minute() >= 0))
  {
    setGPStime();
    if ((minute() % 10 == 0) && (second() <= 0)) {         // 00 - TX WSPR standard message
      TXencode();
    }
    
    else if ((minute() % 10 == 2) && (second() <= 0) ) {   // 02 - TX WSPR telemetry message
      telemetry_set = true; // 02 is WSPR telemetry
      TXencode();
    }
    
    else if ((minute() % 10 == 4) && (second() <= 0) ) {   // 04 - TX WSPR standard message
      TXencode();
    }
    
    else if ((minute() % 10 == 8) && (second() <= 0) ) {  // 08 - TX WSPR telemetry message
      TXencode();
    }
  }
}
