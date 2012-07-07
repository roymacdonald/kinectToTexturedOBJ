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
#include <limits>
#include <fstream>

class DepthToObjExporter {
public:
	//--------------------------------------------------------------
	static string toObjFaceIndex(int ind){
		string temp;
		temp = ofToString(ind);
		return temp + "/" +temp + "/"+temp;
	}
	//--------------------------------------------------------------
	static void exportDepthToObj(string filename, ofShortPixels depth, ofPixels rgb){
		cout << "Exporting OBJ: " << filename << endl;
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
			obj << "mtllib " + filename + ".mat" << endl;
			
			
			
			unsigned short * depthPixels = depth.getPixels();
			for (int y = 0; y < depthHeight; y++) {// WRITE VERTICES
				for (int x = 0; x < depthWidth; x++) {
					obj << "v " << x << " " << -y << " " << ofToString(float(-depthPixels[y * depthWidth + x])/10) << "#" << ofToString(y * depthWidth + x) << endl;
					
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
		mtl.open(ofToDataPath(filename + ".mat").c_str(), ios::out);
		if (mtl.is_open() ) {
			
			mtl << "newmtl rgb" << endl;
			mtl << "Ka 1.000000 1.000000 1.000000" << endl;
			mtl << "Kd 1.000000 1.000000 1.000000" << endl;
			mtl << "Ks 0.000000 0.000000 0.000000" << endl;
			
			mtl << "illum 0" << endl;
			
			mtl << "map_Kd " + filename + ".jpg" << endl;
			mtl.close();
		}
		
		///SAVE RGB TEXTURE//////////////////////////////////////////////
		ofImage tempImg;
		tempImg.setFromPixels(rgb);
		tempImg.mirror(true, false);
		tempImg.saveImage(ofToDataPath(filename + ".jpg"));
		
		
		cout << "Export finished." << endl;
	}	
	
};