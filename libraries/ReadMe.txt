To install:

1. Start MPIDE
2. Look under File->Preferences and take note of your Sketchbook Location
3. Shutdown MPIDE
4. under your Sketchbook directory create a subdirectory called "libraries"; this directory may already exist.
5. Unzip the ChipKITNetworkShield in the libraries directory.
6. You should have 8 subdirectories added (chipKITEthernet, chipKITUSBDevice, chipKITUSBHost, chipKITUSBHIDHost, chipKITUSBMSDHost, chipKITMDDFS, and chipKITCAN, DNETck). These are the chipKIT Network and USB libraries.
7. Restart MPIDE
8. You should see the new libraries under Sketch->Import LIbrary, under Contributed.
9. Also you should see under File->Examples the chipKIT library examples directories.


Fix issue Math lib ATAN2:
-------------------------
Go into your MPIDE install directory\hardware\pic32\boards.txt
There are groupings of settings. Each group is for a different board type. Find the board type that you're using (say uno32), and find the two lines

uno_pic32.compiler.c.flags=-O2::-c::-mno-smart-io::-w::-ffunction-sections::-fdata-sections::-G1024::-g::-mdebugger::-Wcast-align
uno_pic32.compiler.cpp.flags=-O2::-c::-mno-smart-io::-w::-fno-exceptions::-ffunction-sections::-fdata-sections::-G1024::-g::-mdebugger::-Wcast-align

and add 
::-fno-short-double

to the end of each of those lines. (obviously, the beginning of the line will be different for each board type)

Then close all open MPIDE windows, and start it up again.