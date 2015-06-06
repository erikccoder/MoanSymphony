#pragma once

#include "ofMain.h"
#include "ofxSceneManager.h"
#include "AppUtilDir.h"

//change
class ofApp : public ofBaseApp, public AppUtilDir
{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    //Function for receiving audio
    void audioReceived( float *input, int bufferSize, int nChannels );
    
    //Function for generating audio
    void audioOut( float *output, int bufferSize, int nChannels );

    void genSoundFromImage(ofImage& img);
    
    ofxSceneManager sceneManager;
    //Object for sound output and input setup
    ofSoundStream soundStream;

    vector<ofImage> genSoundImage;
};
