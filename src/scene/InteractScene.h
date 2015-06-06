//
//  InteractScene.h
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#ifndef __MoanSymphony__InteractScene__
#define __MoanSymphony__InteractScene__

#include "ofxScene.h"
#include "ShareData.h"

class InteractScene : public ofxScene
{
    
public:
    InteractScene(ShareData* _d)
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
    void willExit(){
        shareData->playingEnabled = false;
        stopAll(baseVoice);
        if(ofInRange(shareData->graphicIndex, 0, graphicVoice.size()-1))
            stopAll(graphicVoice[shareData->graphicIndex]);
    }
    void loadVoiceIntoArray(vector<ofPtr<ofSoundPlayer> >& arr, string _path);
    void playAll(vector<ofPtr<ofSoundPlayer> >& arr);
    void stopAll(vector<ofPtr<ofSoundPlayer> >& arr);
    void updatePointCloud();
    void drawPointCloud();
    
    ofImage exitbtn;
    ofRectangle exitRect;
    
    vector<ofPtr<ofSoundPlayer> > baseVoice;
    vector<vector<ofPtr<ofSoundPlayer> > > graphicVoice;
    vector<float> vol;					//Volumes of the samples

    
    ShareData* shareData;
    
};
#endif /* defined(__MoanSymphony__InteractScene__) */
