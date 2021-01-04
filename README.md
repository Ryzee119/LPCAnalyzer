# LPCAnalyzer Plugin

LPC analyer plugin for the [Kingst](http://www.qdkingst.com/en) branded Logic Analysers. This was developed using their SDK. <br/>  <br/> **Minimum Sample Rate of 200Mhz is required**

This is considered **BETA** and the code has been hastily put together. All results may not be accurate. So use at your own risk.


## Let Me Get It!
Download a pre-build x64 DLL built for Win10 from the `Releases` section.

## What It Can Do
I tried to implement as many features as possible within reason. This has been designed around the Original Xbox LPC implementation so is limited to Memory and IO Read and Writes. **LFRAME signal is required though.**  
Termination of broken cycles is partially handled (Does not throw an error, but will recover for next frame)

The following features are implemented in a basic form:
1. Proper settings screen. <br/> ![enter image description here](https://i.imgur.com/trqbbkM.png)
2. Samples the LADs at the centre of the lower clock pulse (Way better accuracy in my experience) 
3. Descriptive text bubbles. <br/> ![enter image description here](https://i.imgur.com/b7Cvm7T.png)
4. Combines Address nibbles accordingly in text bubbles as per the LPC spec. <br/> ![enter image description here](https://i.imgur.com/eiPMQBT.png)
5. The tabular view ('Decoded Results') along the side populates fully <br/>  ![enter image description here](https://i.imgur.com/r9bEIv7.png)
6. Can export to a CSV. This is laid out to one transaction per line, comma separate by nibble with a timestamp of the first LFRAME in that transaction.  <br/> ![enter image description here](https://i.imgur.com/G2VxiTD.png)
7. Some basic error handling (Sync error and LFRAME error)
8. An inbuilt simulator for debugging (Hit record with your Logic Analyser Disconnected)


## Build Instructions
1. Download the Kingst SDK from [http://www.qdkingst.com/en](http://www.qdkingst.com/en) and extract to a working folder.
2. Clone this repository into the SDK working folder.

### Windows
1. Setup Visual Studio with Windows SDK and Platform Toolset (I used the latest rev of SDK 10.0, Platform Toolset v142)
2. Open `LpcAnalyzer.vcxproj` in the vs2019 folder.
3. If you point the Debugger to the KingstVIS.exe and compile the DLL as debug, you can step through the code. (You need to copy the debug .DLL to the Kingst analyser folder.)
4. Set build properties to Release and x64/x86. This has been tested mainly on x64 build.
5. A DLL file will be generated which should be copied to the Kingst Analyzer folder.

### Linux 
1. Browse the the `Linux` folder of this repo and type `make`. 
2. Copy the `libLPC.so` copied to the KingstVIZ software `Analyzer` folder. 

By Ryzee119

Contributor b1ghamm3r
