#include "ofApp.h"
#include <random>

ofTrueTypeFont font;
ofFbo buffer;
int seed;
std::string seedstring;

const int width = 2000;
const int height = 2000;

int res = 300;
int states[2000][2000];

double time_limit = 1;
double noise_seed;

std::mt19937 engine;

double gaussian(double mean, double deviation) {
    std::normal_distribution<double> nd(mean, deviation);
    return nd(engine);
}

ofVec2f getOffset(string s){
    ofRectangle r = font.getStringBoundingBox(s, 0, 0);
    return ofVec2f( floor(-r.x - r.width * 0.5f), floor(-r.y - r.height * 0.5f) );
}

void drawStringCentered(string s, double x, double y){
    ofVec2f offset = getOffset(s);
    ofSetColor(0);
    font.drawString(s, x + offset.x, y + offset.y);
}

int getState(int n, int x, int y) {
    int l = 0, m, r = 0;
    if(x > 0) l = states[x - 1][y];
    if(x < res - 1) r = states[x + 1][y];
    m = states[x][y];
    
    int pos = (l << 2) + (m << 1) + r;
    return (n >> pos) & 1;
}

double coord_scale;
double rnd_noise;

double getNoise(int _x, int _y) {
    double x = _x, y = _y;
    double noise = ofNoise(x/res * coord_scale, y/res * coord_scale, noise_seed);
    noise += 0.5 * ofNoise(x/res * coord_scale * 2, y/res * coord_scale * 2, noise_seed);
    noise += 0.25 * ofNoise(x/res * coord_scale * 4, y/res * coord_scale * 4, noise_seed);
    noise += 0.125 * ofNoise(x/res * coord_scale * 8, y/res * coord_scale * 8, noise_seed);
    noise = min(1.0, noise * gaussian(1, rnd_noise));
    return noise;
}

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
    buffer.allocate(width, height);
    
    res = ofRandom(150, 400);
    int n = ofRandom(2, 8);
    coord_scale = ofRandom(1, 3);
    rnd_noise = ofRandom(0.001, 0.1);
    
    vector<int> rules = { (int)ofRandom(256) };
    for(int i = 1; i < n; i++) rules.push_back(ofRandom(256));
    
    for(int x = 0; x < res; x++)
        states[x][0] = (ofRandom(1) <= 0.5 ? 1 : 0);
    for(int y = 1; y < res; y++)
        for(int x = 0; x < res; x++) {
            double noise = getNoise(x, y);
            int ind = (n - 1) * noise;
            states[x][y] = getState(rules[ind], x, y - 1);
        }
    
    buffer.begin();
    ofBackground(255);
    ofSetColor(0);
    drawStringCentered(seedstring, width/2, height-50);
    for(int x = 100; x <= width-100; x++) //grain
        for(int y = 100; y <= height-100; y++) {
            int shade = gaussian(240, 30);
            ofSetColor(shade, shade, shade, 60);
            ofDrawRectangle(x, y, 1, 1);
        }

    double cell_size = (width - 200.0)/res;
    for(double y = 100, y_ind = 0; y_ind < res ; y += cell_size, y_ind++)
        for(double x = 100, x_ind = 0; x_ind < res ; x += cell_size, x_ind++) {
            //ofSetColor(states[(int)x_ind][(int)y_ind], states[(int)x_ind][(int)y_ind], states[(int)x_ind][(int)y_ind]);
            ofSetColor(16, 16, 16);
            if(states[(int)x_ind][(int)y_ind]) {
                ofDrawRectangle(x, y, cell_size, cell_size);
                //double noise = getNoise(res-x_ind, res-y_ind);
            }
        }
    buffer.end();
}


//--------------------------------------------------------------
void ofApp::update(){
    if(ofGetElapsedTimef() >= time_limit) {
        ofPixels pix;
        buffer.readToPixels(pix);
        ofSaveImage(pix,"../images/"+seedstring+".jpg");
        cout << seedstring;
        ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
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
