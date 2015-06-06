#include "ofApp.h"
#include "RecordScene.h"
#include "GrahicScene.h"
#include "InteractScene.h"
#include "ShareData.h"


ShareData shareData;

//Constants
const int sampleRate = 44100;           //Sample rate of sound
const float duration = 3;            //Duration of the recorded sound in seconds
const int N = duration * sampleRate;    //Size of the PCM buffer

float volume = 0.5;	//Output sound volume

//Variables
vector<float> buffer;		//PCM buffer of sound sample

//Variables
vector<float> arr; 		//Temporary array of pixels brightness, mapped to range [-1,1]
vector<float> genSoundBuffer;  //PCM buffer of sound sample
int genSoundPlayPos = 0;		//The current position of the buffer playing

const float genDuration = 0.25;            //Duration of the recorded sound in seconds
const int genSampleRate = 44100;           //Sample rate of sound
const int genN = genDuration * genSampleRate;     //Size of the PCM buffer of sound sample
int Y0;
const int grabW = 1024;                 //Width of the camera frame


//--------------------------------------------------------------
void ofApp::setup()
{
    //Set arrays sizes and fill these by zeros
    arr.resize( grabW, 0.0 );
    genSoundBuffer.resize( genN, 0.0 );

    
    ofBackground(ofColor::white);
    
    sceneManager.addScene(ofPtr<ofxScene>(new RecordScene(&shareData)));
    sceneManager.addScene(ofPtr<ofxScene>(new GrahicScene(&shareData)));
    sceneManager.addScene(ofPtr<ofxScene>(new InteractScene(&shareData)));
    sceneManager.setSceneDuration(0.7, 1.5, 0.7);
    sceneManager.setTransitionDissolve();
    
    sceneManager.run();
    

    vector<string> _exts;
    _exts.push_back("png");
    _exts.push_back("jpg");
    
    vector<string>_files = AppUtilDir::getFilesFrom("illustrations/", _exts);
    
    shareData.rgImage.resize(_files.size());
    shareData.rgImgRect.resize(_files.size());
    genSoundImage.resize(_files.size());
    for (int i=0; i < _files.size(); i++)
    {
        shareData.rgImage[i].loadImage(_files[i]);
        genSoundImage[i].loadImage(_files[i]);
        genSoundImage[i].resize(grabW, ofGetHeight());
    }
    
    //Set buffer size and fill it by zeros
    buffer.resize( N, 0.0 );
    
    //Start the sound output in stereo (2 channels)
//    and sound input in mono (1 channel)
    soundStream.setup( this, 2, 1, sampleRate, 256, 4 );
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(shareData.nextScent >= 0){
        sceneManager.changeScene(shareData.nextScent);
        shareData.nextScent = -1;
    }    
    sceneManager.update();
    

    float time = ofGetElapsedTimef();	//Get current time
    //Update volumes
    float tx = time*0.1 + 50;	 //Value, smoothly changed over time
    int i = 6;
    {
        float ty = i * 0.2;
        volume = ofMap(ofNoise( tx, ty ), 0, 1, 0.5, 1.5, true);
    }
    if(shareData.playingEnabled)
    {
        genSoundFromImage(genSoundImage[shareData.graphicIndex]);
    }
}

void ofApp::genSoundFromImage(ofImage& img)
{
    
        //Get pixels of the camera image
        //		ofPixels &pixels = grabber.getPixelsRef();
        ofPixels &pixels = img.getPixelsRef();
    
    
        //Read pixels brightness from the central line of the image
        for (int x=0; x<grabW; x++)
        {
            //Get the pixel brightness
            float v = pixels.getColor( x, Y0 ).getLightness();
            
            //v lies in [0,255], convert it to [-1,1]
            arr[x] = ofMap( v, 0, 255, -1, 1, true );
        }
        
        //Stretch arr to buffer (having size N), using linear interpolation
        for (int i=0; i<genN; i++) {
            float pos = float(i) * grabW / genN;   //Get float position in range [0, grabW]
            int pos0 = int( pos );              //Get left index
            int pos1 = min( pos0 + 1, genN-1 );    //Getright index
            //Interpolate
            genSoundBuffer[i] = ofMap( pos, pos0, pos1, arr[pos0], arr[pos1] );
        }
        
        //Shift the buffer values to have a mean value
        //equal to zero
        //Compute a mean value of buffer
        float mean = 0;
        for (int i=0; i<genN; i++) {
            mean += genSoundBuffer[i];
        }
        mean /= genN;
        //Shift the buffer by mean value
        for (int i=0; i<genN; i++) {
            genSoundBuffer[i] -= mean;
        }        
        
        Y0 += 1;
        if(Y0 >= img.getHeight())
            Y0 = 0;
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    sceneManager.draw();
    if(shareData.playingEnabled)
    {
        ofPushStyle();
        ofNoFill();
        ofSetColor( 255, 255, 0 );
        ofRect( 0, Y0 - 1, genN, 3 );
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == ' ') {
        sceneManager.changeScene();
    }
    else if(key == 'f')
    {
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    ofHideCursor();
    ofShowCursor();
    Y0 = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
//Audio input
void ofApp::audioReceived(float *input, int bufferSize, int nChannels )
{
    //If recording is enabled by the user,
    //then store received data
    if ( shareData.recordingEnabled )
    {
        for (int i=0; i<bufferSize; i++) {
            buffer[ shareData.recPos ] = input[i];
            shareData.recPos++;
            //When the end of buffer is reached, recPos sets
            //to 0, so we record sound in a loop
            shareData.recPos %= N;
        }
    }
    shareData.fft->audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
//Audio output
void ofApp::audioOut(float *output, int bufferSize, int nChannels)
{
    //If playing is enabled by the user, then do output sound
    if ( shareData.playingEnabled )
    {
        for (int i=0; i<bufferSize; i++)
        {
            output[ 2*i ] = output[ 2*i+1 ] = buffer[ shareData.playPos ] * volume + genSoundBuffer[genSoundPlayPos] * .1 ;
            
            shareData.playPos += shareData.playbackSpeed;
            //When the end of buffer is reached, playPos sets
            //to 0, so we hear looped sound
            shareData.playPos %= N;
            
            genSoundPlayPos++;
            //When the end of buffer is reached, playPos sets to 0
            //So we hear looped sound
            genSoundPlayPos %= genN;

        }
    }
}
