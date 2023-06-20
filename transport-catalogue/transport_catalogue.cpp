#include "transport_catalogue.h"
#include <set>
#include "geo.h"

namespace transport_catalog
{
void TransportCatalogue::AddBus(const std::string& route) {
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
 route_.unique.push_back(stops_map.at(route.substr(start_stop,end_stop-start_stop)));
 stop_bus.at(stops_map.at(route.substr(start_stop,end_stop-start_stop))).insert(route_.name);
 uni.insert(route.substr(start_stop,end_stop-start_stop));   
};                    
                   route_.stops.push_back(stops_map.at(route.substr(start_stop,end_stop-start_stop))); 
                    
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
 route_.unique.push_back(stops_map.at(route.substr(start_stop,end_stop-start_stop)));
 stop_bus.at(stops_map.at(route.substr(start_stop,end_stop-start_stop))).insert(route_.name);
 uni.insert(route.substr(start_stop,end_stop-start_stop));   
};                   route_.stops.push_back(stops_map.at(route.substr(start_stop,end_stop-start_stop)));        
                    start_stop = end_stop;
            }
                else {
                    work = false;
                }
            }
     }
    for(int i = 0; i < route_.stops.size() - 1; ++i) {
            route_.dist += stop_dist.at(std::make_pair(route_.stops[i],route_.stops[i+1]));
            route_.dist_cur += dist_count::ComputeDistance({(*(route_.stops[i])).latitude,(*(route_.stops[i])).longitude},{(*(route_.stops[i+1])).latitude,(*(route_.stops[i+1])).longitude});
        }
    route_.cur = route_.dist/route_.dist_cur;
    routs.push_back(route_);
    routs_map.insert(make_pair(std::string_view((routs.back()).name),&(routs.back())));
}

void TransportCatalogue::AddStop(const std::string& stop) {
        road_objects::Stop stop_;
        auto pos = stop.find(':');
        stop_.name = stop.substr(0,pos);
        auto pos_2 = stop.find_first_of(' ',pos);
        stop_.latitude = atof((stop.substr(pos + 1,pos_2 - pos + 1)).c_str());
        stop_.longitude = atof(stop.substr(pos_2).c_str());
        stops.push_back(stop_);
        stops_map.insert(make_pair(std::string_view((stops.back()).name),&(stops.back())));
    stop_bus[(&(stops.back()))];
}


road_objects::Route TransportCatalogue::FindBus(const std::string name) const {
    if(routs_map.count(name) != 0) {
        return *(routs_map.at(name));
    }
    road_objects::Route not_route;
    not_route.name = "not found";
    return not_route;
}

road_objects::Stop* TransportCatalogue::FindStop(const std::string name) const {
    if(stops_map.count(name) != 0) {
        return stops_map.at(name);
    }
    road_objects::Stop* not_stop = new road_objects::Stop;
    not_stop->name = "not found";
    return not_stop;    
}

void TransportCatalogue::AddDist(std::map<std::string,std::vector<std::pair<std::string,double>>>dist) {
      for(auto i : dist) {
          for(auto g : i.second) {
  if(stop_dist.count(std::make_pair(stops_map.at(i.first),stops_map.at(g.first))) != 0) {
   stop_dist.at(std::make_pair(stops_map.at(i.first),stops_map.at(g.first))) = g.second;   
  }
 else {             stop_dist.insert(std::make_pair(std::make_pair(stops_map.at(i.first),stops_map.at(g.first)),g.second));
 if(stop_dist.count(std::make_pair(stops_map.at(g.first),stops_map.at(i.first))) == 0) {
stop_dist.insert(std::make_pair(std::make_pair(stops_map.at(g.first),stops_map.at(i.first)),g.second));
     }
 }    
          }
      }
  }
}