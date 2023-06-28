#include "input_reader.h"
namespace transport_catalog {
    namespace reader {
void input_reader(TransportCatalogue& catalog, std::istream& input) {
    std::vector<std::string> stops;
    std::vector<std::string> routs;
    distance_all_stops stop_dist;
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
    
    for(const auto& stop : stops) {
        catalog.AddStop(Split_stop_str(stop));
    }
    catalog.AddDist(stop_dist);
    for(auto route : routs) {
        catalog.AddBus(Split_route_str(route,catalog));    
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
        
transport_catalog::road_objects::Stop Split_stop_str(const std::string& stop) {
        transport_catalog::road_objects::Stop stop_;
        auto pos = stop.find(':');
        stop_.name = stop.substr(0,pos);
        auto pos_2 = stop.find_first_of(' ',pos);
        stop_.latitude = atof((stop.substr(pos + 1,pos_2 - pos + 1)).c_str());
        stop_.longitude = atof(stop.substr(pos_2).c_str());
        return stop_;
}
        
transport_catalog::road_objects::Route Split_route_str(const std::string& route,TransportCatalogue& catalog) {
    road_objects::Route route_;
    auto start_stop = route.find('-');
    if(start_stop != route.npos) {
    route_.name = route.substr(0,start_stop - 1);
    if(route[start_stop] == '-') {
            bool work = true;
            std::string literals = " -,";
            std::set<std::string> uni;
            while (work) {
                start_stop = route.find_first_not_of(literals, start_stop);
                if (start_stop != route.npos) {
                    auto end_stop = route.find_first_of(',', start_stop);
                    
if(uni.count(route.substr(start_stop,end_stop-start_stop)) == 0) {
 route_.unique.push_back(catalog.get_stops_map().at(route.substr(start_stop,end_stop-start_stop)));
 catalog.stop_bus.at(catalog.get_stops_map().at(route.substr(start_stop,end_stop-start_stop))).insert(route_.name);
 uni.insert(route.substr(start_stop,end_stop-start_stop));   
};                    
                   route_.stops.push_back(catalog.get_stops_map().at(route.substr(start_stop,end_stop-start_stop))); 
                    
                    start_stop = end_stop;
            }
                else {
                    work = false;
                }
            }
        for(int i = route_.stops.size() - 2; i >= 0; --i) {
             route_.stops.push_back(route_.stops[i]);
        }
        }
    }  
    else {
            start_stop = route.find('>');
            route_.name = route.substr(0,start_stop - 1);
            bool work = true;
            std::string literals = " >,";
            std::set<std::string> uni;
            while (work) {
                start_stop = route.find_first_not_of(literals, start_stop);
                if (start_stop != route.npos) {
                    auto end_stop = route.find_first_of(',', start_stop);
                    if(uni.count(route.substr(start_stop,end_stop-start_stop)) == 0) {
 route_.unique.push_back(catalog.get_stops_map().at(route.substr(start_stop,end_stop-start_stop)));
 catalog.stop_bus.at(catalog.get_stops_map().at(route.substr(start_stop,end_stop-start_stop))).insert(route_.name);
 uni.insert(route.substr(start_stop,end_stop-start_stop));   
};                   route_.stops.push_back(catalog.get_stops_map().at(route.substr(start_stop,end_stop-start_stop)));        
                    start_stop = end_stop;
            }
                else {
                    work = false;
                }
            }
     }
    return route_;
}        
}               
    }
