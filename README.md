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

# Debugging info
To debug a keyboard plugin you should run on your device as root:
```
pkill maliit-server; MALIIT_DEBUG=enabled maliit-server
```
(Console output is controlled by the MALIIT_DEBUG variable)

# Thanks, inspiration, links
Matteo Vescovi who developed the Presage:
http://presage.sourceforge.net/

Hanhsuan's predictor helped me at the beginnging a lot
https://github.com/hanhsuan/jolla-chewing

Pekka Vuorela shared the debugging tips with me
