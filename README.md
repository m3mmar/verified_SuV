# SμV - the Security MicroVisor: A formally-verified software-based security architecture for the Internet of Things

*** This file describes the verified source code of the Security MicroVisor along with Remote Attestation(RA) technique built atop of it. For the moment, the implementation supports only AVR architecture. Work is going on to support other architectures along with different security services such as secure code update and secure erasure. ***


## Folder details
 

- **./apps**: contains the various use cases that are implemented on the top of the Security MicroVisor along with the RA protocol implemented. 
	./apps/remote_attest : represents a formally-verified software-based RA technique. The attached python script file represents the verifier code that should run on any python3-enabled envirnoment. 
- **./core**: contains the core MicroVisor files + crypto folder.
- **./full_verified**: contains a formally verified version of the Security MicroVisor (in terms of memory-safety and absence of crashes) + the Mac OS version of VeriFast. We wrapped all related files in one .c file named: verified-smv-ra.c, to simplify the task of verifying the formally verified properties. 


****************************************

## Running VeriFast
Assuming that you are using Mac OS (If not, please download the right version of VeriFast:https://github.com/verifast/verifast):
	- Move to the bin folder inside verifast folder: ./full_verified/verifast/bin
	- Run the following command to check the correct linking of the various software modules + verification of the software itself
	     ./verifast  - prover z3v4.5 -target IP16 -allow_assume -shared ../../avr.o ../../bootloader_progmem.o ../../verified-smv-ra.c
	- If you are interested in GUI, run the following command: ./vfide -prover z3v4.5 -target IP16 ../../verified-smv-ra.c

**************************************

## Running RA: 
- change directory to ./apps/remote_attest
	- Adjust the parameters in the Makefile (e.g. MCU, Frequency, flash, etc.)
	- Run: make microvisor.hex from command line
	- Run: make fuse4 from command line
	- Run: make flash from command line
	- Then, run the python script file verifier.py that serves as a verifier, attaching as parameters the hex file and the serial port of the attached MCU.
***************************************

** Please note that this implementation is only applicable for AVR-based MCUs.

** For further info. Please contact: mahmoud.ammar@cs.kuleuven.be






