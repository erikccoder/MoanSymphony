//
//  RecordScene.cpp
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#include "RecordScene.h"

const int N = 256;		//Number of bands in spectrum
float spectrum[ N ];	//Smoothed spectrum values
float Rad = 600;		//Cloud raduis parameter
float Vel = 0.1;		//Cloud points velocity parameter
int bandRad = 2;		//Band index in spectrum, affecting Rad value
int bandVel = 100;		//Band index in spectrum, affecting Vel value

const int n = 333;		//Number of cloud points

//Offsets for Perlin noise calculation for points
float tx[n], ty[n];
ofPoint p[n];			//Cloud's points positions

float time0 = 0;		//Time value, used for dt computing

bool isRecording = false;
int endAngle;

float MAX_RECORD_DURATION = 3.5;
float startRecordTime;

void RecordScene::setup()
{
    ofHideCursor();
    ofShowCursor();

    //Set up sound sample
    
//    fft.setup();
    fft.setMirrorData(true);
    
    //Set spectrum values to 0
    for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }
    
    //Initialize points offsets by random numbers
    for ( int j=0; j<n; j++ ) {
        tx[j] = ofRandom( 0, 1000 );
        ty[j] = ofRandom( 0, 1000 );
    }
   
    if(!recordbtn.isAllocated())
        recordbtn.loadImage("btn/record_icon.png");
//    recordbtn.setAnchorPercent(.5, .5);
//    recordbtn.resize(150, 150);
    recordRect.setFromCenter(ofGetWidth()*.5, ofGetHeight()*.85, 150, 150);
    
    isRecording = false;
    endAngle = 0;
    startRecordTime = -1;
    
    endAngle = 0;
    angleArc.setArcResolution(360);
    angleArc.setFilled(true);
    angleArc.setColor(ofColor::red);
    
    shareData->recPos = 0;

}
void RecordScene::update()
{
    fft.update();
    
    //Get current spectrum with N bands
    //	float *val = ofSoundGetSpectrum( N );
    float *val = new float[N];
    fft.getFftPeakData(val, N);
    
    //We should not release memory of val,
    //because it is managed by sound engine
    
    //Update our smoothed spectrum,
    //by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for ( int i=0; i<N; i++ ) {
        spectrum[i] *= 0.97;	//Slow decreasing
        spectrum[i] = max( spectrum[i], val[i] );
    }
    
    //Update particles using spectrum values
    
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp( dt, 0.0, 0.1 );
    time0 = time; //Store the current time
    
    //Update Rad and Vel from spectrum
    //Note, the parameters in ofMap's were tuned for best result
    //just for current music track
    Rad = ofMap( spectrum[ bandRad ], 1, 3, 400, 800, true );
    Vel = ofMap( spectrum[ bandVel ], 0, 0.1, 0.05, 0.5 );
    
    //Update particles positions
    for (int j=0; j<n; j++)
    {
        tx[j] += Vel * dt;	//move offset
        ty[j] += Vel * dt;	//move offset
        //Calculate Perlin's noise in [-1, 1] and
        //multiply on Rad
        p[j].x = ofSignedNoise( tx[j] ) * Rad;		
        p[j].y = ofSignedNoise( ty[j] ) * Rad;	
    }
    
    delete[] val;
    
    if(isRecording)
    {
        if(ofGetElapsedTimef() - startRecordTime > MAX_RECORD_DURATION)
        {
            shareData->nextScent = 1;
//            startRecordTime = ofGetElapsedTimef();
        }
        
        if(endAngle < 270)
        {
            endAngle = ofMap(ofGetElapsedTimef(),
                         startRecordTime, startRecordTime + MAX_RECORD_DURATION, -90, 270, true);
        }
        
        angleArc.clear();
        float radius = recordRect.getWidth()*.5+2;
        
        angleArc.arc( 0,  0, radius, radius, -90, endAngle);
        angleArc.close();
        
        
    }
    
    //this is just to draw the arc that represents the angle

}

void RecordScene::draw()
{
    ofPushStyle();
    ofBackground(ofColor::white);
    
    /*
    //Draw background rect for spectrum
    ofSetColor( 230, 230, 230 );
    ofFill();
    ofRect( 0, recordRect.getMaxY(), N * 6, -100 );
    
    //Draw spectrum
    ofSetColor( 0, 0, 0 );
    for (int i=0; i<N; i++) {
        //Draw bandRad and bandVel by black color,
        //and other by gray color
        if ( i == bandRad || i == bandVel ) {
            ofSetColor( 0, 0, 0 ); //Black color
        }
        else {
            ofSetColor( 128, 128, 128 ); //Gray color
        }
        ofRect( 10 + i * 5, recordRect.getMaxY(), 3, -spectrum[i] * 100 );
    }
    */
    
    //Draw cloud
    
    //Move center of coordinate system to the screen center
    ofPushMatrix();
    ofTranslate( ofGetWidth() / 2, ofGetHeight() / 2 );
    
    //Draw points
    ofSetColor( 0, 0, 0 );
    ofFill();
    for (int i=0; i<n; i++) {
        ofCircle( p[i], 2 );
    }
    
    //    ofSetColor(ofColor::yellow);
    //Draw lines between near points
    float dist = 40;	//Threshold parameter of distance
    for (int j=0; j<n; j++) {
        for (int k=j+1; k<n; k++) {
            if ( ofDist( p[j].x, p[j].y, p[k].x, p[k].y )
                < dist ) {
                ofLine( p[j], p[k] );
            }
        }
    }
    
    //Restore coordinate system
    ofPopMatrix();
    
    ofSetColor(ofColor::white);
    ofPoint _p = recordRect.getCenter();
    if(isRecording)
        angleArc.draw(_p.x, _p.y);
    recordbtn.draw(recordRect);

    ofSetColor(ofColor::black);
    
//    path.moveTo(300, 300);
//    path.arc( 0, 0, 200, 200, 0, 271);
////    path.arc(recordRect.getCenter(), recordRect.width+20, recordRect.height+20, 0, endAngle);
    
    if(!isRecording)
        ofDrawBitmapStringHighlight("Press to record your voice expression", 5, 140);
    
    ofPopStyle();
    
    shareData->recordingEnabled = isRecording;
}

//--------------------------------------------------------------
void RecordScene::mouseReleased(int x, int y, int button)
{
    if(recordRect.inside(x, y))
    {
        startRecordTime = ofGetElapsedTimef();
        isRecording = true;
    }
}

void RecordScene::willExit()
{
    isRecording = false;
    shareData->recordingEnabled = false;
}
