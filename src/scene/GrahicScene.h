//
//  GrahicScene.h
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#ifndef __MoanSymphony__GrahicScene__
#define __MoanSymphony__GrahicScene__

#include "ofxScene.h"
#include "ShareData.h"



class GrahicScene : public ofxScene
{
    
public:
    GrahicScene(ShareData* _d)
    {
        shareData = _d;
    }
    
    void setup();
    void update();
    void draw();
    
    void mouseReleased(int x, int y, int button);
    void mouseMoved(int x, int y );
    
    void willFadeIn(){}
    void willDraw(){}
    void willFadeOut(){}
    void willExit(){}
    
    ShareData* shareData;
    vector<ofFbo> rgCropImage;
};

#endif /* defined(__MoanSymphony__GrahicScene__) */
