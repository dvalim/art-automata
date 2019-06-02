#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.visible = false;
    ofCreateWindow(settings);
    ofRunApp(new ofApp);
    
}
