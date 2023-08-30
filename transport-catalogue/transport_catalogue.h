#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <string_view>
#include <unordered_map>
#include <set>
#include "geo.h"

namespace transport_catalog {
namespace road_objects {
struct Stop {
    Stop() {}
    Stop(std::string name_, geo::Coordinates coord_) : name(name_), coord(coord_) {}
    std::string name;
    geo::Coordinates coord;
    std::set<std::string> buses;
};
    
struct Route {
    Route() {}
    Route(std::string name_, std::vector<Stop*> stops_, bool is_roudtrip_) : name(name_), stops(stops_), is_roundtrip(is_roudtrip_) {}
    std::string name;
    double dist_fact = 0;
    double cur = 0;
    std::vector<Stop*> stops;
    std::vector<Stop*> un_stop;
    std::set<Stop*> unique;
    bool is_roundtrip;
};
}

class TransportCatalogue {
private:
    std::map<std::string_view,road_objects::Stop*> stops_map;
    std::deque<road_objects::Stop> stops;
    std::deque<road_objects::Route> routs;
    std::map<std::string_view,road_objects::Route*> routs_map;
    std::map<std::pair<std::string,std::string>,int> dist;
    std::unordered_map<road_objects::Stop*,std::set<std::string>> stop_bus;
public:
    std::map<std::string_view,road_objects::Route*> GetBus();
    void SetDistance(std::string from, std::string to, int dist_);
    int GetDistance(std::string from, std::string to);
    void AddStop(const std::string& name, geo::Coordinates coordinates);
    void AddBus(const std::string& name, std::vector<road_objects::Stop*>& stops, bool is_roundtrip);
    road_objects::Route* FindBus(std::string bus) const;
    road_objects::Stop* FindStop(std::string name) const;
    std::map<std::string_view,road_objects::Stop*>& GetStopsMap();
};
}
