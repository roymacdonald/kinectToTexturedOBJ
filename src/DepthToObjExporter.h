/*
 *  exportDepthToObj.h
 *  depthToObj
 *
 *  Created by Roy Macdonald on 7/5/12.
 *  Copyright 2012 micasa. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include <fstream>


using namespace ofxCv;

class DepthToObjExporter {
public:
	
	bool calibrationSetup;
	
	DepthToObjExporter(){
		bool calibrationSetup = false;
	}
	//--------------------------------------------------------------
	static string toObjFaceIndex(int ind){
		string temp;
		temp = ofToString(ind);
		return temp + "/" +temp + "/"+temp;
	}
	//--------------------------------------------------------------
	void exportDepthToObj(string filename, ofShortPixels depth, ofPixels rgb){
		cout << "Exporting OBJ: " << filename << endl;
		size_t found = filename.find_last_of("/");
		///OBJ FILE WRITE//////////////////////////////////////////////
		
		ofstream obj;
		
		obj.open(ofToDataPath(filename + ".obj").c_str(), ios::out);
		if (obj.is_open() ) {
			int depthHeight = depth.getHeight();
			int depthWidth = depth.getWidth();
			/*
			 The following is the obj file header. Don't modify unless you know what you are doing.
			 //*/
			
			obj << "# -----------------" << endl;
			obj << "# Start of obj file" << endl;
			obj << "g depth" << endl;
			
			obj << "mtllib " + filename.substr(found+1) + ".mat" << endl;
			
			
			
			unsigned short * depthPixels = depth.getPixels();
			ofVec3f r;
			for (int y = 0; y < depthHeight; y++) {// WRITE VERTICES
				for (int x = 0; x < depthWidth; x++) {
					if (calibrationSetup) {
						r =	reproject(x, y, depthPixels[y * depthWidth + x]);
						obj << "v " << r.x << " " << -r.y << " " << -r.z << endl;	
					}else {
						obj << "v " << x << " " << -y << " " << ofToString(float(-depthPixels[y * depthWidth + x])/10)  << endl;
					}
				}
			}
			obj << endl << endl;
			
			for (int y = 0; y < depthHeight; y++) { ///WRITE NORMALS. TODO: CALCULATE NORMALS CORRECTLY
				for (int x = 0; x < depthWidth ; x++) {
					obj << "vn " << 0.0 << " " << 0.0  << " " << 1.0 << endl;
				}
			}
			
			obj << endl << endl;
			
			for (int y = 0; y < depthHeight; y++) {//WRITE TEXTURE MAPPING.
				for (int x = 0; x < depthWidth ; x++) {
					obj << "vt " << float(x)/depthWidth << " " << float(y)/depthHeight << endl;
				}
			}
			
			obj << endl << endl;
			
			obj << "usemtl rgb" << endl;
			for (int y = 0; y < depthHeight -1; y++) {//WRITE FACE INDEXES
				for (int x = 0; x < depthWidth -1 ; x++) {
					int b = y * depthWidth + x ;
					int a = b + 1;
					int c = (y + 1)* depthWidth + x ;
					int d = c + 1 ;
					if (depthPixels[a] > 0 && depthPixels[b] > 0 && depthPixels[c] > 0) {
						obj << "f " << toObjFaceIndex(a + 1)  << " " << toObjFaceIndex(b + 1)<< " " << toObjFaceIndex(c + 1)<< endl;						
					}
					if (depthPixels[a] > 0 && depthPixels[d] > 0 && depthPixels[c] > 0) {
						obj << "f " << toObjFaceIndex(a + 1)  << " " << toObjFaceIndex(c + 1)<< " " << toObjFaceIndex(d + 1)<< endl;
					}
				}
			}
			
			obj.close();
		}
		///MTL FILE WRITE//////////////////////////////////////////////
		ofstream mtl;
		mtl.open(ofToDataPath( filename + ".mat").c_str(), ios::out);
		if (mtl.is_open() ) {
			
			mtl << "newmtl rgb" << endl;
			mtl << "Ka 1.000000 1.000000 1.000000" << endl;
			mtl << "Kd 1.000000 1.000000 1.000000" << endl;
			mtl << "Ks 0.000000 0.000000 0.000000" << endl;
			
			mtl << "illum 0" << endl;
			
			mtl << "map_Kd " + filename.substr(found+1) + ".jpg" << endl;
			mtl.close();
		}
		
		///SAVE RGB TEXTURE//////////////////////////////////////////////
		ofImage tempImg;
		tempImg.setFromPixels(rgb);
		tempImg.mirror(true, false);
		tempImg.saveImage(ofToDataPath(filename + ".jpg"));
		
		
		cout << "Export finished." << endl;
	}
	//--------------------------------------------------------------
	ofVec3f reproject(float x, float y, unsigned short z){
		return ofVec3f(((x - principalPoint.x) / imageSize.width) * z * fx,
					   ((y - principalPoint.y) / imageSize.height) * z * fy, z);
	
	}
	//--------------------------------------------------------------
	//this need to be done so to get the camera intrinsics and be able to perform correcly the reprojection.
	//code taken from RGBToolkit mesh builder. https://github.com/obviousjim/ofxRGBDepth/blob/master/src/ofxRGBDMeshBuilder.cpp
	bool setup(string calibrationDirectory, bool bUseRgbCalibration = true){
		if(!ofDirectory(calibrationDirectory).exists()){
			ofLogError("DepthToObjExporter --- Calibration directory doesn't exist: " + calibrationDirectory);
			return false;
		}
		
		depthCalibration.load(calibrationDirectory+"/kinect-color.yml");
		rgbCalibration.load(calibrationDirectory+"/kinect-ir.yml");
		
		Calibration * calib; 
		
		if (bUseRgbCalibration) {
			calib = &rgbCalibration;
		}else {
			calib = &depthCalibration;
		}

		
		Point2d fov = calib->getUndistortedIntrinsics().getFov();
		fx = tanf(ofDegToRad(fov.x) / 2) * 2;
		fy = tanf(ofDegToRad(fov.y) / 2) * 2;
		
		principalPoint = calib->getUndistortedIntrinsics().getPrincipalPoint();
		imageSize = calib->getUndistortedIntrinsics().getImageSize();
		
		return (calibrationSetup = true);
	}
private:
	Calibration depthCalibration, rgbCalibration;	
    
    Point2d principalPoint;
    cv::Size imageSize;
    float fx,fy;
    
	
};