#include "stat_reader.h"
namespace transport_catalog {
    namespace reader {
void stat_reader(transport_catalog::TransportCatalogue& catalog, std::istream& input) {
    std::string text;
    getline(input,text);
    int g = atof(text.c_str());
    int i = 0;
    while(i != g) {
    getline(input,text);
    auto pos_1 = text.find_first_not_of(" ");
    auto pos_2 = text.find_first_of(" ",pos_1);
    if(text.substr(pos_1,pos_2 - pos_1) == "Bus") {
    if((catalog.FindBus(text.substr(pos_2 + 1))).name != "not found") {
        auto bus = catalog.FindBus(text.substr(pos_2 + 1));  
        std::cout <<"Bus "<< bus.name <<": "<< bus.stops.size() <<" stops on route, "<< bus.unique.size()<<" unique stops, "<<bus.dist<<" route length, "<< bus.cur <<" curvature"<< std::endl;
    }
    else {
        std::cout <<"Bus "<<text.substr(pos_2 + 1)<<": not found" << std::endl;
    }
    }
    else {
          if((*(catalog.FindStop(text.substr(pos_2 + 1)))).name != "not found") {
        auto stop = catalog.FindStop(text.substr(pos_2 + 1));  
        std::cout <<"Stop "<< stop->name <<": ";
        if((catalog.stop_bus).at(stop).size() != 0) {
            std::cout << "buses ";
            for(auto i : (catalog.stop_bus).at(stop)) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
        else {
            std::cout << "no buses" << std::endl;
        }
    }
    else {
        std::cout <<"Stop "<<text.substr(pos_2 + 1)<<": not found" << std::endl;
    }
    }
    ++i;
    }
}
    }
}