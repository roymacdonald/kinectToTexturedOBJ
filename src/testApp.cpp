#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	setupOpenNI();
	
	bPaused = false;
	
	mesh.setMode(OF_PRIMITIVE_POINTS);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(3);
	
	
	bDrawMesh = false;
	exporter.setup(ofToDataPath("calibration"));
	
	transZ = 0;
	bSetCenter = true;
	//cam.setNearClip(0);
	cam.setFarClip(10000); 
	zScale =1; 
}
//--------------------------------------------------------------
void testApp::setupOpenNI(){
	ofFileDialogResult result = ofSystemLoadDialog("select an ONI file: ");
	if (result.bSuccess) {
		oniPath = result.getPath();
		if (ofToLower(oniPath.substr(oniPath.length() -3 , 3)) == "oni") {
			openNIPlayer.setup();
			openNIPlayer.startPlayer(oniPath);
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
			inFrame = 0;
			outFrame = openNIPlayer.getTotalNumFrames()-1;
		}else {
			ofSystemAlertDialog("You didn't select an ONI file.\r\nIt's extension must be \".oni\"");
			setupOpenNI();
		}
	}
}
//--------------------------------------------------------------
void testApp::update(){
	openNIPlayer.update();
	if(bDrawMesh){ // && !bPaused){
		mesh.clear();
		unsigned short * raw = openNIPlayer.getDepthRawPixels().getPixels();
		unsigned char * img = openNIPlayer.getImagePixels().getPixels();
		
		int width = openNIPlayer.getImagePixels().getWidth();
		int height = openNIPlayer.getImagePixels().getHeight();
		int mn=10000, mx=0;
		
		for(int y = 0; y < height; y ++) {
			for(int x = 0; x < width; x ++) {
				if(raw[y*width + x] > 0) {
					if (bSetCenter) {
						mn = MIN(mn, raw[y*width + x]);
						mx = MAX(mx, raw[y*width + x]);
					}		
					float z = raw[y*width + x];// * zScale;
					mesh.addColor(ofColor(img[3*y*width+x*3], img[3*y*width+(x*3+1)], img[3*y*width+(x*3+2)], 255));
					ofVec3f pos;
					if (exporter.calibrationSetup) {
						pos = exporter.reproject(x, y, z);
						//pos.z *= zScale;
					}else {
						//pos.set(x, y, z*zScale);	
						pos.set(x, y, z);
					}
					mesh.addVertex(pos);
				}
			}
		}
		if (bSetCenter) {
			center.set(width/2.0f, height/2.0f, mn + (mx - mn) / 2.0f);
			cam.setTarget(center);
		    cam.setDistance((mx - mn) / 2.0f);
			cam.setFarClip(mx*2);
			bSetCenter = false;
			cout << "center: " << center<<endl;
		}
	}else if (bPaused && ofGetMousePressed() == 1) {
		float frame = float(ofGetMouseX())/ofGetWidth();
		openNIPlayer.setFrame(floor(openNIPlayer.getTotalNumFrames() * frame));
		//	cout << "Set frame: " << frame << endl;
	}
}
//--------------------------------------------------------------
void testApp::draw(){
	ofBackgroundGradient(ofColor:: gray, ofColor::white);
	ofSetColor(255, 255, 255);
	if (bDrawMesh) {
		ofPushMatrix();
		cam.begin();
		ofScale(1, -1, 1);
		ofTranslate(-320, -240,transZ);
		mesh.draw();		
		cam.end();
		ofPopMatrix();
	}else {
	openNIPlayer.drawDebug();
	}
	ofSetColor(255, 255, 0);
	string info = "KEYS:\r\n";
			info +=	"\"i\": set export IN position.\r\n";
			info +=	"	\"o\": set export OUT position.\r\n";
			info +=	"\"e\": export from IN to OUT.\r\n";
			info +=	"\"E\": export current frame.\r\n";
			info +=	"\r\n";
			info +=	"Right arrow key: go to next frame.\r\n";
			info +=	"Left arrow key: go to previous frame.\r\n";
			info +=	"\"0\": go to first frame.\r\n";
			info +=	"\r\n";
			info +=	"RETURN key: toogle mesh view.\r\n";
			info +=	"SpaceBar: toogle paused.\r\n";
			info +=	"\"t\": toogle registration.\r\n";
			info +=	"Current frame: " +ofToString(openNIPlayer.getCurrentFrame()) + "\r\n";
			info +=	"IN position: " + ofToString(inFrame) + "\r\n";
			info +=	"OUT position: " + ofToString(outFrame) + "\r\n";
			info +=	"Total frames: " + ofToString(openNIPlayer.getTotalNumFrames()) + "\r\n";
	ofDrawBitmapStringHighlight(info, 20, ofGetHeight()-240,ofColor(50), ofColor::yellow);
}
//--------------------------------------------------------------
void testApp::exit(){
    openNIPlayer.stop();
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
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
		case 'e':
			saveFrameRange();
		case 'E':
			saveFrameRange(false);
		case 't':
            openNIPlayer.toggleRegister();
            break;
		case OF_KEY_RETURN :
			bDrawMesh ^=true;
			break;
		case 'i':
			inFrame = openNIPlayer.getCurrentFrame();
			if (inFrame >= outFrame) {
				outFrame = inFrame+1;
			}
			break;
		case 'o':
			outFrame = openNIPlayer.getCurrentFrame();
			if (inFrame >= outFrame) {
				inFrame = outFrame-1;
			}
            break;
		case OF_KEY_UP:
			transZ+= 10;
			cout << "transZ: " << transZ << endl;
			break;
		case OF_KEY_DOWN:
			transZ-= 10;
			cout << "transZ: " << transZ << endl;
			break;
			
		case 'c':
			bSetCenter =true;
			break;
			
		case 'z':
			zScale-= zScale * 0.1;
			cout << "zScale: " << zScale << endl;
			break;
		case 's':
			zScale+= zScale * 0.1;
			cout << "zScale: " << zScale << endl;
			break;			
		default:
			break;	
    }
}

//--------------------------------------------------------------
void testApp::saveFrameRange(bool saveRange){
	int current;
	if (saveRange) {
		current = inFrame;		
	}else {
		current = openNIPlayer.getCurrentFrame();
	}

	string exportFolder = "exported"; 
	string fileName = "kinectMesh";
	cout << "Exporting " << fileName << " to folder " << exportFolder << endl;
	cout << "inFrame: " << inFrame << "  outFrame: " << outFrame << "  total: "<< outFrame - inFrame << endl;
	//*
	if (!ofDirectory::doesDirectoryExist(ofToDataPath(exportFolder))) {
		ofDirectory::createDirectory(exportFolder);
	}
	//*/	
	bool bDoExport = true;
	while (bDoExport) {
		
		cout << "exporting frame: " << current << ". " << current - inFrame +1 << " of " << outFrame - inFrame  << " frames." << endl;
		
		if(current <= outFrame){
			
			
			int frameNum;
			frameNum= getNextFile( exportFolder + "/" , fileName, "obj");
			openNIPlayer.setFrame(current);
			openNIPlayer.update();
			exporter.exportDepthToObj(exportFolder + "/" + fileName + ofToString(frameNum), openNIPlayer.getDepthRawPixels(), openNIPlayer.getImagePixels() );
			
		}
		current++;
		
		if (current > outFrame || !saveRange) {
			bDoExport = false;
		}
	}
}
//--------------------------------------------------------------
int testApp::getNextFile(string path, string name,string ext){
	ofDirectory lister;
	int n = -1;
	lister.allowExt(ext);
	lister.listDir(ofToDataPath(path, true));
	for(int i = 0; i < lister.size(); i++){
		size_t found;
		found=lister.getName(i).find(name);
		if (found!=string::npos){
			size_t endPos = lister.getName(i).find(ext, found);
			if (endPos!=string::npos) {
				n = MAX(ofToInt(lister.getName(i).substr(found+ name.length(), endPos - (found+ name.length()))), n);
			}
		}
	}
	return n+1;
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