#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	setupOpenNI();
	bSaveObj =false;
	bPaused = false;
		frameNum =0;
}
//--------------------------------------------------------------
void testApp::setupOpenNI(){
    openNIPlayer.setup();
	openNIPlayer.startPlayer("test0.oni");
	openNIPlayer.addDepthGenerator();
	openNIPlayer.addImageGenerator();
	openNIPlayer.setRegister(true);
   // openNIPlayer.setMirror(true);
    //openNIPlayer.addUserGenerator();
    //openNIPlayer.setMaxNumUsers(1);
//	openNIPlayer.setUseOrientationAllUsers(true);
	//openNIPlayer.setUseMaskPixelsAllUsers(true);
	openNIPlayer.setUseDepthRawPixels(true);
	openNIPlayer.start();
	
}
//--------------------------------------------------------------
void testApp::update(){
	openNIPlayer.update();
		
	if (bSaveObj) {
        if (openNIPlayer.isNewFrame()) {
    		DepthToObjExporter::exportDepthToObj("openNIMesh" + ofToString(frameNum), openNIPlayer.getDepthRawPixels(), openNIPlayer.getImagePixels() );
			frameNum++;
			if (bPaused) {
				bSaveObj = false;
			}
		}    
	}
}
//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(50, 50, 50);
	ofSetColor(255, 255, 255);
	openNIPlayer.drawDebug();
	
	ofDrawBitmapString("Saving OBJ: " + ofToString(bSaveObj), 20, 500);
	
}
//--------------------------------------------------------------
void testApp::exit(){
    openNIPlayer.stop();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
	float smooth;
	
	switch (key) {
		case '0':
			openNIPlayer.firstFrame();
			break;
		case OF_KEY_LEFT:
			openNIPlayer.previousFrame();
			break;
		case OF_KEY_RIGHT:
			openNIPlayer.nextFrame();
			break;
		case ' ':
			openNIPlayer.setPaused(!openNIPlayer.isPaused());
			bPaused = openNIPlayer.isPaused();
			break;
		case 's':
		case 'S':
			bSaveObj ^= true;
		default:
			break;
			
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){}
//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void testApp::windowResized(int w, int h){}