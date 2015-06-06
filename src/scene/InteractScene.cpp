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

const int cN = 256;		//Number of bands in spectrum
float cSpectrum[ cN ];	//Smoothed spectrum values
float cRad = 500;		//Cloud raduis parameter
float cVel = 0.01;		//Cloud points velocity parameter
int cBandRad = 125;		//Band index in spectrum, affecting Rad value
int cBandVel = 100;		//Band index in spectrum, affecting Vel value

const int cNn = 111;		//Number of cloud points

//Offsets for Perlin noise calculation for points
float cTx[cNn], cTy[cNn];
ofPoint cP[cNn];			//Cloud's points positions

float cTime0 = 0;		//Time value, used for dt computing


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
    
    //Set spectrum values to 0
    for (int i=0; i<cN; i++) {
        cSpectrum[i] = 0.0f;
    }
    
    //Initialize points offsets by random numbers
    for ( int j=0; j<cNn; j++ ) {
        cTx[j] = ofRandom( 0, 1000 );
        cTy[j] = ofRandom( 0, 1000 );
    }

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
        
        updatePointCloud();
    }
}

void InteractScene::updatePointCloud(){
    
    shareData->fft->update();
    
    //Get current spectrum with N bands
    //	float *val = ofSoundGetSpectrum( N );
    float *val = new float[cN];
    shareData->fft->getFftPeakData(val, cN);
    
    //We should not release memory of val,
    //because it is managed by sound engine
    
    //Update our smoothed spectrum,
    //by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for ( int i=0; i<cN; i++ ) {
        cSpectrum[i] *= 0.97;	//Slow decreasing
        cSpectrum[i] = max( cSpectrum[i], val[i] );
    }
    
    //Update particles using spectrum values
    
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();	//Get current time

    float dt = time - cTime0;
    dt = ofClamp( dt, 0.0, 0.1 );
    cTime0 = time; //Store the current time
    
    //Update Rad and Vel from spectrum
    //Note, the parameters in ofMap's were tuned for best result
    //just for current music track
    cRad = ofMap( shareData->fft->getAveragePeak(), 0, 1, 400, 800, true );
    cVel = ofMap( cSpectrum[ cBandVel ], 0, 0.1, 0.01, 0.07 );
    
    //Update particles positions
    for (int j=0; j<cNn; j++)
    {
        cTx[j] += cVel * dt;	//move offset
        cTy[j] += cVel * dt;	//move offset
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        cP[j].x = ofSignedNoise( cTx[j] ) * cRad;
        cP[j].y = ofSignedNoise( cTy[j] ) * cRad;
    }
    
    delete[] val;

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
        
        
        ofPushStyle();
            ofPushMatrix();
                ofTranslate(ofGetWidth()*-.73, -500);
                ofScale(2, 2);
                    drawPointCloud();
            ofPopMatrix();
            ofPushMatrix();
                ofTranslate(ofGetWidth()*.333, 300);
                    drawPointCloud();
            ofPopMatrix();
        ofPopStyle();
        
        
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

void InteractScene::drawPointCloud(){
    //Draw cloud
    
    //Move center of coordinate system to the screen center
    ofPushMatrix();
    ofTranslate( ofGetWidth() / 2, ofGetHeight() / 2 );
    
    //Draw points
    ofSetColor( 0, 0, 0 );
    ofFill();
    for (int i=0; i<cNn; i++) {
        ofCircle( cP[i], 3 );
    }
    
    //    ofSetColor(ofColor::yellow);
    //Draw lines between near points
    float dist = 40;	//Threshold parameter of distance
    for (int j=0; j<cNn; j++) {
        for (int k=j+1; k<cNn; k++) {
            if ( ofDist( cP[j].x, cP[j].y, cP[k].x, cP[k].y )
                < dist ) {
                ofLine( cP[j], cP[k] );
            }
        }
    }
    
    //Restore coordinate system
    ofPopMatrix();
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
