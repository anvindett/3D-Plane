#ifndef MODEL_HPP
#define MODEL_HPP

#include <map>
#include <vector>

#define MapModel std::map<std::string, std::vector<std::vector<float>>>

class Model {
    private:
        MapModel model;
        const std::string BASEDIR = "./config/";
        const std::string files[8] = {
            "badan", "sayap1", "sayap2", "ekor1", "ekor2", "baling1", "baling2", "silinder"
        };

    public:
        Model();

        MapModel getMapModel();

        void loadFromFile();

        ~Model();

};

#endif
