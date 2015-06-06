//
//  ShareData.h
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#ifndef MoanSymphony_ShareData_h
#define MoanSymphony_ShareData_h

#include "ofMain.h"
#include "ofxFFTLive.h"

class ShareData {
    
public:
    ShareData()
    {
        nextScent = -1;
        graphicIndex = -1;
        recordingEnabled = false;	//Is recording enabled
        playingEnabled = false;		//Is playing enabled
        recPos = 0;				//Current recording position in the buffer
        playPos = 0;			//Current playing position in the buffer

    }
    int nextScent;
    
    int graphicIndex;
    
    vector<ofImage> rgImage;
    vector<ofRectangle> rgImgRect;    
    
    bool recordingEnabled;	//Is recording enabled
    bool playingEnabled;		//Is playing enabled

    int recPos;				//Current recording position in the buffer
    int playPos;			//Current playing position in the buffer

    ofxFFTLive* fft;
    
    int playbackSpeed;

};

#endif
