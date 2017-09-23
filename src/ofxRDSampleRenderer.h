#pragma once

#include "ofxRD.h"

class ofxRDSampleRenderer{
    int width, height;
    ofVboMesh mesh;
    ofShader render;
    ofTexture mapTexture;
    ofTexture bicubicWeights;
    float resolution;
    float colors[24];
    float colors_pos[6];
    ofParameter<bool> enableBicubic;
    ofParameter<bool> enableHeatMap;
    ofParameterGroup options;
    bool updatedOptions;
    
    void initMesh(float resolution);
    
    float bicubicALPHA = -0.5;
    void initBicubicWeights(float resolution);
    
public:
    ofxRDSampleRenderer();
    void init(ofxRD& _rd, int _width, int _height, int _component_color, int _component_depth = 0, float scale = 1.0);
    void draw();
    int getWidth() { return width; }
    int getHeight() { return height; }
    ofParameterGroup& getOptions() { return options; }
    
    void setValue(ofAbstractParameter &parameter) {
        updatedOptions = true;
    }
};
