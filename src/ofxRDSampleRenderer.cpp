#include "ofxRDSampleRenderer.h"

ofxRDSampleRenderer::ofxRDSampleRenderer() : updatedOptions(true) {
    options.add(ofParameter<float>().set("meshDepth", 100.0f, 0.0f, 400.f));
    options.add(enableBicubic.set("enableBicubic", false));
    options.add(enableHeatMap.set("enableHeatMap", true));
    
    auto colorArray = new ofVec4f[6] {
        ofVec4f(0.0f, 0.0f, 0.0f, 1.0f),
        ofVec4f(0.0f, 1.0f, 0.0f, 1.0f),
        ofVec4f(1.0f, 1.0f, 0.0f, 1.0f),
        ofVec4f(1.0f, 0.0f, 0.0f, 1.0f),
        ofVec4f(1.0f, 0.0f, 1.0f, 1.0f),
        ofVec4f(1.0f, 1.0f, 1.0f, 1.0f)
    };
    for (int i = 0; i < 6; i ++) {
        for (int j = 0; j < 4; j ++){
            colors[i * 4 + j] = (colorArray[i])[j];
        }
    }
    auto posArray = new float[6] {0.0f, 0.1f , 0.11f, 0.4f, 0.6f, 1.0f};
    memcpy(colors_pos, posArray, sizeof(colors_pos));
}

void ofxRDSampleRenderer::initMesh(float scale) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            mesh.addVertex(ofVec3f(j * scale, i * scale, 0));
        }
    }
    for (int i = 0; i < height - 1; i++) {
        for (int j = 0; j < width - 1; j++) {
            mesh.addIndex(j + i * width);
            mesh.addIndex(j + (i + 1) * width);
            mesh.addIndex((j + 1) + (i + 1) * width);
            mesh.addIndex(j + i * width);
            mesh.addIndex((j + 1) + (i + 1) * width);
            mesh.addIndex((j + 1) + i * width);
        }
    }
    mesh.setMode(OF_PRIMITIVE_TRIANGLES);
}

void ofxRDSampleRenderer::initBicubicWeights(float resolution) {
    int bicubicWeightSize = (int)(2.0 / resolution);
    
    vector<float> bicubicWeightVec(bicubicWeightSize);
    for (int i = 0; i < bicubicWeightSize; i++) {
        float d = i * resolution;
        bicubicWeightVec[i] =
        (d <= 1.0f) ? (bicubicALPHA + 2.0f) * pow(d, 3.0f) - (bicubicALPHA + 3.0f) * pow(d, 2.0f) + 1.0f:
        (d <= 2.0f) ? (bicubicALPHA * pow(d, 3.0f) - 5.0f * bicubicALPHA * pow(d, 2.0f) + 8.0f * bicubicALPHA * d - 4.0f * bicubicALPHA):
        0.0f;
    }
    
    bicubicWeights.allocate(bicubicWeightSize, 1, GL_R32F);
    bicubicWeights.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    bicubicWeights.loadData(bicubicWeightVec.data(), bicubicWeightSize, 1, GL_RED);
}

void ofxRDSampleRenderer::init(ofxRD& _rd, int _width, int _height, int _component_color, int _component_depth, float scale){
    width = _width;
    height = _height;
    mapTexture = _rd.getMapTexture();
    
    render.load("shaders/sampleRenderer.vert", "shaders/sampleRenderer.frag");
    
    resolution = _rd.getWidth() / (float)width;
    
    initMesh(scale);
    initBicubicWeights(resolution);

    ofParameter<int> componentColor(_rd.getModel()->getComponentColor());
    ofParameter<int> componentHeight(_rd.getModel()->getComponentHeight());
    options.add(componentColor);
    options.add(componentHeight);

    ofAddListener(options.parameterChangedE(), this, &ofxRDSampleRenderer::setValue);
    
    render.begin();
    render.setUniform4fv("colors", colors, 6);
    render.setUniform1fv("colors_pos", colors_pos, 6);
    render.setUniform1f("resolution", resolution);
    render.setUniform3f("minParam", _rd.getModel()->getMinParam());
    render.setUniform3f("maxParam", _rd.getModel()->getMaxParam());
    render.setUniformTexture("bicubicWeights",
                             bicubicWeights,
                             bicubicWeights.getTextureData().textureID);
    render.end();
}

void ofxRDSampleRenderer::draw(){
    render.begin();
    
    render.setUniformTexture("cellMap", mapTexture, 0);

    if (updatedOptions) {
        render.setUniform1i(enableBicubic.getName(), enableBicubic);
        render.setUniform1i(enableHeatMap.getName(), enableHeatMap);
        render.setUniforms(options);
        updatedOptions = false;
    }

    ofPushStyle();
    ofEnableDepthTest();
    
    ofSetColor(255);
    mesh.draw();
    glEnd();
    
    render.end();
    ofDisableDepthTest();
    ofPopStyle();
}
