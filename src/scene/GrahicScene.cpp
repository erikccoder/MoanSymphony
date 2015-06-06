//
//  GrahicScene.cpp
//  MoanSymphony
//
//  Created by erikccoder on 5/6/15.
//
//

#include "GrahicScene.h"

int highLightIndex;
float refrashTime = 3.77;
float appearTime;

void GrahicScene::setup()
{
    shareData->recordingEnabled = false;

    ofHideCursor();
    ofShowCursor();

    shareData->graphicIndex = -1;
    
    int _size = shareData->rgImage.size();
    if(rgCropImage.size() != _size)
    {
        rgCropImage.resize(_size);
        for (int i=0; i < _size; i++)
        {
            int w = ofGetWidth() / _size;
            int h = ofGetHeight();
            
            ofImage& img = shareData->rgImage[i];
            ofRectangle workingSubjectRect(0,0, img.getWidth(), img.getHeight());
            
            ofRectangle& targetRect = shareData->rgImgRect[i];
            targetRect.set(0, 0, w, h);
            
            workingSubjectRect.scaleTo(targetRect, OF_SCALEMODE_FILL);
            
            rgCropImage[i].allocate(targetRect.getWidth(), targetRect.getHeight());
            rgCropImage[i].begin();
                ofClear(0);
                img.draw(workingSubjectRect);
            rgCropImage[i].end();
            
            targetRect.set(i*w, 0, w, h);
        }
        
    }
    highLightIndex = ofRandom(0, _size-1);
    appearTime = ofGetElapsedTimef();
}
void GrahicScene::update()
{
    if(shareData->graphicIndex>=0)
    {
        shareData->nextScent = 2;
    }
    if(ofGetElapsedTimef() - appearTime > refrashTime)
    {
        highLightIndex = (highLightIndex+1) % rgCropImage.size();
        appearTime = ofGetElapsedTimef();
    }
}

void GrahicScene::draw()
{
    ofBackground(ofColor::white);
    
    for (int i=0; i < rgCropImage.size(); i++)
    {
        ofRectangle& targetRect = shareData->rgImgRect[i];
        rgCropImage[i].draw(targetRect.x, targetRect.y);
        if(i != highLightIndex)
        {
            ofPushStyle();
                ofSetColor(ofColor::black, 128);
                ofRect(targetRect);
            ofPopStyle();
        }
    }

    ofDrawBitmapStringHighlight("Select base symphony", 4, 140);
}

//--------------------------------------------------------------
void GrahicScene::mouseReleased(int x, int y, int button)
{
    for (int i=0; i < shareData->rgImgRect.size(); i++)
    {
        if (shareData->rgImgRect[i].inside(x, y))
        {
            shareData->graphicIndex = i;
            return;
        }
    }
    shareData->graphicIndex = -1;
    
}

//--------------------------------------------------------------
void GrahicScene::mouseMoved(int x, int y )
{
    for (int i=0; i < shareData->rgImgRect.size(); i++)
    {
        ofRectangle& targetRect = shareData->rgImgRect[i];
        if(targetRect.inside(x, y))
        {
            highLightIndex = i;
            appearTime = ofGetElapsedTimef();
        }
    }
    
}

