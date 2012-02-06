#pragma once

#include "ofMain.h"
#include "ofxFireFlyMv.h"
#define CAMERA 11221819 // camera device id (as found in the point grey app)

class testApp : public ofBaseApp{

	public:
		void            setup();
		void            update();
		void            draw();
    
        ofImage         curFrame;
    	ofxFireFlyMv    vidGrabber;	
        unsigned int    camWidth;
        unsigned int    camHeight;
        
};

