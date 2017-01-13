# SiriHome
Everything at home, should be voice commanded

My home is full of devices from TVs, ACs, IPTV boxes, light switches, etc. and I have to push gazillion buttons to make them work. We live in 2017 and it should be fairly easy to control everything by voice. So this is why I'm working on a solution to control everything in my home through Apple Siri. Secondary goal I have is to have a unified remote controll for all my devices - this feature is especially handy for guests.

Before I went down DIY road, I looked at smart home market, what can I actually buy to make it work. At the end I came to realize, that there simply aren't any manufacturers, that offer standardized solution. Whether they have their own closed standards, apps that don't work, or I would have to spend a lot of money on a compromise. So the only way to actually make it work is DIY way. And it is also price / performance efficient - I have spent in a range of 100 EUR for all materials, that make every devices work the way I want them to work. In comparison - I would have to spend couple of 1000s to make it half work - just Logitech Harmony is 350 EUR and it fixes 10% of problems I want to solve.  


## Voice controll / Unified remote controll target devices or features

* TV: Samsung 48UE6400 series
* IPTV Setup box: Arris VIP1113 series
* HVAC: Mitscubishi MFZ-KA series
* Lighting and window shading: Gira KNX devices connected to Loxone Miniserver
* Measuring temperature and humidity inside a room


## Electronic devices
* RaspberyPI 2 or 3 or similar small computer as long as it runs Linux
* Particle Photon - particle.io
* ESP8266 12e Node MCU - find it on AliExpress
* IR leds
* DHT22 temperature and humidity sensor
* Resistor pack
* BC547b transistors
* Breadboards
* breadboard wire pack - go for all combinations - male/male, male/female, female/female
* Mini USB cables and adapters - more is better ;)
* Loxone Miniserver


## Software
* Homebridge: it is a database, that supports Apple HomeKit protocol and it talks with Siri - https://github.com/nfarina/homebridge
* Homebridge plugins: plugins give Homebridge functionality and interaction with an outside world - our IoT devices - https://www.npmjs.com/browse/keyword/homebridge-plugin
* NodeRed: JavaScript LEGO for IoT devices. - https://nodered.org/
* Arduino IDE: you will need this to program ESPs - https://www.arduino.cc/
* Particle IDE: you will need this to program Photons - go for Local IDE - https://www.particle.io/products/development-tools/particle-local-ide
* Etcher: burn disk images to SD card, the easy way - https://etcher.io/

## Control your devices

### TV Samsung 48UE6400
With TV I need to control power, volume, source (HDMI1, HMDI2, etc.) and key position. I have explored LibCEC and IR control. LibCEC was a mixed bag - some commands work (TV ON/Off), most of them did not. I could also go with RS232 via audio headphones port on the back of TV, but it would be an overkill, since I needed no feedback on a status of device (TV is usually not a device you interact remotly, because you sit in front of it). So IR control should be ok.

I went with RaspberryPI 2, LIRC, a transistor and IR diode. It works like a charm. Please check TV folder for details.


### IPTV Setup box Arris VIP1113
Work in progress. I am waitng for IR sensor, to capture IR signal. Setup box will also be controlled via the same RaspberryPi and IR diode as TV. The only thing that is going to be different is Lirc config file, that drives setup box. 


### Mitsubishi HVAC MFZ-KA
Mitsubishi falls in the same category as Samsung TVs - hard to controll, beacuse it is a closed system. But in last couple of months people hacked IR codes, serial protocol and a part of REST protocol. In the first phase to control Mitsubishi HVAC I went with ESP8266 and IR diode. It is funny actually, because WiFi modules from Mitsubishi cost 80 EURs per module and I have control for 4 EURs. It works without any problems, but I do not have any feedback on a status of the device. So if I want to start my Mitsubishi from work, I do not really know, did it start or not. That is why I am upgrading IR control to serial control. Plase check HVAC folder for details. 


### Lighting and window control and shading
When I moved into my appartment it already had a Gira KNX switches inside. Gira offers some server solutions for remote control, but their solutions are pricy and functionalities are poor in comparison to modern competitors. KNX is a great, bullet proof protocol, but it does not come cheap. I went with Loxone Miniserver as a central hub for KNX devices (lighting, window shading, windows open/close) that exposes KNX devices to the network and Homebridge. Please check Lighting folder for details.


### Measuring temperature and humidity
I went with DHT22 temperature and humidity sensor and Particle Photon. You can easily expose temperature and humidity to the network and Homebridge, with just a couple of lines of code. Plase check TempHum folder for details.
