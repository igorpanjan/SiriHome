
# RaspberryPI 2 LIRC setup guide
I really didn't want to make any step by step tutorial, but setting up LIRC was a project, so I decided to share my experience, what actually worked for me.
My RaspberryPI 2 runs Raspbian Jessie. I controll my IR diode via BC547 transistor and GPIO_PIN = 22. Look at Fritzing file and pictures.

First we install LIRC: 
```
sudo apt-get install lirc
```

After the installation finishes, we have to setup lirc_rpi kernel module. We will edit /etc/modules:
```
sudo nano /etc/modules
```

Add these two lines:
```
lirc_dev
lirc_rpi gpio_out_pin=22
```

Now, let's edit Lirc hardware config file:
```
sudo nano /etc/lirc/hardware.conf
```

and change it, so that it matches the following setup:

```
LIRCD_ARGS="--uinput"
LOAD_MODULES=true
DRIVER="default"
DEVICE="/dev/lirc0"
MODULES="lirc_rpi"
LIRCD_CONF=""
LIRCMD_CONF=""
```


We also have to add couple of lines to /boot/config.txt:
```
sudo nano /boot/config.txt
```

and change it accordingly:
```
# Uncomment this to enable the lirc-rpi module
dtoverlay=lirc-rpi,gpio_out_pin=22
```

Reboot RaspberryPi:
```
sudo reboot
```

We have configured LIRC, but now we have to find a suitable remote configuration file or library, that LIRC uses to controll the IR led. You can find configuration files here: [LIRC Remotes](http://lirc.sourceforge.net/remotes/)

So start new file in /etc/lirc/lircd.conf:
```
nano /etc/lirc/lircd.conf
```

and paste the following text into it (You can control your Samsung UE series with BN59-01054A file from LIRC Remotes library):
```
# this config file was automatically generated
# using lirc-0.9.0-pre1(default) on Sun Dec 13 18:26:53 2015
#
# contributed by Rikels
#
# brand:                       Samsung
# model no. of remote control: BN59-01054A
# devices being controlled by this remote: UE46C7700WS (46" SmartTV)
#
    # +------------------------+
    # |                        |
    # |  POWER   SOURCE  [   ] | KEY_POWER SOURCE   []=on/off button for iluminating keys
    # |                        |
    # | (  1  ) (  2  ) (  3  )| KEY_1 KEY_2 KEY_3
    # |                        |
    # | (  4  ) (  5  ) (  6  )| KEY_4 KEY_5 KEY_6
    # |                        |
    # | (  7  ) (  8  ) (  9  )| KEY_7 KEY_8 KEY_9
    # |                        |
    # |TTX/MIX (  0  ) (PRE-CH)| KEY_TEXT KEY_0 KEY_PREVIOUS
    # |                        |
    # |  VOLUP   MUTE  PAGEUP  | KEY_VOLUMEUP KEY_MUTE KEY_CHANNELUP
    # |                        |
    # |  VOLDWN CHLIST PAGEDWN | KEY_VOLUMEDOWN CHLIST KEY_CHANNELDOWN
    # |                        |
    # | CONTENT  MENU  GUIDE   | CONTENT KEY_MENU KEY_EPG
    # |                        |
    # | TOOLS     ▲     INFO   | TOOLS KEY_UP KEY_INFO
    # |                        |
    # |     ◄     OK     ►     | KEY_LEFT KEY_OK KEY_RIGHT
    # |                        |
    # |  RETURN   ▼      EXIT  | KEY_BACK KEY_DOWN KEY_EXIT
    # |                        |
    # |   A     B     C    D   | KEY_A KEY_B KEY_C KEY_D
    # |                        |
    # | MEDIA.P  INTERNET   3D | MEDIAP INTERNET 3D
    # |            @TV         |
    # |                        |
    # |   AD   P.SIZE   SUBT.  | AD PSIZE KEY_SUBTITLE
    # |                        |
    # |   <<      ||      >>   | KEY_BACKWARD KEY_PAUSE KEY_FORWARD
    # |                        |
    # |   REC     ►      STOP  | KEY_RECORD KEY_PLAY KEY_STOP
    # |                        |
    # |        SAMSUNG         |
    # +------------------------+

begin remote

  name  samsung
  bits           16
  flags SPACE_ENC|CONST_LENGTH
  eps            30
  aeps          100

  header       4589  4511
  one           591  1664
  zero          591   536
  ptrail        591
  pre_data_bits   16
  pre_data       0xE0E0
  gap          108682
  toggle_bit_mask 0x0

      begin codes
          KEY_POWER                0x40BF # Was:POWER
          SOURCE               0x807F # Was:SOURCE
          KEY_1                    0x20DF # Was:1
          KEY_2                    0xA05F # Was:2
          KEY_3                    0x609F # Was:3
          KEY_4                    0x10EF # Was:4
          KEY_5                    0x906F # Was:5
          KEY_6                    0x50AF # Was:6
          KEY_7                    0x30CF # Was:7
          KEY_8                    0xB04F # Was:8
          KEY_9                    0x708F # Was:9
          KEY_0                    0x8877 # Was:0
          KEY_TEXT                  0x34CB # Was:TTX/MIX
          KEY_PREVIOUS               0xC837 # Was:PRE-CH
          KEY_VOLUMEUP             0xE01F # Was:VOL+
          KEY_VOLUMEDOWN           0xD02F # Was:VOL-
          KEY_MUTE                 0xF00F # Was:MUTE
          CHLIST               0xD629 # Was:CHLIST
          KEY_CHANNELUP            0x48B7 # Was:CHANNEL+
          KEY_CHANNELDOWN          0x08F7 # Was:CHANNEL-
          CONTENT              0x9E61 # Was:CONTENT
          KEY_MENU                 0x58A7 # Was:MENU
          KEY_EPG                0xF20D # Was:GUIDE
          TOOLS                0xD22D # Was:TOOLS
          KEY_INFO                 0xF807 # Was:INFO
          KEY_BACK               0x1AE5 # Was:RETURN
          KEY_EXIT                 0xB44B # Was:EXIT
          KEY_UP                   0x06F9 # Was:UP
          KEY_DOWN                 0x8679 # Was:DOWN
          KEY_LEFT                 0xA659 # Was:LEFT
          KEY_RIGHT                0x46B9 # Was:RIGHT
          KEY_OK                   0x16E9 # Was:OK
          KEY_A                  0x36C9 # Was:RED(A)
          KEY_B                0x28D7 # Was:GREEN(B)
          KEY_C               0xA857 # Was:YELLOW(C)
          KEY_D                 0x6897 # Was:BLUE(D)
          MEDIAP               0x31CE # Was:MEDIA.P
          INTERNET             0xC936 # Was:INTERNET@TV
          3D                   0xF906 # Was:3D
          AD                   0xE41B # Was:AD
          PSIZE                0x7C83 # Was:P.SIZE
          KEY_SUBTITLE                 0xA45B # Was:SUBT.
          KEY_BACKWARD             0xA25D # Was:BACKWARD
          KEY_FORWARD              0x12ED # Was:FORWARD
          KEY_PAUSE                0x52AD # Was:PAUSE
          KEY_PLAY                 0xE21D # Was:PLAY
          KEY_STOP                 0x629D # Was:STOP
          KEY_RECORD                  0x926D # Was:REC
      end codes

end remote
```

So all we have to do now is to give it a test ride:

```
sudo irsend SEND_ONCE samsung KEY_VOLUMEUP
```

Now you can use these commands in NodeRed (Exec Node) or Homebridge (Homebridge-System-Plugin). 
