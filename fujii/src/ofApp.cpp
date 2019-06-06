//refer to masaru fujii's formula for a strange attractor, this is a 3d version of it
//https://how-to-build-du-e.tumblr.com/

#include "ofApp.h"
#include <random>

double a[20], f[20], x, y, z, t, v;
int p[3];

double time_limit = 20;
int width = 2000, height = 2000;
ofTrueTypeFont font;
ofFbo buffer;
int seed;
std::string seedstring;

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

double sec(double x) { //secant
    return 1.0/cos(x);
}

double square(double x) { //square wave
    for(int i = 1; i <= 10; i++)
        x = sin(x)*2;
    return x/2;
}

double ssin(double x, int p) { //some cool periodic functions
    if(p == -5) return square(sin(x)*2);
    else if(p == -4) return (sec(sin(x))-1.5)*2;
    else if(p == -3) return cos(sin(x)*M_PI);
    else if(p == -2) return sin(sin(x)*2);
    else if(p == -1) return abs(fmod(x, 4)-2)-1;
    else if(p == 0) return asin(sin(x))/(M_PI/2);
    else return pow(sin(x), p);
}

double ccos(double x, int p) {
    if(p == -5) return square(cos(x)*2);
    else if(p == -4) return (sec(cos(x))-1.5)*2;
    else if(p == -3) return cos(cos(x)*M_PI);
    else if(p == -2) return sin(cos(x)*2);
    else if(p == -1) return abs(fmod(x+2, 4)-2)-1;
    else if(p == 0) return asin(cos(x))/(M_PI/2);
    else return pow(cos(x), p);
}

double minx = 1000, miny = 1000, maxx = -1000, maxy = -1000, mind = 1000, maxd = -1000;
double avgx, avgy, avgz;
bool mult = 0, dist = 1;

double noise_seed;

ofVec3f cam;
vector<double> hues;

//--------------------------------------------------------------
void ofApp::setup(){
    font.load("sans.ttf", 30);
    seed = std::chrono::system_clock::now().time_since_epoch().count();
    ofSeedRandom(seed);
    engine.seed(seed);
    std::stringstream sstream;
    sstream << std::hex << seed;
    seedstring = sstream.str();
    noise_seed = ofRandom(1000);
    
    ofSetWindowShape(width, height);
    ofSetBackgroundAuto(false);
    buffer.allocate(width, height);
    ofEnableSmoothing();
    
    if(ofRandom(1) <= 0.4) mult = 1;//doing wrong 3d projection sometimes looks cool
    
    for(int i = 1; i <= 12; i++) //important to keep these close to 1.0
        a[i] = ofRandom(0.7, 1.2) * (ofRandom(0, 100) > 50 ? -1: 1),
        f[i] = ofRandom(0.7, 1.2) * (ofRandom(0, 100) > 50 ? -1: 1);
    v = ofRandom(0.0001, 0.01); //important to keep this low
    p[1] = ofRandom(1, 5);
    p[2] = ofRandom(1, 5);

    if(ofRandom(1) <= 0.85) {
        p[1] = ofRandom(-5.99, 0);
        if(ofRandom(1) <= 0.7) p[2] = ofRandom(-4.99, 0);
        v = ofRandom(0.00005, 0.0007);
    }
    
    buffer.begin();
    ofBackground(0);
    ofSetColor(255);
    drawStringCentered(seedstring, width/2, height-50);
    buffer.end();

    for(int i = 1; i <= 10000; i++) { //calculate some initial points to setup bounds and averages
        double xx = a[1]*ssin(f[1]*x, p[1]) + a[2]*ccos(f[2]*y, p[2]) + a[4]*ssin(f[4]*z, p[1]) + a[5]*ccos(f[5]*t, p[2]);
        double yy = a[6]*ccos(f[6]*x, p[2]) + a[7]*ssin(f[7]*y, p[1]) + a[8]*ccos(f[8]*z, p[2]) + a[8]*ssin(f[8]*t, p[1]);
        double zz = a[9]*ssin(f[9]*x, p[1]) + a[10]*ssin(f[10]*y, p[2]) + a[11]*ccos(f[11]*z, p[1]) + a[12]*ccos(f[12]*t, p[2]);
        t += v;
        x = xx;
        y = yy;
        z = zz;
        double d = sqrt(x*x+y*y+z*z);
        
        avgx += x;
        avgy += y;
        avgz += z;

        minx = min(minx, x);
        miny = min(miny, y);
        maxx = max(maxx, x);
        maxy = max(maxy, y);
    }
    
    avgx /= 10000;
    avgy /= 10000;
    avgz /= 10000;
    
    x = y = z = t = 0;
    
    cam = ofVec3f(avgx, avgy, avgz); //putting camera in the center yields better results
    
    for(int i = 1; i <= 10000; i++) { //calculate points again to setup distance bounds
        double xx = a[1]*ssin(f[1]*x, p[1]) + a[2]*ccos(f[2]*y, p[2]) + a[4]*ssin(f[4]*z, p[1]) + a[5]*ccos(f[5]*t, p[2]);
        double yy = a[6]*ccos(f[6]*x, p[2]) + a[7]*ssin(f[7]*y, p[1]) + a[8]*ccos(f[8]*z, p[2]) + a[8]*ssin(f[8]*t, p[1]);
        double zz = a[9]*ssin(f[9]*x, p[1]) + a[10]*ssin(f[10]*y, p[2]) + a[11]*ccos(f[11]*z, p[1]) + a[12]*ccos(f[12]*t, p[2]);
        t += v;
        x = xx;
        y = yy;
        z = zz;
        double d = sqrt(pow(x-avgx, 2)+pow(y-avgy, 2)+pow(z-avgz, 2));
        
        mind = min(mind, d);
        maxd = max(maxd, d);
    }
    
    x = y = z = t = 0;
    
    //setup color palette
    double hue = ofRandom(1);
    hues.push_back(hue);
    hues.push_back(fmod(hue+ofRandom(0.4, 0.6), 1));
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

int iterations = 5000;

//--------------------------------------------------------------
void ofApp::draw(){
    buffer.begin();
    for(int i = 1; i <= iterations; i++) {
        double xx = a[1]*ssin(f[1]*x, p[1]) + a[2]*ccos(f[2]*y, p[2]) + a[4]*ssin(f[4]*z, p[1]) + a[5]*ccos(f[5]*t, p[2]);
        double yy = a[6]*ccos(f[6]*x, p[2]) + a[7]*ssin(f[7]*y, p[1]) + a[8]*ccos(f[8]*z, p[2]) + a[8]*ssin(f[8]*t, p[1]);
        double zz = a[9]*ssin(f[9]*x, p[1]) + a[10]*ssin(f[10]*y, p[2]) + a[11]*ccos(f[11]*z, p[1]) + a[12]*ccos(f[12]*t, p[2]);
        t += v;
        
        double d = sqrt(pow(xx-cam.x, 2)+pow(yy-cam.y, 2)+pow(zz-cam.z, 2)); //distance of point from camera
        
        //3d projection
        double xxx = ofMap((xx-cam.x)/(zz-cam.z)*(mult ? d : 1/d)+cam.x, minx, maxx, 100, width-100);
        double yyy = ofMap((yy-cam.y)/(zz-cam.z)*(mult ? d : 1/d)+cam.y, miny, maxy, 100, height-100);
        
        if(xxx > 100+gaussian(0, 2) && xxx < width-100+gaussian(0, 2) && yyy > 100+gaussian(0, 2) && yyy < height-100+gaussian(0, 2)) { //borders
            double dd = ofMap(d, mind, maxd, 1, 0.01);
            ofEnableBlendMode(OF_BLENDMODE_ADD);
            //chromatic aberration
            double scl = 3;//
            double step = ofMap(sqrt(pow(x-xx, 2)+pow(y-yy, 2)+pow(z-zz, 2)), 0, maxd*1.5, 0, 1);
            
            double hue = ofLerp(hues[0], hues[1], step);
            ofFloatColor c;
            c.setHsb(hue, min(step+0.3, 0.8), 1);
            c.a = 0.08*dd;
            
            ofSetColor(c);
            ofDrawRectangle(xxx, yyy, 1, 1);
            /*ofSetColor(0, 255, 255, 15*dd);
            double offset = ofMap(d, mind, maxd, 2, 0);
            ofDrawRectangle(xxx+offset, yyy+offset, 1, 1);*/
        }
        
        x = xx;
        y = yy;
        z = zz;
        
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
