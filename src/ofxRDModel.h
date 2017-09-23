#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofxRDModel {
    const vector<string> formulas;
    const ofParameterGroup variables;
    const ofParameter<ofVec3f> cellParam;
    const ofVec3f defaultParam;
    const ofParameter<int> componentColor;
    const ofParameter<int> componentHeight;
    
public:
    ofxRDModel(vector<string> _formulas,
               ofParameterGroup _variables,
               ofParameter<ofVec3f> _cellParam,
               ofVec3f _defaultParam,
               ofParameter<int> _componentColor,
               ofParameter<int> _componentHeight):
    formulas(_formulas),
    variables(_variables),
    cellParam(_cellParam),
    defaultParam(_defaultParam),
    componentColor(_componentColor),
    componentHeight(_componentHeight) {}
    
    const vector<string>& getFormulas() const { return formulas; }
    const ofParameterGroup& getVariables() const { return variables; }
    
    const ofVec3f &getDefaultParam() const { return defaultParam; }
    const ofVec3f &getInitParam() const { return cellParam; }
    const ofVec3f &getMaxParam() const { return cellParam.getMax(); }
    const ofVec3f &getMinParam() const { return cellParam.getMin(); }
    const ofParameter<int> &getComponentColor() const { return componentColor; }
    const ofParameter<int> &getComponentHeight() const { return componentHeight; }
};

class ofxRDModelLoader {
private:
    ofxRDModelLoader();
public:
    static ofxRDModel* load(string path);
};

class ModelLoadFailedException : public std::exception {
public:
    ModelLoadFailedException( const std::string &str ) : m_error(str){}
    ~ModelLoadFailedException(){};
    
    const char* what( void ) const noexcept { return m_error.c_str(); }
private:
    std::string m_error;
};
