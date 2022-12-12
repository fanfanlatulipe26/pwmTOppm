# pwmTOppm

### Generation of a PPM signal for Flysky receivers that dont have PPM output.

Runs on  **Arduino Promini 5v 16Mhz** Atmega328 (or Atmega128)  

Tested with “old” Flysky AFHDS 8 channels **FS-R9B** receiver (same as **Turnigy 9X8C**), with 6 channels **FS-R6**, and with 3 channels **FS-GR3**  ...

**Auto adaptation to the number of channels.** No changes in the code needed for 3, 6 or 8 channels receivers

This converter fits nicely in the enclosure of the FS-R9B receiver  
<img src="/img/realisation.jpg" width="300">

### Warning:
The order of the PPM pulses will not be the order of the ports, but the order of the PWM pulses as they appear in the time (see pictures at the end):
 - for 8 channels receivers:  ch1, ch3, ch2, ch4, ch5, ch6, ch7, ch8  
 - for 6 channels receivers:  ch1, ch2, ch3, ch4, ch5, ch6  
 - for 3 channels receivers:  ch2, ch1, ch3  

### Wiring:

 | channel  | pin  |   |
| ------------ | ------------ |------|
|  channel 1| pin D2 |  |
|  channel 2| pin D3 |  |
|  channel 3| pin D4 || 
|  channel 4| pin D5 |  (or unconnected if 3 channels receiver) |
|  channel 5| pin D6 |  (or unconnected if 3 channels receiver) |
|  channel 6| pin D7 |  (or unconnected if 3 channels receiver) |
|  channel 7| pin D8 | (or unconnected if 3 or 6 channels receiver) |
|  channel 8| pin D9 |  (or unconnected if 3 or 6 channels receiver) |
|   |   | |
| **PPM output**  |  pin D12 |  | 
|||

The conversion is pretty acurate as can be seen in the .logicdata capture files provided in the captureFile directory.  
They can be explored with Saleae Logic Software. 

<img src="/img/8channels.JPG" >  
<img src="/img/6channels.JPG" >  
<img src="/img/3channels.JPG" >  
