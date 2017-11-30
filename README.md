# Presage based input predictor for the Sailfish OS
I am trying to create an alternative word predictor for the Sailfish OS based on presage.
Code still wip, working on the setting up the infrastructure part. 

Brief introduction about the components:
- The presage is an open source library for word prediction this package compiles without trouble for SFOS. You can find it here, I have only created an rpm spec file for that to make packaging easier.
- There is an InputHandler plugin written in C++ which could be found in this repository (see src/presagepredictor.cpp)
- There is an QML InputHandler heavily (inspired by Nuance InputHandler by Jolla) in this repository
- To have predictive word prediction you will need to have ngram databases for your language

My plan is to separate these things to 2+one language specific packages:
- libpresage1 which contains only the presage library
- PresagePredictor for my QML and C++ InputHandler code
- One language specific package for installing the ngram databases

# Building
## Build presage
Copy the presage/rpm folder to the checked out presage source folder.
Follow the instructions here:
https://sailfishos.org/develop/tutorials/building-sailfish-os-packages-manually/

```
mm@lapos:~$ ssh -p 2222 -i /opt/SailfishOS/vmshare/ssh/private_keys/engine/mersdk mersdk@localhost
Last login: Thu Nov 30 19:38:03 2017 from 10.0.2.2
,---
| SailfishOS 2.1.3.7 (Kymijoki) (i486)
'---
[mersdk@SailfishSDK ~]$ cd /home/src1/keyboard/presage_predictor/presage/
[mersdk@SailfishSDK presage]$ mb2 -t SailfishOS-2.1.3.7-armv7hl build

```
If you want to build the InputHandler plugin with the SDK you should install the presage packages on the build VM with:

```
[mersdk@SailfishSDK presage]$ sb2 -t SailfishOS-2.1.3.7-armv7hl -m sdk-install -R rpm -i RPMS/*.rpm
```
After this you should be able to build the PresagePredictor.pro with the SailfishOS IDE.

# Debugging info
To debug a keyboard plugin you should run:
pkill maliit-server; MALIIT_DEBUG=enabled maliit-server
(Console output is controlled by the MALIIT_DEBUG variable)
From QML you could use the 

# Thanks, inspiration, links
Matteo Vescovi who developed the Presage:
http://presage.sourceforge.net/

Hanhsuan's predictor helped me at the beginnging a lot
https://github.com/hanhsuan/jolla-chewing

Pekka Vuorela shared the debugging tips with me
