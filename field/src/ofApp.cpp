#include "ofApp.h"
#include <random>
#include "flame.h"

ofTrueTypeFont font;
ofFbo buffer;
int seed;
std::string seedstring;

std::vector<int> angles;

const int width = 2000;
const int height = 2000;
double time_limit = 25;

std::mt19937 engine;

double gaussian(double mean, double deviation) {
    std::normal_distribution<double> nd(mean, deviation);
    return nd(engine);
}

ofVec2f getOffset( string s ){
    ofRectangle r = font.getStringBoundingBox(s, 0, 0);
    return ofVec2f( floor(-r.x - r.width * 0.5f), floor(-r.y - r.height * 0.5f) );
}

void drawStringCentered(string s, float x, float y){
    ofVec2f offset = getOffset(s);
    font.drawString(s, x + offset.x, y + offset.y);
}

ofVec3f cam;
vector<fract> fractals;
vector<string> vars;

//these are for depth of field, refer to inconvergent's tutorial
//https://inconvergent.net/2019/depth-of-field/

double m = 0.07, e = 1.5;
double f = 0.85;
double fov = 15;

double noise_seed;

bool mult;

//--------------------------------------------------------------
void ofApp::setup() {
    font.load("sans.ttf", 30);
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    ofSeedRandom(seed);
    engine.seed(seed);
    noise_seed = ofRandom(1000);
    std::stringstream sstream;
    sstream << std::hex << seed;
    seedstring = sstream.str();
    
    ofSetWindowShape(width, height);
    ofSetBackgroundAuto(false);
    buffer.allocate(width, height);
    
    buffer.begin();
    ofBackground(10);
    ofSetColor(240);
    drawStringCentered(seedstring, width/2, height-50);
    buffer.end();
    
    if(ofRandom(1) <= 0.4) mult = 1;

    cam = ofVec3f(gaussian(0, 0.35), gaussian(0, 0.35), gaussian(0, 0.35)); //position camera close to origin
    
    int var_number = ofRandom(2, 7.99);
    for(int i = 1; i <= var_number; i++)
        vars.push_back(randVariation());
    
    double hue = ofRandom(1);
    base_hues = {hue, fmod(hue+ofRandom(0.4, 0.6), 1), ofRandom(1)};
    
    int fract_number = 20;
    for(int i = 1; i <= fract_number; i++)
        fractals.push_back(fract(vars));
    
    m = ofRandom(0.01, 0.0265);
    e = ofRandom(1.2, 2.1);
    f = ofRandom(0.75, 1.75);
    
    fov = ofRandom(2, 6);
    
    
}

//--------------------------------------------------------------
void ofApp::update() {
    if(ofGetElapsedTimef()>=time_limit) {
        ofPixels pix;
        buffer.readToPixels(pix);
        ofSaveImage(pix,"../images/"+seedstring+".jpg");
        cout << seedstring;
        ofExit();
    }
}

int iterations = 800;
int samples = 10;

ofVec3f rndSphere(double r) {
    double phi = ofRandom(2*PI);
    double costheta = ofRandom(-1, 1);
    double u = ofRandom(0, 1);
    
    double theta = acos(costheta);
    double rad = r * cbrt(u);

    return ofVec3f(rad*sin(theta)*cos(phi), rad*sin(theta)*sin(phi), rad*cos(theta));
}

//--------------------------------------------------------------
void ofApp::draw() {
    buffer.begin();
    for(int i = 1; i <= iterations; i++) {
        for(int j = 0; j < fractals.size(); j++) {
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            ofVec3f p = fractals[j].step();
            double hue = fractals[j].hue;
            double sat = min(fractals[j].sat+0.2, 0.8);
            ofFloatColor c;
            c.setHsb(hue, sat, 1);
            double d = sqrt(pow(p.x-cam.x, 2) + pow(p.y-cam.y, 2) + pow(p.z-cam.z, 2));
            c.a = 0.05/max(1.0, d);
            ofSetColor(c);
            double r = m*pow(abs(f-d), e);
            for(int k = 1; k <= samples; k++) {
                auto w = p+rndSphere(r);
                double xx = ((w.x-cam.x)/(w.z-cam.z)*(mult ? d : 1/d)+cam.x)*width/fov+width/2;
                double yy = ((w.y-cam.y)/(w.z-cam.z)*(mult ? d : 1/d)+cam.y)*height/fov+height/2;
                if(xx > 100+gaussian(0, 2) && xx < width-100+gaussian(0, 2) && yy > 100+gaussian(0, 2) && yy < height-100+gaussian(0, 2)) //borders
                    ofDrawRectangle(xx, yy, 1, 1);
            }
        }
    }
    buffer.end();
    
    ofSetColor(255);
    buffer.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
