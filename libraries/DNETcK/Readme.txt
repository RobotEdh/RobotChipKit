How to install the DNETcK/DWIFIcK libraries:

1.  You must be using mpide-0023-windows-20111221 or newer.
2.  Identify where your 3rd party libraries directory is. In MPIDE look under File->Preferences, and then look at the Sketchbook location. 
    Under the Sketchbook location directory there should be a libraries subdirectory, this is where you will install DNETcK and DWIFIcK. If the libraries
    subdirectory does not exist, then create it; make sure you create it as "libraries".
3.  Close MPIDE, you do not what it running.
4.  Navigate to your <Sketchbook location>/libraries directory.
5.  If you already have a DNETcK and/or DWIFIcK subdirectory, back them up and remove (delete) DNETcK AND DWIFIcK from the libraries directory. 
    This is extremely important as the DNETcK and DWIFIcK libraries will both be installed and must be of a consistent build! Any exsiting DNETcK
    sketch that you have should build and run as expected with this updated DNETcK library.
6.  Copy this .zip file to your <Sketchbook location>/libraries directory. 
7.  Unzip the .zip file "in place", both DNETcK and DWIFIcK subdirectories should be created. (you can remove the .zip file once unzipped).
8.  In addition to the DNETcK and DWIFIcK subdirectories being created, you will see some network hardware libraries created; NetworkShield
    PmodNIC, PmodNIC100, and WiFiShieldOrPmodWiFi.
9.  Start MPIDE, you should see under sketch->import library the DNETcK and DWIFIcK and network hardware libraries. Also under File->examples, 
    you should see the DNETcK and DWIFIcK examples.
