#include "input_reader.h"
namespace transport_catalog {
    namespace reader {
void input_reader(TransportCatalogue& catalog, std::istream& input) {
    std::vector<std::string> stops;
    std::vector<std::string> routs;
    std::map<std::string,std::vector<std::pair<std::string,double>>> stop_dist;
    std::string text;
    getline(input,text);
    int g = atof(text.c_str());
    int k = 0;
    while(k != g)  {
        getline(input,text);
        if(Check(text) == "stop") {
            stops.push_back(Split(text));
            auto pos_1 = text.find_first_not_of(' ');
            auto pos_2 = text.find_first_of(' ', pos_1);
            auto pos_3 = text.find(":", pos_1);
            std::string name = text.substr(pos_2 + 1, pos_3 - pos_2 - 1);
            if(text.find(" to") != text.npos) {
            stop_dist.insert(make_pair(name,Split_dist(text)));
            }
        }
        else {
            routs.push_back(Split(text));
        }
        ++k;
    }
    
    for(auto stop : stops) {
        catalog.AddStop(stop);
    }
    catalog.AddDist(stop_dist);
    for(auto route : routs) {
        catalog.AddBus(route);    
    }
}

std::string Split(const std::string& text) {
    std::string all_text;
    if (Check(text) == "stop") {
        auto pos_1 = text.find_first_not_of(' ');
        auto pos_2 = text.find_first_of(' ', pos_1);
        auto pos_3 = text.find(":", pos_1);
        all_text += text.substr(pos_2 + 1, pos_3 - pos_2);
        pos_1 = text.find_first_not_of(' ', pos_3 + 1);
        pos_3 = text.find(',', pos_1);
        all_text += text.substr(pos_1, pos_3 - pos_1) + " ";
        pos_1 = text.find_first_not_of(' ', pos_3 + 1);
        all_text += text.substr(pos_1);
    }
    else {
        auto pos_1 = text.find_first_not_of(' ');
        auto pos_2 = text.find_first_of(' ', pos_1);
        auto pos_3 = text.find(":", pos_1);
        all_text += text.substr(pos_2 + 1, pos_3 - pos_2 - 1) + ' ';
        std::string tir_min;
        if (text.find('-') != text.npos) {
            tir_min = "-";
        }
        else {
            tir_min = ">";
        }
        all_text += tir_min + " ";
        bool flag = true;
        while (flag) {
            std::string literals = ">-";
            pos_1 = text.find_first_not_of(' ', pos_3 + 1);
            pos_3 = text.find_first_of(literals, pos_1);
            if (pos_3 != text.npos) {
                all_text += text.substr(pos_1, pos_3 - pos_1 - 1) + ",";
            }
            else {
                all_text += text.substr(pos_1);
                flag = false;
            }
        }
    }
    return all_text;
}


std::string Check (const std::string& text) {
    auto pos_1 = text.find_first_not_of(' ');
    auto pos_2 = text.find_first_of(' ', pos_1);
    if (text.substr(pos_1, pos_2 - pos_1) == "Stop") {
        return "stop";
    }
    else {
        return "bus";
    }
}

std::vector<std::pair<std::string,double>> Split_dist(std::string text) {
    std::vector<std::pair<std::string,double>> vec;
    std::string name;
    auto pos_1 = text.find_first_not_of(' ');
    auto pos_2 = text.find_first_of(' ', pos_1);
    auto pos_3 = text.find(":", pos_1);
    name = text.substr(pos_2 + 1, pos_3 - pos_2);
    pos_1 = text.find_first_not_of(' ', pos_3 + 1);
    pos_3 = text.find(',', pos_1);
    pos_1 = text.find_first_not_of(' ', pos_3 + 1);
    pos_3 = text.find(",",pos_1);
    bool flag = true;
    while(flag) {
        pos_1 = text.find_first_not_of(' ', pos_3 + 1);
        pos_3 = text.find_first_of('m',pos_1);
        double dist = atof((text.substr(pos_1,pos_3 - pos_1)).c_str());
        pos_1 = text.find_first_of(' ',pos_3 + 2);
        pos_3 = text.find_first_of(',',pos_3);
        std::string name = text.substr(pos_1 + 1,pos_3 - pos_1 - 1);
        vec.push_back(make_pair(name,dist));
        if(pos_3 == text.npos) {
            flag = false;
        }
    }
    return vec;
}
    }
}