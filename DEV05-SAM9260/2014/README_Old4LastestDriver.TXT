
set bootargs 'loglevel=7 console=ttyS0,115200 initrd=0x21100040,5242880 root=/dev/ram0 rw time'
--------------------------------------------------------------------------------------------------------------------


~ >: cat /proc/ioctl 
Current driver support ioctl command:
========================== Commone ioctl =============================
Enable/Disable Debug    : 1074005249
Get driver version      : 2147747074

=========================== LED driver ==============================
Turn LED on             : 1305
Turn LED off            : 1304
Turn LED blink          : 1074005274
Turn all LED on         : 1308
Turn all LED off        : 1307
Turn all LED blink      : 1074005277

=========================== Beep driver ==============================
Turn buzzer on          : 1303
Turn buzzer off         : 1302
Set buzzer frequency    : 1074005272

=================== Hardware status Detect driver ====================
Detect WIFI module ID   : 1299
Detect SD/ETH0/ETH1/WIFI: 1298

=========================== GPRS driver ==============================
GPRS power on           : 1074005297
GPRS power off          : 1328
GPRS(3G) reset          : 1074005298
GPRS(3G) power monitor  : 1331
Check SIM door          : 1074005302
Set work SIM slot       : 1074005301
Get working SIM slot    : 1332
Set GPRS DTR pin        : 1074005303
Set GPRS RTS pin        : 1074005304
