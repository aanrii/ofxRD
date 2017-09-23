#pragma once

#include "ofMain.h"
#include "ofxRDModel.h"
#include "pingPong.h"

class ofxRD {
    int width, height;
    ofxRDModel* model;
    ofShader updateShader;
    pingPongBuffer cellMap;
    ofVboMesh mesh;
    ofParameterGroup variables;
    bool updatedVariables;
    vector<ofParameterGroup> cellClusters;

    void setupShader();
    
public:
    ofxRD();
    
    void init(int _width, int _height, string modelPath);
    void initMap();
    
    void update(ofEventArgs &e);
    void draw();

    int getWidth(){ return width; }
    int getHeight(){ return height; }
    
    ofxRDModel* getModel() { return model; }
    ofParameterGroup& getVariables() { return variables; }
    ofTexture& getMapTexture(){ return cellMap.src->getTexture(); }

    void addCellCluster(ofVec2f& pos, int radius);
    
    void setValue(ofAbstractParameter &parameter){ updatedVariables = true; }
};

