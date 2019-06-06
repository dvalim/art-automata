//refer to generateme's blog post on vector fields, this is a 3d version of it
//https://generateme.wordpress.com/

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
double fov = 2;

vector<particle> particles;

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
    ofBackground(20);
    ofSetColor(240);
    drawStringCentered(seedstring, width/2, height-50);
    buffer.end();
    
    cam = ofVec3f(gaussian(0, 0.01), gaussian(0, 0.01), -4); //position camera close to origin
    
    fov = 0.3;
    speed = ofRandom(0.015, 0.28);
    
    int formula_length = ofRandom(1, 5);
    for(int i = 1; i <= formula_length; i++)
        formula.push_back({randVariation(), (int)ofRandom(5)});
    
    for(int i = 1; i <= 12; i++) //parameters used by some variations
        aff.push_back(ofRandom(1.2)*(ofRandom(1) <= 0.5 ? -1 : 1));
    
    double hue = ofRandom(1);
    hues = {hue, fmod(hue+ofRandom(0.4, 0.6), 1), ofRandom(1)};
    
    //setup particles
    double step = 0.15;
    for(double x = -4; x <= 4; x += step)
        for(double y = -4; y <= 4; y += step)
            for(double z = -2; z <= 6; z += step)
                particles.push_back(particle(ofVec3f(x, y, z)));
    
    
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

//--------------------------------------------------------------
void ofApp::draw() {
    buffer.begin();
    for(auto &i : particles) {
        ofEnableBlendMode(OF_BLENDMODE_ADD);
        i.update();
        auto p = i.pos;
        double d = sqrt(pow(p.x-cam.x, 2) + pow(p.y-cam.y, 2) + pow(p.z-cam.z, 2)); //distance from camera
        ofFloatColor c;
        c.setHsb(i.hue, min(i.sat+0.2, 0.8), 1);
        c.a = 0.2/d;
        ofSetColor(c);
        //ofSetColor(ofFloatColor(0, 0, 0, 0.1/d));
        //3d projection
        double xx = ((p.x-cam.x)/(p.z-cam.z)/d+cam.x)*width/fov+width/2;
        double yy = ((p.y-cam.y)/(p.z-cam.z)/d+cam.y)*height/fov+height/2;
        if(xx > 100+gaussian(0, 2) && xx < width-100+gaussian(0, 2) && yy > 100+gaussian(0, 2) && yy < height-100+gaussian(0, 2)) //borders
            ofDrawRectangle(xx, yy, 1, 1);
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
