#pragma once

#include "ofApp.h"

vector<double> aff;

#define R sqrt(v.x*v.x+v.y*v.y+v.z*v.z)
#define R1 sqrt(v.x*v.x+v.y*v.y)
#define R2 sqrt(v.x*v.x+v.z*v.z)
#define O tan(v.y/(v.x == 0 ? 1 : v.x))
#define O2 tan(v.z/(v.x == 0 ? 1 : v.x))

ofVec3f sinusoidal(ofVec3f v) {
    return ofVec3f(sin(v.x), sin(v.y), sin(v.z));
}

ofVec3f spherical(ofVec3f v) {
    return v*(1/(R*R));
}

ofVec3f spiral(ofVec3f v) {
    return (1/(R == 0 ? 1 : R))*ofVec3f(cos(O)+sin(R), sin(O)-cos(R), cos(O2)+sin(R));
}

ofVec3f swirl(ofVec3f v) {
    return ofVec3f(v.x*sin(R*R)-v.y*cos(R*R), v.x*cos(R*R)+v.y*sin(R*R), v.x*cos(R*R)-v.z*sin(R*R));
}

ofVec3f disc(ofVec3f v) {
    return ofVec3f(O/PI*sin(PI*R), O/PI*cos(PI*R), O2/PI*cos(PI*R));
}

ofVec3f hyperbolic(ofVec3f v) {
    return ofVec3f(sin(O)/(R == 0 ? 1 : R), R*cos(O), sin(O2)/R);
}

ofVec3f julia(ofVec3f v) {
    return sqrt(R)*ofVec3f(cos(O/2), sin(O/2), sin(O2/2));
}

ofVec3f wave(ofVec3f v) {
    return ofVec3f(v.x+aff[1]*sin(v.y/(aff[2]*aff[2])), v.y+aff[3]*sin(v.x/(aff[4]*aff[4])), v.z+aff[5]*sin(v.z/(aff[6]*aff[6])));
}

ofVec3f popcorn(ofVec3f v) {
    return ofVec3f(v.x+aff[1]*sin(tan(3*v.y)), v.y+aff[2]*sin(tan(3*v.z)), v.z+aff[3]*sin(tan(3*v.x)));
}

ofVec3f horseshoe(ofVec3f v) {
    return (1/(R == 0 ? 1 : R))*ofVec3f((v.x-v.y)*(v.x+v.y), 2*v.x*v.y, (v.x-v.z)*(v.x+v.z));
}

ofVec3f polar(ofVec3f v) {
    return ofVec3f(O/PI, R-1, O2/PI);
}

ofVec3f handkerchief(ofVec3f v) {
    return R*ofVec3f(sin(O+R), cos(O-R), sin(O2+R));
}

ofVec3f heart(ofVec3f v) {
    return R*ofVec3f(sin(O*R1), -cos(O*R1), -cos(O2*R2));
}

ofVec3f diamond(ofVec3f v) {
    return ofVec3f(sin(O)*cos(R), cos(O)*sin(R), sin(O2)*cos(R));
}

ofVec3f ex(ofVec3f v) {
    double p0 = sin(O+R);
    double p1 = cos(O-R);
    double p2 = sin(O2+R);
    double p3 = cos(O2-R);
    return R*ofVec3f(pow(p0, 3)+pow(p1, 3), pow(p0, 3)-pow(p1, 3), pow(p2, 3)-pow(p3, 3));
}

double cosh(double x) { return 0.5 * (exp(x) + exp(-x));}
double sinh(double x) { return 0.5 * (exp(x) - exp(-x));}

ofVec3f cosine(ofVec3f v) {
    return ofVec3f(cos(PI*v.x)*cosh(v.y), -sin(PI*v.x)*sinh(v.y), sin(v.x));
}

ofVec3f power(ofVec3f v) {
    return pow(R, sin(O))*ofVec3f(cos(O), sin(O), cos(O2));
}

ofVec3f bent(ofVec3f v) {
    if(v.x >= 0 && v.y >= 0) return v;
    else if(v.x < 0 && v.y >= 0) return ofVec3f(2*v.x, v.y, 2*v.z);
    else if(v.x >= 0 && v.y < 0) return ofVec3f(v.x, v.y/2, v.z/2);
    else return ofVec3f(v.x*2, v.y/2, v.z);
}

ofVec3f exponential(ofVec3f v) {
    return ofVec3f(exp(v.x-1)*cos(PI*v.y), exp(v.x-1)*sin(PI*v.y), exp(v.y-1)*sin(PI*v.z));
}

#undef R
#undef R1
#undef R2
#undef O
#undef O2

vector<string> chosen_vars;

string randVariation() {
    vector<string> v = {
        "sinusoidal", "spiral", "swirl",  "hyperbolic",
        "wave", "popcorn", "horseshoe", "handkerchief",
        "diamond",  "cosine" , "polar", "heart",
        "disc", "julia", "ex", "spherical",
        "power", "bent", "exponential"
    };
    return v[(int)ofRandom(v.size()-0.01)];
}

ofVec3f resolveVariation(string name, ofVec3f v) {
    if(name == "sinusoidal") return sinusoidal(v);
    else if(name == "spiral") return spiral(v);
    else if(name == "swirl") return swirl(v);
    else if(name == "disc") return disc(v);
    else if(name == "hyperbolic") return hyperbolic(v);
    else if(name == "julia") return julia(v);
    else if(name == "wave") return wave(v);
    else if(name == "popcorn") return popcorn(v);
    else if(name == "horseshoe") return horseshoe(v);
    else if(name == "polar") return polar(v);
    else if(name == "handkerchief") return handkerchief(v);
    else if(name == "heart") return heart(v);
    else if(name == "diamond") return diamond(v);
    else if(name == "ex") return ex(v);
    else if(name == "cosine") return cosine(v);
    else if(name == "spherical") return spherical(v);
    else if(name == "power") return power(v);
    else if(name == "bent") return bent(v);
    else if(name == "exponential") return exponential(v);
}

double noise_seed, noise_scale = 0.25, speed = 0.03;

vector<pair<string, int> > formula;

//this is basically polish notation
//https://en.wikipedia.org/wiki/Polish_notation

ofVec3f resolveFormula(ofVec3f v) {
    ofVec3f w = resolveVariation(formula[0].first, v);
    for(int i = 1; i < formula.size(); i++) {
        if(formula[i].second == 0) w = resolveVariation(formula[i].first, w);
        else if(formula[i].second == 1) w += resolveVariation(formula[i].first, v);
        else if(formula[i].second == 2) w -= resolveVariation(formula[i].first, v);
        else if(formula[i].second == 3) w *= resolveVariation(formula[i].first, v);
        else w /= resolveVariation(formula[i].first, v);
    }
    return w;
}

int octaves = 2;

double getNoise(double x, double y, double z, double seed) {
    double n = 0;
    for(double i = 1, j = 1, k = 1; i <= octaves; i++, j /= 2, k *= 2)
        n += j * ofNoise(x * k, y * k, z * k, seed);
    return min(1.0, n);
}

ofVec3f sphereNoise(ofVec3f v) {
    v *= noise_scale;
    double phi = getNoise(v.x, v.y, v.z, noise_seed)*2*PI;
    double costheta = getNoise(v.x, v.y, v.z, noise_seed+100)*2-1;
    double u = getNoise(v.x, v.y, v.z, noise_seed+1000);
    
    double theta = acos(costheta);
    double rad = cbrt(u);
    
    return ofVec3f(rad*sin(theta)*cos(phi), rad*sin(theta)*sin(phi), rad*cos(theta));
}

vector<double> hues;

struct particle {
    ofVec3f pos;
    double hue, sat;
    particle() {}
    particle(ofVec3f p) : pos(p) {
        p /= 5;
        hue = hues[(int)(getNoise(p.x, p.y, p.z, noise_seed)*3)];
        sat = getNoise(p.x, p.y, p.z, noise_seed);
    }
    void update() {
        auto v = sphereNoise(pos);
        v = resolveFormula(v);
        pos += v*speed;
    }
};
