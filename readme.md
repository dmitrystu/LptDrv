WinRing0 wrapper for Postal2 ISP (LPT bit-banding)
Rerplace Micronas LptDrv for x64 and x32 systems
Tested on WinXP SP3 x32 & Win7 SP1 x64
Initial release

Featuries
    Integrated digitally signed WinRing0 drivers for both x32 & x64
    Automatic driver install
    Supports only Postal 2 bit-banding. No integrated I2C/SPI
    Automatic rises host process priority to realtime when port opened

Port setting for Postal 2 ISP
    ports can be set using configuration file (LptDrv.ini)
    If no configuration file exists default will de created

Targets
    all     -> creates pkg and pkg-src
    pkg     -> creates zipped LptDrv.dll
    pkg-src -> creates zipped sources
    release -> creates LptDrv.dll

How to use
    Copy LptDrv.dll to Postal2 folder.
    Run Postal2
    Enjoy


[3.0.1]
    + added configuration file support
    + added configurable extra delay

[3.0.0]
Initial release
