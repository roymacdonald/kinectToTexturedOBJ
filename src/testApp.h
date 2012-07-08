#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenNI.h"

#include "DepthToObjExporter.h"

class testApp : public ofBaseApp{
	


public:
	void setup();
	void update();
	void draw();
    void exit();
	void setupOpenNI();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	void saveFrameRange(bool saveRange = true);
	int getNextFile(string path, string name,string ext);
	ofxOpenNI openNIPlayer;
	
	bool bDrawDepth ;
	
	bool bPaused;
	bool bSaveObj;
	int frameNum;
	
	DepthToObjExporter exporter;

	int inFrame, outFrame;
	
	ofMesh mesh;
	bool bDrawMesh;
	
	float transZ;
	
	string oniPath;
	
	float zScale; 
	ofEasyCam cam;
	ofVec3f center;
	bool bSetCenter;
};

#endif


