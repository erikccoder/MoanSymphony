//
//  InteractScene.cpp
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#include "InteractScene.h"
#include "AppUtilDir.h"

float curAngle;



void InteractScene::setup()
{
    ofHideCursor();
    ofShowCursor();

    if(!exitbtn.isAllocated())
    {
        exitbtn.loadImage("btn/cross_icon.png");
        exitbtn.setAnchorPercent(.5, .5);
    }
    exitRect.set(ofGetWidth()- 120, 20, 100, 100);
    

    shareData->playPos = 0;
    shareData->playingEnabled = true;
    shareData->recordingEnabled = false;

    
    if(!baseVoice.size())
    {
        loadVoiceIntoArray(baseVoice, "baseVoice");
        vol.resize(baseVoice.size());
    }
    if(!graphicVoice.size())
    {
        graphicVoice.resize(5);
        loadVoiceIntoArray(graphicVoice[0], "recording001");
        loadVoiceIntoArray(graphicVoice[1], "recording002");
        loadVoiceIntoArray(graphicVoice[2], "recording003");
        loadVoiceIntoArray(graphicVoice[3], "recording004");
        loadVoiceIntoArray(graphicVoice[4], "recording005");
    }
    playAll(baseVoice);
    
    if(ofInRange(shareData->graphicIndex, 0, graphicVoice.size()-1))
        playAll(graphicVoice[shareData->graphicIndex]);
    
    
    ofSoundSetVolume( 0.2 );
}

void InteractScene::stopAll(vector<ofPtr<ofSoundPlayer> >& arr)
{
    for ( int i=0; i<arr.size(); i++)
    {
        arr[i]->stop();		//Start a sample to play
    }
}


void InteractScene::playAll(vector<ofPtr<ofSoundPlayer> >& arr)
{
    for ( int i=0; i<arr.size(); i++)
    {
        arr[i]->setLoop( true );
        arr[i]->setPan( ofMap( i, 0, arr.size()-1, -0.5, 0.5 ) );
        arr[i]->setVolume( 0 );
        arr[i]->play();		//Start a sample to play
    }
}

void InteractScene::loadVoiceIntoArray(vector<ofPtr<ofSoundPlayer> >& arr, string _path)
{
    AppUtilDir _appDir;
    vector<string> _voice = _appDir.getFilesFrom(_path, "wav");
    arr.resize(_voice.size());
    for ( int i=0; i<_voice.size(); i++)
    {
        arr[i] = ofPtr<ofSoundPlayer>(new ofSoundPlayer);
        arr[i]->loadSound(_voice[i]);
    }

}

void InteractScene::update()
{
    if(shareData->graphicIndex <0 || shareData->graphicIndex >= shareData->rgImage.size())
    {
        shareData->nextScent = 0;
    }
    else
    {
        float tarAngle = exitRect.inside(ofGetMouseX(), ofGetMouseY()) ?  -90 : 45;
        curAngle = ofLerp(curAngle, tarAngle, 0.1);
        
        
        float time = ofGetElapsedTimef();	//Get current time
        //Update volumes
        float tx = time*0.1 + 50;	 //Value, smoothly changed over time
        
        int _size = baseVoice.size() + graphicVoice[shareData->graphicIndex].size();
        for (int i=0; i<_size; i++)
        {
            
            if(i < baseVoice.size())
            {
                //Calculate the sample volume as 2D Perlin noise,
                //depending on tx and ty = i * 0.2
                float ty = i * 0.2;
                vol[i] = ofNoise( tx, ty );	//Perlin noise
                vol[i] = ofMap(vol[i], 0, 1, 0, 0.1, true);
                baseVoice[i]->setVolume( vol[i] );	//Set sample's volume
                //        ofLog() << "voice["<< i <<"]: " << vol[i];
            }
            else
            {
                float ty = i * 0.2;
                float vol = ofNoise( tx, ty );	//Perlin noise
                vol = ofMap(vol, 0, 1, 0, 0.55, true);

                graphicVoice[shareData->graphicIndex][i-baseVoice.size()]->setVolume(vol);
            }
            
        }
        
        //Update sound engine
        ofSoundUpdate();
    }
}

void InteractScene::draw()
{
    if(shareData->graphicIndex <0 || shareData->graphicIndex >= shareData->rgImage.size())
    {
        shareData->nextScent = 0;
    }
    else
    {
        ofPushStyle();
        
        ofBackground(ofColor::white);
        
        ofImage& img = shareData->rgImage[shareData->graphicIndex];
        ofRectangle targetRect(0,0, ofGetWidth(), ofGetHeight());
        ofRectangle workingSubjectRect(0,0, img.getWidth(), img.getHeight());

        workingSubjectRect.scaleTo(targetRect, OF_SCALEMODE_FIT);

        img.draw(workingSubjectRect);
        
        
        ofSetColor(ofColor::black);
        ofDrawBitmapStringHighlight("Drag to variant the Symphony", 4, 140);
        
        
        ofPushMatrix();
            ofTranslate(exitRect.getCenter());
            ofRotate(curAngle);
            exitbtn.draw(0,0, exitRect.getWidth(), exitRect.getHeight());
        ofPopMatrix();
        
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void InteractScene::mouseReleased(int x, int y, int button)
{
    if(exitRect.inside(x, y))
    {
        shareData->nextScent = 0;
    }
}

//--------------------------------------------------------------
void InteractScene::mouseMoved(int x, int y )
{
    float speed = ofMap( x, 0, ofGetWidth(), 0.3, 2 );
    for (int i=0; i<baseVoice.size(); i++) {
        baseVoice[i]->setSpeed( speed  );
    }
    if(ofInRange(shareData->graphicIndex, 0, graphicVoice.size()-1))
    {
        for (int i=0; i<graphicVoice[shareData->graphicIndex].size(); i++) {
            graphicVoice[shareData->graphicIndex][i]->setSpeed( speed  );
        }
    }
    shareData->playbackSpeed = MAX(1, speed * 1.3);
}
