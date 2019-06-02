#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    //settings.visible = false; //uncomment to hide window
    ofCreateWindow(settings);
    ofRunApp(new ofApp);

}
