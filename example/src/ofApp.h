#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxRD.h"
#include "ofxRDSampleRenderer.h"

class ofApp : public ofBaseApp{
    float planeSizeRatio = 0.75;
    float texResolution = 0.25;
    
    ofxRD rd;
    ofxRDSampleRenderer renderer;
    
    ofEasyCam eCam;
    ofxPanel panelRD;
    ofxPanel panelRenderer;
    
    bool isMouseOn;
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
};
