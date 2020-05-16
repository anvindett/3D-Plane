#include "model.hpp"
#include <iostream>
#include <fstream>

using namespace std;

// Constructor
Model::Model() {}

MapModel Model::getMapModel() {
    return this->model;
}

void Model::loadFromFile() {
    ifstream ifs;
    float x, y, z;
    vector<float> currVertex;
    int files_length = sizeof(this->files)/sizeof(*this->files);

    for (int i=0; i<files_length; i++) {
        ifs.open(BASEDIR+files[i]);
        while (ifs >> x >> y >> z) {
            currVertex.clear();
            currVertex.push_back(x);
            currVertex.push_back(y);
            currVertex.push_back(z);
            this->model[files[i]].push_back(currVertex);
        }
        ifs.close();
    }
    
}

Model::~Model() {}