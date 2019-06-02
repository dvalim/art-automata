//refer to tyler hobbs' tutorial on drawing generative watercolor
//https://tylerxhobbs.com/essays/2017/a-generative-approach-to-simulating-watercolor-paints

#include "ofApp.h"
#include <random>

//--------------------------------------------------------------

int vertex_count = 6;
int layers = 8;
int frames_per_layer = 55;
int seed;
std::string seedstring;

const int width = 2000;
const int height = 2000;

std::vector<int> hues;
std::vector<ofFloatColor> colors;
std::vector<int> framecount;
std::mt19937 engine;

double initial_ydeviation = 40;
double variation_deviation = 0.3;
double depression_chance = 0.05; //lmao
double variance_mult = 0.6; //very important

ofTrueTypeFont font;

ofVec2f getOffset( string s ){
    ofRectangle r = font.getStringBoundingBox(s, 0, 0);
    return ofVec2f( floor(-r.x - r.width * 0.5f), floor(-r.y - r.height * 0.5f) );
}

void drawStringCentered(string s, float x, float y){
    ofVec2f offset = getOffset(s);
    ofSetColor(0);
    font.drawString(s, x + offset.x, y + offset.y);
}

double gaussian(double mean, double deviation) {
    std::normal_distribution<double> nd(mean, deviation);
    return nd(engine);
}

struct polygon {
    std::vector<std::pair<double, double> > vertices;
    std::vector<double> variation;
    polygon(double y) {
        for(int i = 0; i <= vertex_count; i++) {
            vertices.emplace_back(width/vertex_count*i+gaussian(0, 1)*20, y+gaussian(0, 1)*initial_ydeviation);
            double var = std::min(std::max(0.0, (ofRandom(1.0) > depression_chance ? gaussian(1, variation_deviation) : 0)), 1.0);
            variation.push_back(var);
        }
        
        y += 400;
        for(int i = vertex_count; i >= 0; i--) {
            vertices.emplace_back(width/vertex_count*i+gaussian(0, 1)*20, y+gaussian(0, 1)*initial_ydeviation);
            double var = std::min(std::max(0.0, gaussian(1, 0.1)), 1.0);
            variation.push_back(var);
        }
    }
    void deform(int count) {
        for(int I = 1; I <= count; I++) {
            for(int i = 0; i < vertices.size()-1; i+=2) {
                int j = i+1;
                auto v = vertices[i], w = vertices[j];
                double xx = v.first/2+w.first/2; //middle of line
                double yy = v.second/2+w.second/2;
                double new_var = (variation[i]/2+variation[j]/2)*0.9;
                double variance =  variance_mult * new_var * sqrt( //variance depends on length of line
                  pow(vertices[i].first-vertices[j].first, 2) +
                  pow(vertices[i].second-vertices[j].second, 2)
                );
                std::pair<double, double> new_v = {
                    xx + gaussian(0, 1)*variance, yy + gaussian(0, 1)*variance
                };
                vertices.insert(vertices.begin()+i+1, new_v);
                variation.insert(variation.begin()+i+1, new_var);
            }
        }
    }
};

std::vector<polygon> polygons;

void ofApp::setup(){
    font.load("sans.ttf", 30);
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    ofSeedRandom(seed);
    engine.seed(seed);
    std::stringstream sstream;
    sstream << std::hex << seed;
    seedstring = sstream.str();
    
    //randomize variables
    initial_ydeviation = ofRandom(30, 50);
    variation_deviation = ofRandom(0.2, 0.4);
    depression_chance = ofRandom(0.02, 0.08);
    variance_mult = ofRandom(0.55, 0.65);
    
    ofSetWindowShape(width, height);
    ofSetBackgroundAuto(false);
    
    buffer.allocate(width, height);
    
    //set up color palette
    hues = {(int)ofRandom(255), (int)ofRandom(255)};
    hues.push_back((hues[0]+127+(int)ofRandom(10))%255);
    //set up initial polygons
    for(int i = 1; i <= layers; i++) {
        polygon p(height/(layers+1)*i);
        p.deform(5);
        polygons.push_back(p);
        ofColor c;
        int aa = hues[(int)ofRandom(3)];
        aa = (aa + (int)ofRandom(20))%255;
        c.setHsb(aa, ofRandom(90, 250), (ofRandom(1.0) > ofMap(i, 1, layers, 0.9, 0.08) ? ofRandom(20) : ofRandom(220, 250)), 6);
        ofFloatColor c2 = c;
        c2.a = 0.02;
        colors.push_back(c2);
        framecount.push_back(0);
    }
    
    buffer.begin();
    ofBackground(255);
    buffer.end();
    
    ofSetPolyMode(OF_POLY_WINDING_ODD);
}

//--------------------------------------------------------------
void ofApp::update(){
    if(ofGetElapsedTimef()>=20) {
        ofPixels pix;
        buffer.readToPixels(pix);
        ofSaveImage(pix,"../images/"+seedstring+".jpg");
        cout << seedstring;
        ofExit();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int i = 0;
    for(auto p : polygons) {
        if(framecount[i] < frames_per_layer) {
            framecount[i]++;
            
            p.deform(4);
            p.vertices.insert(p.vertices.begin(), {0, p.vertices[0].second});
            
            buffer.begin();
            ofSetColor(colors[i]);
            ofBeginShape();
            for(auto v : p.vertices)
                ofVertex(v.first, v.second);
            ofEndShape();
            
            //draw borders
            ofSetColor(255);
            ofDrawRectangle(0, 0, width, 100);
            ofDrawRectangle(0, 0, 100, height);
            ofDrawRectangle(width-100, 0, 100, height);
            ofDrawRectangle(0, height-100, width, 100);
            
            //draw seed
            ofSetColor(0);
            drawStringCentered(seedstring, width/2, height-50);
            buffer.end();
        }
        i++;
    }
    ofSetColor(255, 255, 255, 255);
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
