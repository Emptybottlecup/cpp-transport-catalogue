#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <set>
typedef std::map<std::string,std::vector<std::pair<std::string,double>>> distance_all_stops;
namespace transport_catalog {
namespace road_objects {
struct Stop {
    std::string name;
    double latitude;
    double longitude;
};
    
struct Route {
    std::string name;
    double dist_fact = 0;
    double dist_cur = 0;
    double cur = 0;
    std::vector<Stop*> stops;
    std::vector<Stop*> unique;
};
}

class TransportCatalogue {
private:
    std::map<std::string_view,road_objects::Stop*> stops_map;
    std::deque<road_objects::Stop> stops;
    std::deque<road_objects::Route> routs;
    std::map<std::string_view,road_objects::Route*> routs_map;
    std::map<std::pair<road_objects::Stop*,road_objects::Stop*>,double> stop_dist;
public:
    void AddStop(road_objects::Stop&& stop_);
    void AddBus(road_objects::Route&& route);
    void AddDist(const std::map<std::string,std::vector<std::pair<std::string,double>>>& dist);
    road_objects::Route FindBus(const std::string& bus) const;
    road_objects::Stop* FindStop(const std::string& name) const;
    std::unordered_map<road_objects::Stop*,std::set<std::string>> stop_bus;
    std::map<std::string_view,road_objects::Stop*>& get_stops_map();
};
}