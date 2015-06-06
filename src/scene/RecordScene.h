//
//  RecordScene.h
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#ifndef __MoanSymphony__RecordScene__
#define __MoanSymphony__RecordScene__

#include "ofxScene.h"
#include "ofxFFTLive.h"
#include "ShareData.h"

class RecordScene : public ofxScene
{
    
public:
    RecordScene(ShareData* _d)
    {
        shareData = _d;
        shareData->fft = &fft;
    }
    void setup();
    void update();
    void draw();
    
    void mouseReleased(int x, int y, int button);

    
    void willFadeIn(){}
    void willDraw(){}
    void willFadeOut(){}
    void willExit();
    
    ofxFFTLive fft;
    
    ShareData* shareData;
    ofImage recordbtn;
    ofRectangle recordRect;
    
    ofPath angleArc;
    
};



#endif /* defined(__MoanSymphony__RecordScene__) */
