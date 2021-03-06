EESchema Schematic File Version 4
LIBS:romprg_megadrive_cart-cache
EELAYER 26 0
EELAYER END
$Descr A4 8268 11693 portrait
encoding utf-8
Sheet 1 1
Title "Sega Megadrive cart programmer - 29f based"
Date ""
Rev "1"
Comp ""
Comment1 "file, You can obtain one at http://mozilla.org/MPL/2.0/."
Comment2 "License, v. 2.0. If a copy of the MPL was not distributed with this"
Comment3 "This Source Code Form is subject to the terms of the Mozilla Public"
Comment4 ""
$EndDescr
$Comp
L arduino:Arduino_Mega2560_Shield XA1
U 1 1 5B94F068
P 2850 3950
F 0 "XA1" H 2850 1570 60  0000 C CNN
F 1 "Arduino_Mega2560_Shield" H 2850 1464 60  0000 C CNN
F 2 "romprg:Arduino_Mega2560_Shield_no_icsp" H 3550 6700 60  0001 C CNN
F 3 "https://store.arduino.cc/arduino-mega-2560-rev3" H 3550 6700 60  0001 C CNN
	1    2850 3950
	1    0    0    -1  
$EndComp
Text GLabel 4150 3000 2    50   Input ~ 0
PA0
Text GLabel 4150 3100 2    50   Input ~ 0
PA1
Text GLabel 4150 3200 2    50   Input ~ 0
PA2
Text GLabel 4150 3300 2    50   Input ~ 0
PA3
Text GLabel 4150 3400 2    50   Input ~ 0
PA4
Text GLabel 4150 3500 2    50   Input ~ 0
PA5
Text GLabel 4150 3600 2    50   Input ~ 0
PA6
Text GLabel 4150 3700 2    50   Input ~ 0
PA7
Text GLabel 4150 4500 2    50   Input ~ 0
PC0
Text GLabel 4150 4300 2    50   Input ~ 0
PC2
Text GLabel 4150 4200 2    50   Input ~ 0
PC3
Text GLabel 4150 4100 2    50   Input ~ 0
PC4
Text GLabel 4150 4000 2    50   Input ~ 0
PC5
Text GLabel 4150 3900 2    50   Input ~ 0
PC6
Text GLabel 4150 3800 2    50   Input ~ 0
PC7
Text GLabel 4150 4400 2    50   Input ~ 0
PC1
Text GLabel 4150 6100 2    50   Input ~ 0
PB0
Text GLabel 4150 5900 2    50   Input ~ 0
PB2
Text GLabel 4150 5800 2    50   Input ~ 0
PB3
Text GLabel 4150 6000 2    50   Input ~ 0
PB1
Text GLabel 4150 2600 2    50   Input ~ 0
PB4
Text GLabel 4150 2700 2    50   Input ~ 0
PB5
Text GLabel 4150 2800 2    50   Input ~ 0
PB6
Text GLabel 1550 3900 0    50   Input ~ 0
PK0
Text GLabel 1550 4000 0    50   Input ~ 0
PK1
Text GLabel 1550 4100 0    50   Input ~ 0
PK2
Text GLabel 1550 4200 0    50   Input ~ 0
PK3
Text GLabel 1550 4300 0    50   Input ~ 0
PK4
Text GLabel 1550 4400 0    50   Input ~ 0
PK5
Text GLabel 1550 4500 0    50   Input ~ 0
PK6
Text GLabel 1550 4600 0    50   Input ~ 0
PK7
Text GLabel 4150 5700 2    50   Input ~ 0
PL0
Text GLabel 4150 5500 2    50   Input ~ 0
PL2
Text GLabel 4150 5400 2    50   Input ~ 0
PL3
Text GLabel 4150 5300 2    50   Input ~ 0
PL4
Text GLabel 4150 5200 2    50   Input ~ 0
PL5
Text GLabel 4150 5100 2    50   Input ~ 0
PL6
Text GLabel 4150 5000 2    50   Input ~ 0
PL7
Text GLabel 4150 5600 2    50   Input ~ 0
PL1
Text GLabel 4150 4900 2    50   Input ~ 0
PG0
Text GLabel 4150 4700 2    50   Input ~ 0
PG2
Text GLabel 4150 4800 2    50   Input ~ 0
PG1
$Comp
L power:GND #PWR0101
U 1 1 5B94F313
P 1450 5600
F 0 "#PWR0101" H 1450 5350 50  0001 C CNN
F 1 "GND" H 1450 5450 50  0000 C CNN
F 2 "" H 1450 5600 50  0001 C CNN
F 3 "" H 1450 5600 50  0001 C CNN
	1    1450 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 5600 1550 5600
Wire Wire Line
	1550 5500 1450 5500
Wire Wire Line
	1450 5500 1450 5600
Connection ~ 1450 5600
Wire Wire Line
	1550 5400 1450 5400
Wire Wire Line
	1450 5400 1450 5500
Connection ~ 1450 5500
Wire Wire Line
	1550 5300 1450 5300
Wire Wire Line
	1450 5300 1450 5400
Connection ~ 1450 5400
Wire Wire Line
	1550 5200 1450 5200
Wire Wire Line
	1450 5200 1450 5300
Connection ~ 1450 5300
$Comp
L power:+5V #PWR0102
U 1 1 5B94F3E4
P 1250 5800
F 0 "#PWR0102" H 1250 5650 50  0001 C CNN
F 1 "+5V" H 1265 5973 50  0000 C CNN
F 2 "" H 1250 5800 50  0001 C CNN
F 3 "" H 1250 5800 50  0001 C CNN
	1    1250 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 5800 1550 5800
Wire Wire Line
	1550 5900 1250 5900
Wire Wire Line
	1250 5900 1250 5800
Connection ~ 1250 5800
Wire Wire Line
	1550 6000 1250 6000
Wire Wire Line
	1250 6000 1250 5900
Connection ~ 1250 5900
$Comp
L megadrive:smd_cart_connector_slot CON1
U 1 1 5B94F9C7
P 6400 2750
F 0 "CON1" H 6400 3587 60  0000 C CNN
F 1 "cartridge_connector" H 6400 3481 60  0000 C CNN
F 2 "megadrive:cart_slot" H 6400 2750 60  0001 C CNN
F 3 "" H 6400 2750 60  0000 C CNN
	1    6400 2750
	1    0    0    -1  
$EndComp
Text GLabel 5750 3000 0    50   Input ~ 0
PK0
Text GLabel 5750 3100 0    50   Input ~ 0
PK1
Text GLabel 5750 3200 0    50   Input ~ 0
PK2
Text GLabel 5750 3300 0    50   Input ~ 0
PK3
Text GLabel 5750 3400 0    50   Input ~ 0
PK4
Text GLabel 5750 3500 0    50   Input ~ 0
PK5
Text GLabel 5750 3600 0    50   Input ~ 0
PK6
Text GLabel 5750 3700 0    50   Input ~ 0
PK7
Text GLabel 5750 3800 0    50   Input ~ 0
PB0
Text GLabel 5750 3900 0    50   Input ~ 0
PB1
Text GLabel 5750 4000 0    50   Input ~ 0
PB2
Text GLabel 5750 4100 0    50   Input ~ 0
PB3
Text GLabel 5750 4200 0    50   Input ~ 0
PB4
Text GLabel 5750 4300 0    50   Input ~ 0
PB5
Text GLabel 5750 4400 0    50   Input ~ 0
PB6
Text GLabel 5750 2200 0    50   Input ~ 0
PL0
Text GLabel 5750 2300 0    50   Input ~ 0
PL1
Text GLabel 5750 2400 0    50   Input ~ 0
PL2
Text GLabel 5750 2500 0    50   Input ~ 0
PL3
Text GLabel 5750 2600 0    50   Input ~ 0
PL4
Text GLabel 5750 2700 0    50   Input ~ 0
PL5
Text GLabel 5750 2800 0    50   Input ~ 0
PL6
Text GLabel 5750 2900 0    50   Input ~ 0
PL7
Text GLabel 7050 3000 2    50   Input ~ 0
PC0
Text GLabel 7050 3100 2    50   Input ~ 0
PC1
Text GLabel 7050 3200 2    50   Input ~ 0
PC2
Text GLabel 7050 3300 2    50   Input ~ 0
PC3
Text GLabel 7050 3400 2    50   Input ~ 0
PC4
Text GLabel 7050 3500 2    50   Input ~ 0
PC5
Text GLabel 7050 3600 2    50   Input ~ 0
PC6
Text GLabel 7050 3700 2    50   Input ~ 0
PC7
Text GLabel 7050 2200 2    50   Input ~ 0
PA0
Text GLabel 7050 2300 2    50   Input ~ 0
PA1
Text GLabel 7050 2400 2    50   Input ~ 0
PA2
Text GLabel 7050 2500 2    50   Input ~ 0
PA3
Text GLabel 7050 2600 2    50   Input ~ 0
PA4
Text GLabel 7050 2700 2    50   Input ~ 0
PA5
Text GLabel 7050 2800 2    50   Input ~ 0
PA6
Text GLabel 7050 2900 2    50   Input ~ 0
PA7
Text GLabel 7050 4300 2    50   Input ~ 0
PG0
Text GLabel 7050 4200 2    50   Input ~ 0
PG1
Text GLabel 5750 4800 0    50   Input ~ 0
PG2
$Comp
L power:GND #PWR0103
U 1 1 5B9517F7
P 5450 5400
F 0 "#PWR0103" H 5450 5150 50  0001 C CNN
F 1 "GND" H 5455 5227 50  0000 C CNN
F 2 "" H 5450 5400 50  0001 C CNN
F 3 "" H 5450 5400 50  0001 C CNN
	1    5450 5400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0104
U 1 1 5B951A96
P 5450 5000
F 0 "#PWR0104" H 5450 4850 50  0001 C CNN
F 1 "+5V" H 5465 5173 50  0000 C CNN
F 2 "" H 5450 5000 50  0001 C CNN
F 3 "" H 5450 5000 50  0001 C CNN
	1    5450 5000
	1    0    0    -1  
$EndComp
NoConn ~ 7050 5400
NoConn ~ 7050 5300
NoConn ~ 7050 5200
NoConn ~ 7050 5100
NoConn ~ 7050 5000
NoConn ~ 7050 4900
NoConn ~ 7050 4800
NoConn ~ 7050 4700
NoConn ~ 7050 4600
NoConn ~ 7050 4500
NoConn ~ 7050 4400
NoConn ~ 7050 4100
NoConn ~ 7050 4000
NoConn ~ 7050 3900
NoConn ~ 5750 4600
NoConn ~ 5750 4700
NoConn ~ 5750 4900
NoConn ~ 4150 4600
NoConn ~ 1550 4900
NoConn ~ 1550 5000
NoConn ~ 1550 5700
NoConn ~ 1550 6100
NoConn ~ 4150 2500
NoConn ~ 4150 2400
NoConn ~ 4150 2300
NoConn ~ 4150 2200
NoConn ~ 4150 2100
NoConn ~ 4150 1900
NoConn ~ 4150 1800
Wire Wire Line
	5450 5000 5750 5000
Wire Wire Line
	5750 5100 5450 5100
Wire Wire Line
	5450 5100 5450 5000
Connection ~ 5450 5000
Wire Wire Line
	5750 5400 5450 5400
Wire Wire Line
	5750 5300 5450 5300
Wire Wire Line
	5450 5300 5450 5400
Connection ~ 5450 5400
Wire Wire Line
	5750 5200 5450 5200
Wire Wire Line
	5450 5200 5450 5300
Connection ~ 5450 5300
NoConn ~ 2600 1350
NoConn ~ 2700 1350
NoConn ~ 2800 1350
NoConn ~ 2900 1350
NoConn ~ 3000 1350
NoConn ~ 3100 1350
NoConn ~ 1550 1800
NoConn ~ 1550 1900
NoConn ~ 1550 2000
NoConn ~ 1550 2100
NoConn ~ 1550 2200
NoConn ~ 1550 2300
NoConn ~ 1550 2400
NoConn ~ 1550 2500
NoConn ~ 1550 2600
NoConn ~ 1550 2700
NoConn ~ 1550 2800
NoConn ~ 1550 2900
NoConn ~ 1550 4700
NoConn ~ 1550 3100
NoConn ~ 1550 3200
NoConn ~ 1550 3300
NoConn ~ 1550 3400
NoConn ~ 1550 3500
NoConn ~ 1550 3600
NoConn ~ 1550 3700
NoConn ~ 1550 3800
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 5B960E29
P 950 5800
F 0 "#FLG0101" H 950 5875 50  0001 C CNN
F 1 "PWR_FLAG" H 950 5974 50  0000 C CNN
F 2 "" H 950 5800 50  0001 C CNN
F 3 "~" H 950 5800 50  0001 C CNN
	1    950  5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	950  5800 1250 5800
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 5B96136E
P 1250 5200
F 0 "#FLG0102" H 1250 5275 50  0001 C CNN
F 1 "PWR_FLAG" H 1250 5374 50  0000 C CNN
F 2 "" H 1250 5200 50  0001 C CNN
F 3 "~" H 1250 5200 50  0001 C CNN
	1    1250 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 5200 1450 5200
Connection ~ 1450 5200
NoConn ~ 4150 2000
NoConn ~ 4150 2900
$EndSCHEMATC
