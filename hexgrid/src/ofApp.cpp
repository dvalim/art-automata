#include "ofApp.h"
#include <random>

#define sq3 sqrt(3)/2

ofTrueTypeFont glyphs, font;
ofFbo buffer;
int seed;
string seedstring;

int width = 1000;
int height = 1000;

std::mt19937 engine;

double time_limit = 3;

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
    ofSetColor(0);
    font.drawString(s, x + offset.x, y + offset.y);
}

//c++ has trash unicode support

void GetUnicodeChar(unsigned int code, char chars[5]) {
    if (code <= 0x7F) {
        chars[0] = (code & 0x7F); chars[1] = '\0';
    } else if (code <= 0x7FF) {
        // one continuation byte
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xC0 | (code & 0x1F); chars[2] = '\0';
    } else if (code <= 0xFFFF) {
        // two continuation bytes
        chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xE0 | (code & 0xF); chars[3] = '\0';
    } else if (code <= 0x10FFFF) {
        // three continuation bytes
        chars[3] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[2] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[1] = 0x80 | (code & 0x3F); code = (code >> 6);
        chars[0] = 0xF0 | (code & 0x7); chars[4] = '\0';
    } else {
        // unicode replacement character
        chars[2] = 0xEF; chars[1] = 0xBF; chars[0] = 0xBD;
        chars[3] = '\0';
    }
}

double noise_seed, font_size = 30;

double safeguard = 0.03;

double line_chance;
double fill_chance;
double noise_mult;
double letter_chance;
double grid_size;

double getNoise(double x, double y, double z) {
    x = x / width;
    y = y / width;
    double n = ofNoise(x, y, z);
    n += 0.5 * ofNoise(x * 2, y * 2, z);
    n += 0.25 * ofNoise(x * 4, y * 4, z);
    return min(1.0, n) + gaussian(0, 0.2);
}

int rndChar() { //random character from a few interesting blocks
    vector<vector<int> > v = {
        {0x2300, 0x23E8},
        {0x25FF, 0x25A0},
        {0x259F, 0x2500},
        {0x2190, 0x21FF}
    };
    auto p = v[(int)ofRandom(v.size())];
    return ofRandom(p[1]-p[0])+p[0];
}

void loadFonts() {
    ofTrueTypeFontSettings settings("segoe.ttf", font_size);
    settings.antialiased = true;
    settings.addRange(ofUnicode::Arrows);
    settings.addRange(ofUnicode::MiscTechnical);
    settings.addRange(ofUnicode::BoxDrawing);
    settings.addRange(ofUnicode::BlockElement);
    settings.addRange(ofUnicode::GeometricShapes);
    glyphs.load(settings);
    font.load("sans.ttf", 30);
}

//--------------------------------------------------------------
void ofApp::setup(){
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    ofSeedRandom(seed);
    engine.seed(seed);
    stringstream sstream;
    sstream << std::hex << seed;
    seedstring = sstream.str();
    noise_seed = ofRandom(1000);
    
    ofSetWindowShape(width, height);
    ofSetBackgroundAuto(false);
    buffer.allocate(width, height);
    
    buffer.begin();
    ofBackground(255);
    for(int x = 0; x < width; x++) //grain
        for(int y = 0; y < height; y++) {
            int shade = gaussian(240, 30);
            ofSetColor(shade, shade, shade, 60);
            ofDrawRectangle(x, y, 1, 1);
        }
    
    ofEnableSmoothing();
    
    //set up color palette
    vector<int> hues = {(int)ofRandom(255), (int)ofRandom(255)};
    hues.push_back((hues[0]+127+(int)ofRandom(10))%255);
    vector<ofColor> colors;
    for(int i = 0; i < 3; i++) {
        int aa = hues[i];
        int s = (ofRandom(1) <= 0.5 ? ofRandom(50, 80) : ofRandom(150, 190));
        int b = (ofRandom(1) <= 0.5 ? ofRandom(60, 100) : ofRandom(170, 230));
        ofColor c;
        c.setHsb(aa, s, b);
        colors.push_back(c);
    }
    
    vector<string> grids = {"triangle", "square", "hex"};
    string grid_type = grids[(int)ofRandom(3)];
    
    safeguard = 0.03;
    
    line_chance = ofRandom(safeguard, 1-safeguard);
    fill_chance = ofRandom(safeguard, 1-safeguard);
    noise_mult = ofRandom(safeguard, 4);
    letter_chance = gaussian(0, 0.5);
    grid_size = ofRandom(35, 130);
    
    font_size = grid_size * 0.8;
    loadFonts();
    int N = ofRandom(20)+1;
    
    vector<ofVec2f> points;
    
    ofPushMatrix();
    if(ofRandom(1.0) <= 0.5) { //rotate by 45 degrees
        ofTranslate(width/2, -height/2);
        ofScale(1.3, 1.3);
        ofRotateDeg(45);
    }
    
    //the math for calculating grid points could be written in a better way
    
    for(double x = 0; x <= width; x += (grid_type == "hex" ? grid_size*sq3*2 : grid_size))
        for(double y = 0; y <= height; y += (grid_type == "triangle" ? grid_size*sq3 : grid_type == "square" ? grid_size : grid_size*1.5)) {
            int n = getNoise(x, y, noise_seed)*N; //how many points
            for(int i = 1; i <= n; i++) {
                vector<ofVec2f> v;
                for(int j = 1; j <= 4; j++) { //need 4 points for bezier
                    if(grid_type == "triangle") {
                        double xx = ((int)y/(int)(grid_size*sq3) % 2 ? 0 : grid_size/2), yy = 0; //anchor point
                        vector<ofVec2f> w = {ofVec2f(x, y), ofVec2f(x+grid_size, y), ofVec2f( x+grid_size/2, y-grid_size*sq3)}; //other possible points
                        auto u = w[(int)ofRandom(3)]; //choose one randomly
                        v.push_back(ofVec2f(xx+u.x, yy+u.y));
                    } else if(grid_type == "hex") {
                        double xx = ((int)y/(int)(grid_size*1.5) % 2 ? 0 : grid_size*sq3), yy = 0;
                        vector<ofVec2f> w = {
                            ofVec2f(x, y), ofVec2f(x+grid_size*sq3, y-grid_size/2), ofVec2f( x+grid_size*sq3*2, y),
                            ofVec2f(x, y+grid_size), ofVec2f(x+grid_size*sq3, y+grid_size*1.5), ofVec2f(x+grid_size*sq3*2, y+grid_size)
                        };
                        auto u = w[(int)ofRandom(6)];
                        v.push_back(ofVec2f(xx+u.x, yy+u.y));
                    } else {
                        double xx = 0, yy = 0;
                        vector<ofVec2f> w = {ofVec2f(x, y), ofVec2f(x+grid_size, y), ofVec2f(x+grid_size, y+grid_size), ofVec2f(x, y+grid_size)};
                        auto u = w[(int)ofRandom(4)];
                        v.push_back(ofVec2f(xx+u.x, yy+u.y));
                    }
                }
                
                for(auto &i : v) points.push_back(i); //save points for later
                
                ofSetColor(colors[(int)(getNoise(x, y, noise_seed+2000)*3)]);
                
                if(i == 1 && getNoise(x, y, noise_seed+2000) <= letter_chance) {
                    char c[5];
                    GetUnicodeChar(rndChar(), c);
                    auto rect = glyphs.getStringBoundingBox(c, 0, 0);
                    glyphs.drawString(c, x+(grid_size-rect.width)/2, y-(grid_size-rect.height)/2);
                }
                
                if(getNoise(x, y, noise_seed) <= fill_chance) ofFill();
                else ofNoFill();
                
                if(getNoise(x, y, noise_seed+1000) <= line_chance) {
                    ofDrawLine(v[0].x, v[0].y, v[1].x, v[1].y);
                    ofDrawLine(v[2].x, v[2].y, v[3].x, v[3].y);
                } else {
                    ofDrawBezier(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, v[3].x, v[3].y);
                    ofNoFill();
                    ofDrawBezier(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, v[3].x, v[3].y);
                }
            }
            
            if(grid_type == "triangle") { //triangular grid needs a second pass
                int n = getNoise(x, y, noise_seed)*N;
                for(int i = 1; i <= n; i++) {
                    vector<ofVec2f> v;
                    for(int j = 1; j <= 4; j++) {
                        double xx = ((int)y/(int)(grid_size*sq3) % 2 ? 0 : grid_size/2), yy = 0;
                        vector<ofVec2f> w = {ofVec2f(x+grid_size, y), ofVec2f(x+grid_size/2, y-grid_size*sq3), ofVec2f( x+grid_size*1.5, y-grid_size*sq3)};
                        auto u = w[(int)ofRandom(3)];
                        v.push_back(ofVec2f(xx+u.x, yy+u.y));
                    }
                    for(auto &i : v) points.push_back(i);
                    if(getNoise(x, y, noise_seed) <= fill_chance) ofFill();
                    else ofNoFill();
                    ofSetColor(colors[(int)(getNoise(x, y, noise_seed+2000)*3)]);
                    
                    if(getNoise(x, y, noise_seed+1000) <= line_chance) {
                        ofDrawLine(v[0].x, v[0].y, v[1].x, v[1].y);
                        ofDrawLine(v[2].x, v[2].y, v[3].x, v[3].y);
                    } else {
                        ofDrawBezier(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, v[3].x, v[3].y);
                        ofNoFill();
                        ofDrawBezier(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, v[3].x, v[3].y);
                    }
                }
            }
        }
    
    //draw big beziers/lines
    int n = ofRandom(N);
    for(int i = 1; i <= n; i++) {
        vector<ofVec2f> v;
        for(int i = 1; i <= 4; i++)
            v.push_back(points[(int)ofRandom(points.size())]);
        ofNoFill();
        ofSetColor(colors[(int)ofRandom(3)]);
        if(ofRandom(1) <= line_chance) {
            ofDrawLine(v[0].x, v[0].y, v[1].x, v[1].y);
            ofDrawLine(v[2].x, v[2].y, v[3].x, v[3].y);
        } else ofDrawBezier(v[0].x, v[0].y, v[1].x, v[1].y, v[2].x, v[2].y, v[3].x, v[3].y);
    }
    
    ofPopMatrix();
    
    //draw borders
    ofSetColor(255);
    ofFill();
    ofDrawRectangle(0, 0, width, 100);
    ofDrawRectangle(0, 0, 100, height);
    ofDrawRectangle(width-100, 0, 100, height);
    ofDrawRectangle(0, height-100, width, 100);
    drawStringCentered(seedstring, width/2, height-50);
    buffer.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    if(ofGetElapsedTimef()>=time_limit) {
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
