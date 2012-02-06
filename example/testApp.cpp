#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    camWidth    = 640;
    camHeight   = 480;
    
	if( !vidGrabber.initGrabber(camWidth, camHeight, (unsigned int)cameraSerialNr) ){
		ofLog(OF_LOG_FATAL_ERROR, "sorry, no camera found - exit application!");
		ofExit();
	}
    
    curFrame.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);

}

//--------------------------------------------------------------
void testApp::update(){
    bool bNewFrame = false;
    vidGrabber.grabFrame();
	bNewFrame = vidGrabber.isFrameNew();
    if( bNewFrame ) {
        curFrame.setFromPixels(vidGrabber.getPixels(), camWidth, camHeight, OF_IMAGE_GRAYSCALE);
        curFrame.update();
    }
	 	
}

//--------------------------------------------------------------
void testApp::draw(){
	
	curFrame.draw(0, 0); //draw your grabbed image

}
