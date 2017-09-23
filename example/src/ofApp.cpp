#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetWindowPosition(0, 0);
    ofBackground(0);
    
    auto planeSize = ofGetWindowSize() * planeSizeRatio;
    auto texSize = planeSize * texResolution;
    
    // rd.init(texSize.x, texSize.y, "models/fitzhugh_nagumo.xml");
    rd.init(texSize.x, texSize.y, "models/gray_scott.xml");
    renderer.init(rd, planeSize.x, planeSize.y, 0);
    
    auto centerPos = planeSize / 2.0;
    eCam.setPosition(centerPos.x, centerPos.y, -1);
    eCam.setTarget(ofVec3f(centerPos.x, centerPos.y, 0.0));
    eCam.enableMouseInput();
    eCam.setDistance(500);
    eCam.roll(180);
    
    panelRD.setup();
    panelRD.add(rd.getVariables());
    
    panelRenderer.setup();
    panelRenderer.add(renderer.getOptions());
    panelRenderer.setPosition(panelRD.getPosition().x, panelRD.getPosition().y + panelRD.getHeight());
    
    ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::update(){
    if (isMouseOn){
        auto mousePos = ofVec2f(ofGetMouseX(), ofGetMouseY()) * planeSizeRatio * texResolution;
        rd.addCellCluster(mousePos, 5);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    eCam.begin();
    renderer.draw();
    eCam.end();
    
    panelRD.draw();
    panelRenderer.draw();
    
    ofDrawBitmapString("Fps: " + ofToString(ofGetFrameRate()), ofGetWindowWidth() - 100, 15);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'i':
            rd.initMap();
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (button == OF_MOUSE_BUTTON_RIGHT){
        isMouseOn = true;
        eCam.disableMouseInput();
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (isMouseOn) {
        isMouseOn = false;
        eCam.enableMouseInput();
    }
}


