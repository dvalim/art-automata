#pragma once

//refer to the original fractal flame pdf and the 3d version
//https://flam3.com/flame_draves.pdf
//https://tigerprints.clemson.edu/cgi/viewcontent.cgi?article=2704&context=all_theses

#include "ofApp.h"

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

ofVec3f wave(ofVec3f v, vector<double> aff) {
    return ofVec3f(v.x+aff[1]*sin(v.y/(aff[2]*aff[2])), v.y+aff[3]*sin(v.x/(aff[4]*aff[4])), v.z+aff[5]*sin(v.z/(aff[6]*aff[6])));
}

ofVec3f popcorn(ofVec3f v, vector<double> aff) {
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

ofVec3f resolveVariation(string name, ofVec3f v, vector<double> aff) {
    if(name == "sinusoidal") return sinusoidal(v);
    else if(name == "spiral") return spiral(v);
    else if(name == "swirl") return swirl(v);
    else if(name == "disc") return disc(v);
    else if(name == "hyperbolic") return hyperbolic(v);
    else if(name == "julia") return julia(v);
    else if(name == "wave") return wave(v, aff);
    else if(name == "popcorn") return popcorn(v, aff);
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

ofVec3f affine(ofVec3f v, vector<double> a) {
    return ofVec3f(v.x*a[0]+v.y*a[1]+v.z*a[2]+a[3], v.x*a[4]+v.y*a[5]+v.z*a[6]+a[7], v.x*a[8]+v.y*a[9]+v.z*a[10]+a[11]);
}

string chooseVariation(vector<string> vars) {
    if(!vars.size()) return randVariation();
    return vars[(int)ofRandom(vars.size()-0.01)];
}

struct func {
    int len;
    vector<double> aff, post, weight;
    vector<string> vars;
    
    ofVec3f resolve(ofVec3f v) {
        ofVec3f w(0, 0, 0);
        for(int i = 0; i < len; i++) {
            ofVec3f s = weight[i]*resolveVariation(vars[i], affine(v, aff), aff);
            w += s;
        }
        return affine(w, post);
    }
    
    func() {
        len = ofRandom(1, 8);
        for(int i = 1; i <= 12; i++)
            post.push_back(ofRandom(0, 1.2)*(ofRandom(1) <= 0.5 ? -1 : 1)),
            aff.push_back(ofRandom(0, 1.2)*(ofRandom(1) <= 0.5 ? -1 : 1));
        for(int i = 1; i <= len; i++)
            weight.push_back(0),
            vars.push_back(randVariation());
        double w_sum = 0, w_inc = 0.05;
        while(w_sum < 1) {
            weight[ofRandom(len-0.01)] += w_inc;
            w_sum += w_inc;
        }
    }
    
    func(vector<string> available_vars) {
        len = ofRandom(1, 8);
        for(int i = 1; i <= 12; i++)
            post.push_back(ofRandom(1.2)*(ofRandom(1) <= 0.5 ? -1 : 1)),
            aff.push_back(ofRandom(1.2)*(ofRandom(1) <= 0.5 ? -1 : 1));
        for(int i = 1; i <= len; i++)
            weight.push_back(0),
            vars.push_back(chooseVariation(available_vars));
        double w_sum = 0, w_inc = 0.05;
        while(w_sum < 1) {
            weight[ofRandom(len-0.01)] += w_inc;
            w_sum += w_inc;
        }
    }
};


struct fract {
    vector<double> f_weight;
    vector<func> funcs;
    ofVec3f v;
    func fin;
    
    int weightedRand() {
        double r = ofRandom(1), w = 0;
        for(int i = 0; i < f_weight.size(); i++) {
            w += f_weight[i];
            if(r <= w) return i;
        }
        return 0;
    }
    
    fract(vector<string> chosen_vars) {
        int func_number = ofRandom(2, 15);
        for(int i = 1; i <= func_number; i++)
            funcs.push_back(func(chosen_vars)),
            f_weight.push_back(0);
        double f_sum = 0, f_inc = 0.5;
        while(f_sum < 0.99) {
            f_weight[(int)ofRandom(func_number-0.01)] += f_inc;
            f_sum += f_inc;
            f_inc /= 2;
        }
        fin = func(chosen_vars);
    
        v = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
    }
    
    ofVec3f step() {
        if(isnan(v.x) || isnan(v.y) || isnan(v.z)) v = ofVec3f(ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1));
        
        int id = weightedRand();
        v = funcs[id].resolve(v);
        v = fin.resolve(v);
        return v;
    }
};
