#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial 
import time 
import MySQLdb

db = MySQLdb.connect("localhost", "edh", "edh", "CHIPKIT")
curs=db.cursor()

add_data = ("INSERT INTO raw_data "
               "(source, type, value) "
               "VALUES (%s,%s,%s)")
              
       
ser = serial.Serial('/dev/ttyUSB0', 9600)
time.sleep(4)           #Wait ChipKit ready.

print("Start")
while True:             # Infinite loop
    line = ser.readline()
    
    if line[1] == "#":
        print(line[1]) # #
        source = ord(line[0]) # source
        type = ord(line[2])   # type 
        value = line[3:line.index(">")]  # value ended by > 
        data_raw = (source, type ,value)
        curs.execute(add_data, data_raw)
        db.commit()
    else:    
        print(line) # not a measurement message

