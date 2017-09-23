#include "ofxRDModel.h"

ofxRDModel* ofxRDModelLoader::load(string path) {
    ofxXmlSettings xml;
    if (!xml.loadFile(path)) {
        throw new ModelLoadFailedException("There is no xml file at " + path);
    }
    xml.pushTag("model");
    
    vector<string> formulas(3);
    xml.pushTag("formulas");
    formulas[0] = xml.getValue("formula_x", "0");
    formulas[1] = xml.getValue("formula_y", "0");
    formulas[2] = xml.getValue("formula_z", "0");
    xml.popTag();
    
    ofParameterGroup variables;
    xml.pushTag("variables");
    int numVariables = xml.getNumTags("variable");
    for (int i = 0; i < numVariables; i++) {
        ofParameter<float> variable(xml.getAttribute("variable", "name", "", i),
                                    (float)xml.getAttribute("variable", "value", 0.0f, i),
                                    (float)xml.getAttribute("variable", "min", 0.0f, i),
                                    (float)xml.getAttribute("variable", "max", 0.0f, i)
                                    );
        variables.add(variable);
    }
    xml.popTag();

    xml.pushTag("params");
    string componentColorString = xml.getAttribute("component", "color", "x");
    ofParameter<int> componentColor("componentColor",
                                    componentColorString == "z" ? 2 :
                                    componentColorString == "y" ? 1 : 0, 0, 2);
    string componentHeightString = xml.getAttribute("component", "height", "x");
    ofParameter<int> componentHeight("componentHeight",
                                     componentHeightString == "z" ? 2 :
                                     componentHeightString == "y" ? 1 : 0, 0, 2);
    
    ofParameter<ofVec3f> cellParam("",
                                   ofVec3f(xml.getAttribute("init", "x", 0.0f),
                                           xml.getAttribute("init", "y", 0.0f),
                                           xml.getAttribute("init", "z", 0.0f)),
                                   ofVec3f(xml.getAttribute("min", "x", 0.0f),
                                           xml.getAttribute("min", "y", 0.0f),
                                           xml.getAttribute("min", "z", 0.0f)),
                                   ofVec3f(xml.getAttribute("max", "x", 0.0f),
                                           xml.getAttribute("max", "y", 0.0f),
                                           xml.getAttribute("max", "z", 0.0f))
                                   );
    
    ofVec3f defaultParam(xml.getAttribute("default", "x", 0.0f),
                         xml.getAttribute("default", "y", 0.0f),
                         xml.getAttribute("default", "z", 0.0f));
    
    return new ofxRDModel(formulas,
                          variables,
                          cellParam,
                          defaultParam,
                          componentColor,
                          componentHeight);
}
