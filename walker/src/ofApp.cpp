//this was inspired by a gif i saw on twitter once
//i never found it again though and can't remember the author

#include "ofApp.h"
#include <random>

ofTrueTypeFont font;
ofFbo buffer;
int seed;
std::string seedstring;

std::vector<int> angles;

int angle() {
    return angles[(int)ofRandom(angles.size())];
}

const int width = 2000;
const int height = 2000;

int vis[width][height];

double speed = 0.15;
double spawn_chance = 0.003;
double acceleration = 0.1; //circle radius
double circle_chance = 0.5;
double distortion = 1;
double distort_level = 0;
double warp = 0;
int parameter_changes = 0;
double time_limit = 12;
double change_time = 0;
double direction = 1;
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

void randomiseParameters() {
    acceleration = ofRandom(0.005, 0.4);
    circle_chance = ofRandom(-0.2, 0.4);
    distortion = ofRandom(1.0);
    distort_level = max(0.0, min(1.0, gaussian(0, 0.25)));
    warp = max(0.0, min(1.0, gaussian((ofRandom(1.0) <= 0.3 ? 1 : 0), 1)));
    if(ofRandom(1.0) <= 0.07) circle_chance = 0.98;
    speed = ofRandom(0.1, 0.35);
    spawn_chance = ofRandom(0.001, 0.015);
    direction = (ofRandom(1.0) <= 0.5 ? 1 : -1);
}

int counter = 1;

struct walker {
    double x, y, angle, speed_mult = 1;
    int shade = 0, circular = 0, parent, id;
    std::set<int> children;
    walker(double x, double y, double angle, int parent, int id) : x(x), y(y), angle(angle), parent(parent), id(id) {
        if(ofRandom(1.0) <= circle_chance) circular = (ofRandom(1.0) >= 0.5 ? -1 : 1);
        speed_mult = ofRandom(0.2, 2);
        if(ofRandom(1.0) <= 0.025) speed_mult = 8;
        shade = ofRandom(80);
    }
    bool update() {
        if(!circular && ofRandom(1.0) <= 0.0005) circular = (ofRandom(1.0) >= 0.5 ? -1 : 1);
        if(ofRandom(1.0) <= 0.002) circular *= -1; //chance to change direction
        if(circular) angle += circular*acceleration;
        double noise = ofNoise(x/width, y/height, noise_seed) * distortion;
        double distort = ofMap(noise, 0, 1, 1, ofRandom(1.3));
        double aa = (angle*PI/180 + distort*warp)*(1-distort*distort_level);
        x += cos(aa) * speed * speed_mult * direction;
        y += sin(aa) * speed * speed_mult * direction;
        if(ofRandom(1.0) <= spawn_chance) return 1; //spawn child
        return 0;
    }
    void draw() {
        buffer.begin();
        ofSetColor(shade);
        if(x > 100+gaussian(0, 2) && x < width-100+gaussian(0, 2) && y > 100+gaussian(0, 2) && y < height-100+gaussian(0, 2)) //borders
            ofDrawRectangle(x+gaussian(0, 1)*0.05, y+gaussian(0, 1)*0.05, 1+gaussian(0, 0.5), 1+gaussian(0, 0.5));
        buffer.end();
    }
};

std::vector<walker> walkers;

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
    
    std::vector<int> possible_angles = {20, 45, 60, 90, 120, 160};
    for(int i = 1; i <= 2; i++)
        angles.push_back(possible_angles[(int)ofRandom(possible_angles.size())]);
    
    if(ofRandom(1.0) <= 0.35) {
        parameter_changes = (int)ofRandom(3)+1;
        change_time = time_limit/(parameter_changes+1);
    }

    randomiseParameters();
    
    int starting = (int)ofRandom(40)+1; //inital walkers
    for(int i = 1; i <= starting; i++)
        walkers.push_back(walker(ofRandom(width), ofRandom(height), (int)ofRandom(10)*angle(), 0, ++counter));
    
    buffer.begin();
    ofBackground(255);
    ofSetColor(0);
    drawStringCentered(seedstring, width/2, height-50);
    for(int x = 0; x <= width; x++) //grain
        for(int y = 0; y <= height; y++) {
            int shade = gaussian(240, 30);
            ofSetColor(shade, shade, shade, 60);
            ofDrawRectangle(x, y, 1, 1);
        }
    buffer.end();
}

bool changed = 0;

//--------------------------------------------------------------
void ofApp::update(){
    if(parameter_changes &&
       ofGetElapsedTimef()>=change_time) {
        change_time += time_limit/(parameter_changes+1);
        randomiseParameters();
    }
    
    for(int i = walkers.size()-1; i >= 0; i--) { //loop backwards to avoid skipping
        if(walkers[i].update()) { //if child is spawned
            walkers.push_back(walker(walkers[i].x, walkers[i].y, walkers[i].angle + (ofRandom(1.0) < 0.5 ? 1 : -1)*angle(), walkers[i].id, ++counter));
            walkers[i].children.insert(counter);
        }
        
        if(walkers[i].x < 0 || walkers[i].x > width || walkers[i].y < 0 || walkers[i].y > height || //check if a line has been hit
           (
            vis[(int)walkers[i].x][(int)walkers[i].y] != 0 && //pixel is occupied
            vis[(int)walkers[i].x][(int)walkers[i].y] != walkers[i].id && //does not belong to us
            vis[(int)walkers[i].x][(int)walkers[i].y] != walkers[i].parent && //our parent
            !walkers[i].children.count(vis[(int)walkers[i].x][(int)walkers[i].y])) ) //or our child
                walkers.erase(walkers.begin()+i); //then die
        else vis[(int)walkers[i].x][(int)walkers[i].y] = walkers[i].id; //else mark it as occupied
    }
    
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
    for(auto w : walkers)
        w.draw();
    
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
