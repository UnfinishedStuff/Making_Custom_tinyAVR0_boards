# Making_Custom_tinyAVR0_boards

This repo describes setting up and programming Microchip's tinyAVR0 boards using the UPDI interface.  A the time of writing all of this was tested with ATtiny1607 chips, but in theory should work with all tinyAVR0 chips (ATtiny804/806/807/1604/1606/1607 and ATtiny202/204/402/404/406).  The instructions are largely related to Windows because that's what I've used.

# Setting up the programmer hardware: adapting USB-UART adapters

There are expensive purpose-built programmers, but you can also program UPDI-interface chips using a simple USB-UART adapter.  I originally came across this via Mr Aardvark's [pyupdi repo](https://github.com/mraardvark/pyupdi/), however that has since been incorporated into Microchip's [pymcuprog](https://github.com/microchip-pic-avr-tools/pymcuprog) repo.

In short, get a USB-UART adapter.  Connect the RX pin to the target microcontroller's UPDI pin, and then also connect the UART adapter's TX pin to the target's UPDI pin through a 1K resistor.

A schematic showing this setup with a cp2102n USB-UART adapter and an ATtiny1607 with an MCP1700T voltage regulator is shown below.  Note: while this is the setup I've used, it doesn't follow some of the component datasheet recommendations, e.g. for capacitors around the MCP1700T.  If you're having problems make sure you're following the specifications of the part you're using.  Also, [other people's experiences](https://github.com/mraardvark/pyupdi/issues/75) suggest that UPDI is very sensitive to other devices on the bus.  If you're using a device with LEDs driven directly by the TX/RX pins you may need to desolder them so that there is nothing else on the bus besides the 1K resistor.


![Schematic of a cp2102n USB-UART adapter set up to program an ATtiny1607 over UPDI](https://raw.githubusercontent.com/UnfinishedStuff/Making_Custom_tinyAVR0_boards/main/images/cp2102n_UPDI_programmer_ATtiny1607.jpg)

# Setting up the software: pymcuprog

Next, prepare the software.  You will need [Python3](https://www.python.org/) and [pymcuprog](https://github.com/microchip-pic-avr-tools/pymcuprog).  As per pymcuprog's documentation, open a terminal (in Windows, click on the Start menu and type `CMD`.  A black icon with `C:\__` should appear, click that) and run `pip install pymcuprog`.  This should install and then confirm the installation of pymcuprog.  That's all the setup you need for the programmer.

# Blinky

Finally, we need something to upload.  I've been using [Microchip Studio for AVR and SAM Devices ](https://www.microchip.com/en-us/development-tools-tools-and-software/microchip-studio-for-avr-and-sam-devices#Downloads) simply because it is described in the newbie guides for ATtiny0 chips, but if you prefer setting up your own toolchain then go right ahead.

If you're using MC Studio, go to File > New Project.  In the window which pops up, select `GCC C executable`, and use the `Name` box to give the program a name (I'll use `ATtiny1607_Blinky` in this example).  Select OK.  This should open a window asking you to select your device, with a list of possible chips displayed.  In the text input box in the top right type `ATtiny1607` and this should narrow the list down to just one option.  Select the ATtiny1607, and click on OK.  This should open a code window.  The code I've used can be found in [ATtiny1607_Blinky](https://github.com/UnfinishedStuff/Making_Custom_tinyAVR0_boards/blob/main/ATtiny1607_Blinky.c).  On your keyboard hit `F7`.  This should build the program, and you should see some debug info and finally `Build: 1 succeeded or up-to-date, 0 failed, 0 skipped` in the Output window at the bottom of MC Studio.

We now need the compiled program file.  Take a look a few lines up in the Output window: one line should say something like `Target "Build" in file "C:\Program Files (x86)\Atmel\Studio\7.0\Vs\Avr.common.targets" from project "C:\Users\[Username]\Documents\Atmel Studio\7.0\ATtiny1607_Blinky\ATtiny1607_Blinky\ATtiny1607_Blinky.cproj" (entry point):`.  This tells you that it output the files to `C:\Users\[Username]\Documents\Atmel Studio\7.0\ATtiny1607_Blinky\ATtiny1607_Blinky`.  Navigate to that directory, and you should see a folder called `Debug`.  There should be a few different files there including one called `ATtiny1607_Blinky.hex`.  That's the one we need.  Either make a note of this file's location, or move it somewhere easier to remember (e.g. the Desktop).

# Programming the ATtiny1607 with pymcuprog

Finally, let's program the chip using pymcuprog.  Open a terminal again.  If you ever need help with the program run `pymcuprog --help`.

First, we need the COM port which your UART adapter is on.  On Windows, click on the Start menu and type Device Manager, and launch the Device Manager.  In the list which appears, click the arrow beside the icon next to `Ports (COM & LPT)`.  That should list your serial devices, and each should have `(COMxx)` in the name, where `xx` is a number.  That is your COM port, e.g. `COM47`.  Make a note of the port for the UART device you're using.  (If you have many and you're not sure which one to use, you may need to unplug the adapter you're using and see which one disappears from the list, then plug it back in again).

First, lets check that we can detect your microcontroller.  In the terminal, run `pymcuprog ping -d attiny1607 -u comxx -t uart`, replacing `comxx` with your COM port.

* `pymcuprog` invokes the `pymcuprog` program which knows how to talk to UPDI chips.
* `ping` tells `pymcuprog` that we just want to check if we can communicate with the chip.
* `-d attiny1607` tells `pymcuprog` that we want to talk to an attiny1607.  If you're working with a different chip, run the same command but change `attiny1607` to some random text.  It should throw an error, and then list valid chip names.
* `-u comxx` tells `pymcuprog` to use the COMxx serial port to talk to the chip.   
* `-t uart` tells `pymcuprog` that we're using a UART adapter to do the programming.

The data you get back should look like this (your response may or may not be the same):

```
Pinging device...
Ping response: 1E9423
Done.
```

Now, before we write the ATtiny_Blinky program to the board you **must** erase the chip first.  If you don't then nothing will happen when you program the board.  Run:

`pymcuprog erase -d attiny1607 -u com47 -t uart`

And your output should look like:

```
Pinging device...
Ping response: 1E9423
Chip/Bulk erase:
- Memory type eeprom is conditionally erased (depending upon EESAVE fuse setting)
- Memory type lockbits is always erased
- Memory type flash is always erased
Erased.
Done.
```

Now program the device:

`pymcuprog write -d attiny1607 -u com47 -t uart -f /path/to/ATtiny1607_Blinky.hex --verify`

Your output should look like:

```
Pinging device...
Ping response: 1E9423
Writing from hex file...
Writing flash...
Verifying flash...
OK
Done.
```

Now, if you hook up an LED with resistor to any of the PORTB pins (`PBxx`-labelled pins, physical pins 9-16 on a QFN-24 ATtiny1607) you should see the LED blinking.  Use this same process to upload any other programs to your tinyAVR0 device.

# A few other notes

The [pymcuprog documentation](https://pypi.org/project/pymcuprog/) has a few other ntoes which may be relevant to programming UPDI devices, thought I've not tried them yet.

First, there is a switch described as `-C to erase and unlock a locked device (UPDI only)`.  I've not locked the memory my device, but if you do or happen to get one which is, this may be useful.

In addition to this, I've seen a few people use the `-i updi` option.  The documentation describes this as `-i INTERFACE to specify the target communication interface. Optional`.  I've no idea what it does and I've not needed to use it, presumably because it is marked as `optional` ?

Some tinyAVR devices have a "user row" of memory which can be written.  `pymcuprog` has a flag for doing this (`-U to write user row values when the device is locked (UPDI only)`).  Again, I've not tried it yet.

