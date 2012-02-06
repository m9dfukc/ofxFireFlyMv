#pragma once

#include "ofConstants.h"
#include "ofTypes.h"
#include "ofUtils.h"

#include "FlyCapture2.h"
using namespace FlyCapture2;

class ofxFireFlyMv {

	public :

		ofxFireFlyMv();
		~ofxFireFlyMv();

		bool 					isFrameNew();
		void					grabFrame();
		void					close();
		bool					initGrabber(int w, int h, unsigned int _deviceID = 0);
		void					setDeviceID(unsigned int _deviceID);
		void					listDevices();
		unsigned char*  		getPixels();

		void					update();
		
		float 					getHeight();
		float 					getWidth();

		int						height;
		int						width;
		

	
	protected:
                
		unsigned int 			deviceID;
		bool 					bGrabberInited;
		unsigned char*		 	pixels;
		bool 					bIsFrameNew;
        
		//--------------------------------- flyCapture
        Error                   err;
        BusManager		        busMgr;
        PGRGuid                 guid;
	    Camera          		cam;
        CameraInfo              camInfo;
        Format7Info             fmt7Info;
        Format7ImageSettings    fmt7ImageSettings;
        Format7PacketInfo       fmt7PacketInfo;
        Property                frmRate;
	    Image			        rawImage;

        Mode                    k_fmt7Mode;
        PixelFormat             k_fmt7PixFmt;

        Property                prop;
        PropertyInfo            propInfo;

        void PrintError( Error error ){
            printf( "%s\n", error.GetDescription() );
        }
};
