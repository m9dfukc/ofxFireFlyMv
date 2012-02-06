#include "ofxFireFlyMv.h"


//--------------------------------------------------------------------
ofxFireFlyMv::ofxFireFlyMv(){
	bIsFrameNew				= false;
	bGrabberInited 			= false;
	deviceID				= 0;
	width 					= 320;	// default setting
	height 					= 240;	// default setting
	pixels					= NULL;

	k_fmt7Mode = MODE_1;  // 2x2 pixel binning
    k_fmt7PixFmt = PIXEL_FORMAT_MONO8;
	
    unsigned int 			numCameras;
	
    err = busMgr.GetNumOfCameras(&numCameras);
    if(err!=PGRERROR_OK){ ofLog(OF_LOG_ERROR, "Failed to enumerate cameras"); }

    if( numCameras == 0 ) {
		ofLog(OF_LOG_ERROR, "No cameras found");
    }
}


//--------------------------------------------------------------------
ofxFireFlyMv::~ofxFireFlyMv(){
	close();
}


//--------------------------------------------------------------------
void ofxFireFlyMv::listDevices(){
    ofLog(OF_LOG_NOTICE, "-------------------------------------");
    /*    
	ofLog(OF_LOG_NOTICE, "Listing available IIDC capture devices.\nUse unit number as unique ID for camera...");
	for (uint32_t index = 0; index < deviceList->num; index++) {
		//printf("Using camera with GUID %"PRIx64"\n", camera->guid);
		ofLog(OF_LOG_NOTICE, "Device[%d].unit = %x, GUID = %x", index, deviceList->ids[index].unit, deviceList->ids[index].guid);
	}    
    */
    ofLog(OF_LOG_NOTICE, "-------------------------------------");
}

//--------------------------------------------------------------------
void ofxFireFlyMv::setDeviceID(unsigned int _deviceID){
	deviceID		= _deviceID;
}

//---------------------------------------------------------------------------
unsigned char * ofxFireFlyMv::getPixels(){
	return pixels;
}

//---------------------------------------------------------------------------
bool  ofxFireFlyMv::isFrameNew(){
	return bIsFrameNew;
}

//--------------------------------------------------------------------
void ofxFireFlyMv::update(){
	grabFrame();
}

//--------------------------------------------------------------------
void ofxFireFlyMv::grabFrame(){

    // Retrieve an image
    err = cam.RetrieveBuffer( &rawImage );
    if (err != PGRERROR_OK){
        PrintError( err );
        return;
    }
    
    bIsFrameNew = true;

	// copy into pixels
	for( int i = 0; i < height; i++ ) {
		memcpy( pixels + (i*width), rawImage.GetData() + (i*rawImage.GetStride()), width );
	}      
}


//--------------------------------------------------------------------
void ofxFireFlyMv::close(){
    // Stop capturing images
    err = cam.StopCapture();
    if (err != PGRERROR_OK){
        PrintError( err );
        return;
    }      

    // Disconnect the camera
    err = cam.Disconnect();
    if (err != PGRERROR_OK){
        PrintError( err );
        return;
    }

	if (pixels != NULL) {
		delete[] pixels;
		pixels = NULL;
	}
}

//--------------------------------------------------------------------
bool ofxFireFlyMv::initGrabber(int w, int h, unsigned int _deviceID){
    width = w;
    height = h;
	deviceID = _deviceID;

    /*-----------------------------------------------------------------------
     *  select camera
     *-----------------------------------------------------------------------*/
    err = busMgr.GetCameraFromSerialNumber(deviceID, &guid);

    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    // Connect to a camera
    err = cam.Connect(&guid);
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }


    /*-----------------------------------------------------------------------
     *  set features
     *-----------------------------------------------------------------------*/
    // Set the brighness property of the camera
    prop.type = BRIGHTNESS;
    err = cam.GetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    prop.autoManualMode = false;
    prop.valueA = 120;  //1-255

    err = cam.SetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    // exposure
    prop.type = AUTO_EXPOSURE;
    err = cam.GetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    prop.autoManualMode = false;
    prop.valueA = 50;  //7-62

    err = cam.SetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }


    // gain
    prop.type = GAIN;
    err = cam.GetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    prop.autoManualMode = false;
    prop.valueA = 64;  //16-64

    err = cam.SetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }
    
    // shutter
    prop.type = SHUTTER;
    err = cam.GetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    prop.autoManualMode = false;
    prop.valueA = 500;  //1-524

    err = cam.SetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }


    // pan
    prop.type = PAN;
    err = cam.GetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    prop.autoManualMode = false;
    prop.valueA = 80;  //0-112

    err = cam.SetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }


    /*-----------------------------------------------------------------------
     *  get  camera info
     *-----------------------------------------------------------------------*/
    // Get the camera information
    err = cam.GetCameraInfo(&camInfo);
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    //PrintCameraInfo(&camInfo);

    // Get EXPOSURE Property
    propInfo.type = PAN;
    err = cam.GetPropertyInfo( &propInfo );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }
    cout << "----EXPOSURE---------" << endl;
    cout << "min " << propInfo.min << endl;
    cout << "max " << propInfo.max << endl;
    cout << "absMin " << propInfo.absMin << endl;
    cout << "absMax " << propInfo.absMax << endl;

    prop.type = PAN;
    err = cam.GetProperty( &prop );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }
    cout << "valueA " << prop.valueA << endl;
    cout << "valueB " << prop.valueA << endl;


    /*-----------------------------------------------------------------------
     *  setup capture
     *-----------------------------------------------------------------------*/
    // Query for available Format 7 modes
    bool supported;
    fmt7Info.mode = k_fmt7Mode;
    err = cam.GetFormat7Info( &fmt7Info, &supported );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    //PrintFormat7Capabilities( fmt7Info );

    if ( (k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0 ){
        // Pixel format not supported!
		printf("Pixel format is not supported\n");
        return false;
    }
    
    fmt7ImageSettings.mode = k_fmt7Mode;
    fmt7ImageSettings.offsetX = 0;
    fmt7ImageSettings.offsetY = 0;
    //fmt7ImageSettings.width = fmt7Info.maxWidth;
    //fmt7ImageSettings.height = fmt7Info.maxHeight;
    fmt7ImageSettings.width = width;
    fmt7ImageSettings.height = height;
    fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

    // Validate the settings to make sure that they are valid
    bool valid;
    err = cam.ValidateFormat7Settings(
        &fmt7ImageSettings,
        &valid,
        &fmt7PacketInfo );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    if ( !valid ){
		printf("Format7 settings are not valid\n");
        return false;
    }

    // Set the settings to the camera
    err = cam.SetFormat7Configuration(
        &fmt7ImageSettings,
        fmt7PacketInfo.recommendedBytesPerPacket );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }


    /*-----------------------------------------------------------------------
     *  have the camera start sending us data
     *-----------------------------------------------------------------------*/
    err = cam.StartCapture();
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    // Retrieve frame rate property
    frmRate.type = FRAME_RATE;
    err = cam.GetProperty( &frmRate );
    if (err != PGRERROR_OK){
        PrintError( err );
        return false;
    }

    printf( "Frame rate is %3.2f fps\n", frmRate.absValue );

	pixels	= new unsigned char[width * height];
	memset(pixels, 0, width*height);

    bGrabberInited = true;

	return true;
}

//----------------------------------------------------------
float ofxFireFlyMv::getHeight(){
	return (float)height;
}

//----------------------------------------------------------
float ofxFireFlyMv::getWidth(){
	return (float)width;
}



