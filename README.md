# ICT-v5-ATGM336
Ads support for WDT and switching on GPS only mode on the gps for powersaving

Ads support for WDT - Watch Dog Timer to prevent the program running in the 328 to freeze or lock up.          
Switching on GPS only mode on the gps for powersaving (BDS off), saves about 4-5mA 

Timing2.h is a testfile that will start tx every 2min

Fuses to set

62 D9 FE 1.8V / internal /div8 @ 1Mhz
E2 D9 FE 1.8V / internal 8Mhz osc
or the one if you use a 2/4/8Mhz external xo
FD D9 FE 
