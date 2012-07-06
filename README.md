kinectToTexturedOBJ
============

This uses openNI, but it should work with no problems with ofxKinect or whatever.

if you want to use this core right away you'll need to use gameoverhack's ofxOpenNi experimental branch:
https://github.com/gameoverhack/ofxOpenNI/tree/experimental

Right now it's using a recorded ONI file to extract the depth and rgb data but if you want to use a live stream you must comment the line in setup where the player is setup.
Using the live stream will save in realtime, at least for now. 