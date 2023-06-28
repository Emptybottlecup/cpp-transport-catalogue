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
        For_bus(catalog,text.substr(pos_2 + 1),std::cout);
    }
    else {
        For_stop(catalog,text.substr(pos_2 + 1),std::cout);
    }
    ++i;
    }
}
        
std::ostream& For_bus(transport_catalog::TransportCatalogue& catalog, const std::string& bus, std::ostream& out) {
      if(catalog.FindBus(bus).name != "not found") {
        auto bus_ = catalog.FindBus(bus);  
        out <<"Bus "<< bus_.name <<": "<< bus_.stops.size() <<" stops on route, "<< bus_.unique.size()<<" unique stops, "<<bus_.dist_cur<<" route length, "<< bus_.cur <<" curvature"<< std::endl;
    }
    else {
        out <<"Bus "<< bus <<": not found" << std::endl;
    }
    return out;
}
        
std::ostream& For_stop(transport_catalog::TransportCatalogue& catalog, const std::string& stop, std::ostream& out) {
         if((*(catalog.FindStop(stop))).name != "not found") {
        auto stop_ = catalog.FindStop(stop);  
        out <<"Stop "<< stop_->name <<": ";
        if((catalog.stop_bus).at(stop_).size() != 0) {
            out << "buses ";
            for(auto i : (catalog.stop_bus).at(stop_)) {
                out << i << " ";
            }
            out << std::endl;
        }
        else {
            out << "no buses" << std::endl;
        }
    }
    else {
        out <<"Stop "<< stop <<": not found" << std::endl;
    }
    return out;
}        
    }
}