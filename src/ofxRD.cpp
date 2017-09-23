#include "ofxRD.h"
#include <cmath>

ofxRD::ofxRD():updatedVariables(true) {}

void ofxRD::init(int _width, int _height, string modelPath){
    width = _width;
    height = _height;
    
    model = ofxRDModelLoader::load(modelPath);
    
    variables = model->getVariables();
    variables.add(ofParameter<bool>().set("run", true));
    variables.add(ofParameter<int>().set("repeat", 3, 0, 10));
    ofAddListener(variables.parameterChangedE(), this, &ofxRD::setValue);

    initMap();
    setupShader();

    ofAddListener(ofEvents().update, this, &ofxRD::update, OF_EVENT_ORDER_AFTER_APP);
}

void ofxRD::initMap() {
    auto initialMap = new float[width * height * 4]();
    auto initParam = model->getInitParam();
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            initialMap[(j + i * width) * 4] = initParam.x;
            initialMap[(j + i * width) * 4 + 1] = initParam.y;
            initialMap[(j + i * width) * 4 + 2] = initParam.z;
            initialMap[(j + i * width) * 4 + 3] = 0;
        }
    }
    cellMap.allocate(width, height, GL_RGBA32F);
    cellMap.src->getTexture().loadData(initialMap, width, height, GL_RGBA);
    
    delete[] initialMap;
}

void ofxRD::setupShader() {
    updateShader.setupShaderFromFile(GL_VERTEX_SHADER, "shaders/passthru.vert");
    
    auto buffer = ofBufferFromFile("shaders/updateMap.frag");
    
    string optionStr = "";
    for (auto variable: model->getVariables()){
        optionStr += "uniform float " + variable->getName() + ";\n";
    }

    auto formulas = model->getFormulas();
    string formulaStr = "";
    formulaStr += formulas[0] + ",\n";
    formulaStr += formulas[1] + ",\n";
    formulaStr += formulas[2] + "\n";
     
    string source = buffer.getText();
    source.replace(source.find("// replace to options\n"), strlen("// replace to options\n"), optionStr);
    source.replace(source.find("// replace to formulas\n"), strlen("// replace to formulas\n"), formulaStr);
    
    updateShader.setupShaderFromSource(GL_FRAGMENT_SHADER, source);
    
    if(ofIsGLProgrammableRenderer()){
        updateShader.bindDefaults();
    }
    updateShader.linkProgram();
    
    updateShader.begin();
    updateShader.setUniform3f("maxParam", model->getMaxParam());
    updateShader.setUniform3f("minParam", model->getMinParam());
    updateShader.end();
}

void ofxRD::update(ofEventArgs &e) {
    if (variables.getBool("run")) {
        updateShader.setUniform1f("timeValue", (ofGetElapsedTimeMillis() % 20000 + 10000) / 30000.0f);
        
        for (int i = 0; i < (int)pow(2.0, (double)variables.getInt("repeat")); i++){
            cellMap.dst->begin();
            ofClear(0);
            updateShader.begin();
            
            if (!cellClusters.empty()){
                updateShader.setUniforms(cellClusters.front());
                cellClusters.erase(cellClusters.begin());
            } else {
                updateShader.setUniform2f("cellClusterPos", ofVec2f(0, 0));
                updateShader.setUniform1i("cellClusterRadius", 0);
                updateShader.setUniform3f("cellClusterParam", ofVec3f(0, 0, 0));
            }

            if (updatedVariables) {
                updateShader.setUniforms(model->getVariables());
                updatedVariables = false;
            }
            
            updateShader.setUniformTexture("cellMap", cellMap.src->getTexture(), 0);
            cellMap.src->draw(0, 0);
            updateShader.end();
            cellMap.dst->end();
            cellMap.swap();
        }
    }
}

void ofxRD::draw() {
    ofPushStyle();
    
    ofSetColor(255);
    cellMap.src->getTexture().draw(0, 0);
    
    ofPopStyle();
}

void ofxRD::addCellCluster(ofVec2f& pos, int radius) {
    ofParameterGroup group;
    group.add(ofParameter<ofVec2f>().set("cellClusterPos", pos));
    group.add(ofParameter<int>().set("cellClusterRadius", radius));
    group.add(ofParameter<ofVec3f>().set("cellClusterParam", model->getDefaultParam()));
    cellClusters.push_back(group);
}
