#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.visible = false; //uncomment to hide the window
    ofCreateWindow(settings);
    ofRunApp(new ofApp);
}
