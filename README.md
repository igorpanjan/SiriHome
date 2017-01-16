# SiriHome
Everything at home, should be voice controled

My home is full of devices from TVs, ACs, IPTV boxes, light switches, etc. and I have to push gazillion buttons to make them work. We live in 2017 and it should be fairly easy to control everything by voice. So this is why I'm working on a solution to control everything in my home through Apple Siri. Secondary goal I have is to have a unified remote controll for all my devices - this feature is especially handy for guests.

Before I went down DIY road, I looked at smart home market, what can I actually buy to make it work. At the end I came to realize, that there simply aren't any manufacturers, that offer standardized solution. Whether they have their own closed standards, apps that don't work, or I would have to spend a lot of money on a compromise. So the only way to actually make it work is DIY way. And it is also price / performance efficient - I have spent in a range of 100 EUR for all materials, that make every devices work the way I want them to work. In comparison - I would have to spend couple of 1000s to make it half work - just Logitech Harmony is 350 EUR and it fixes 10% of problems I want to solve. 


![Home Network Topography](https://github.com/igorpanjan/SiriHome/blob/master/Home%20Network%20Topography_001.png?raw=true)

## Voice controll / Unified remote controll target devices or features

* TV: Samsung 48UE6400 series
* IPTV Setup box: Arris VIP1113 series
* HVAC: Mitscubishi MFZ-KA series
* Lighting and window shading: Gira KNX devices connected to Loxone Miniserver
* Measuring temperature and humidity inside a room

![Hardware and Software](https://github.com/igorpanjan/SiriHome/blob/master/Home%20Network%20Topography_002.png)

## Control your devices

### Samsung TV
I own 48UE6400 model. With TV I need to control power, volume, source (HDMI1, HMDI2, etc.) and key position. I have explored LibCEC and IR control. LibCEC was a mixed bag - some commands work (TV ON/Off), most of them did not. I could also go with RS232 via audio headphones port on the back of TV, but it would be an overkill, since I needed no feedback on a status of device (TV is usually not a device you interact remotly, because you sit in front of it). So IR control should be ok. Please check TV folder for more details.

#### Hardware
* RaspberryPI 2/3. Every board with Linux should work.
* Small breadboard
* IR diode (944nm)
* BC547 or similar transistor

#### Software
* LIRC - drive IR diode
* NodeRed - for my Unified Remote App
* [Homebridge-System-Plugin](https://www.npmjs.com/package/homebridge-system-plugin) - I am using Homebridge for Siri voice controll


### IPTV Setup box Arris VIP1113
Still work in progress. I am waitng for IR sensor, to capture IR signals. Setup box will also be controlled via the same RaspberryPi and IR diode as TV. The only thing that is going to be different is Lirc config file, that drives setup box. 


### Mitsubishi HVAC MFZ-KA
Mitsubishi falls in the same category as Samsung TVs - hard to controll, beacuse it is a closed system. But in last couple of months people hacked IR codes, serial protocol and a part of REST protocol. In the first phase to control Mitsubishi HVAC I went with ESP8266 and IR diode. It is funny actually, because WiFi modules from Mitsubishi cost 80 EURs per module (I own 2) and I have control for 4 EURs. It works without any problems, but I do not have any feedback on a status of the device. So if I want to start my Mitsubishi from work, I do not really know, did it start or not. That is why I am upgrading IR control to serial control. Plase check HVAC folder for details.


#### Hardware
* ESP8266 12e Node MCU - this ESPs come with on board USB to serial chip, so they are really easy to interact with
* IR diode (944nm)
* Small breadboard
* BC547 or similar transistor


#### Software
* custom firmware on ESP - it is basically a web server, that accepts custom URLs
* I am using Arduino IDE to program ESPs
* Homebridge HTTP plugin - there are a lot of HTTP plugins, so pick yours


### Lighting and window control and shading
When I moved into my appartment it already had a Gira KNX switches inside. Gira offers some server solutions for remote control, but their solutions are pricy and functionalities are poor in comparison to modern competitors. KNX is a great, bullet proof protocol, but it does not come cheap. I went with Loxone Miniserver as a central hub for KNX devices (lighting, window shading, windows open/close) that exposes KNX devices to the network and Homebridge. Please check Lighting folder for details.


#### Hardware
* Loxone Miniserver - this guys make affordable home server. It even comes in a wireless version. So if you are in position, that you have to combine legacy systems (Gira and similar) with new solutions (Apple, Samsung, Domotics,...) Homeserver is my top choice. [Loxone](https://www.loxone.com/enen/).
* Just to be clear - I am using Loxone only as KNX to Network gateway. Once you expose KNX devices as URL, you can controll them in any app of your choice.


#### Software
* Loxone has a great configuration tool [Loxone Config](https://www.loxone.com/enen/products/loxone-config/). It only runs on Windows, so some Parallels or VMWare could come in handy, if you are on a Mac.
* Homebridge HTTP plugin

### Measuring temperature and humidity
I went with DHT22 temperature and humidity sensor and Particle Photon. You can easily expose temperature and humidity to the network and Homebridge, with just a couple of lines of code. Plase check TempHum folder for details.

#### Hardware
* [Particle Photon](https://www.particle.io/) - just love these guys.
* DMT22 with on-board resistor. You can get them on AliExpress.

#### Software
* Particle IDE - I am using local IDE. It is an Atom IDE with Particle customization. Super easy to work with, simple UI and UX.
* Particle Cloud - I am exposing temperature and humidity into cloud, so that I can access them with variuos services. 
* [Homebridge-Particle](https://www.npmjs.com/package/homebridge-particle) plugin


## Siri voice controll VS. Unified remote app
Just to be clear about the difference between Homebridge and NodeRed. Homebridge is a solution, that enables you to controll your devices through Siri - so that you have voice command. NodeRed is a service, where you can glue different services, IoT devices into centrilized environment and create new functionalities, in my example web app with centrilised control for all my devices. Both solutions can run on RaspberryPI at the same time. 


# Refferences
It took me some time to figure everything out, the way my Siri control is usable and it works. And all this work wouldn't be possible without people sharing their work and creating awsome stuff. I gathered bits and pieces of information all around the Internet, but these guys deserve extra credit and a big **thank you**! 


* [nfarina](https://github.com/nfarina/homebridge)
* [khaost](https://github.com/KhaosT/HAP-NodeJS)
* [einarzt ](https://www.npmjs.com/package/homebridge-custom-http)
* [itsfrosty](https://www.npmjs.com/package/homebridge-system-plugin)
* Mitsubishi HVAC control is based on Vincent Cruvellier's work [Ref1](https://www.analysir.com/blog/2015/01/06/reverse-engineering-mitsubishi-ac-infrared-protocol/) and [Ref2](https://github.com/r45635/HVAC-IR-Control)
* ESP web server parsing multiple URL parameters is based on Nuno Santos's [work](https://techtutorialsx.wordpress.com/2016/10/22/esp8266-webserver-getting-query-parameters/)
