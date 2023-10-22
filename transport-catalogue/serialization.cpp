#include "serialization.h"
#include <fstream>

void Serialize(transport_catalog::TransportCatalogue& catalog, MapRender& render, TransportRouter& router, std::ostream& out) {
	proto_transport_catalog::TransportCatalogue proto_catalog;
	SerializeStop(catalog, proto_catalog);
	SerializeDist(catalog, proto_catalog);
	SerializeBus(catalog, proto_catalog);
    SerializeRender(render, proto_catalog);
    SerializeRouter(router, proto_catalog,catalog);
	proto_catalog.SerializeToOstream(&out);
}

std::tuple<transport_catalog::TransportCatalogue, MapRender,TransportRouter, graph::DirectedWeightedGraph<double>, std::map<std::string, uint32_t>> Deserialize(std::istream& input) {
	proto_transport_catalog::TransportCatalogue proto_catalog;
	proto_catalog.ParseFromIstream(&input);
	transport_catalog::TransportCatalogue catalog;
	DeserializeStop(catalog, proto_catalog);
	DeserializeDist(catalog, proto_catalog);
	DeserializeBus(catalog, proto_catalog);
    MapRender render = DeserializeRender(proto_catalog);
    Parametrs param = DeserializeParametrs(proto_catalog);
    TransportRouter router(param);
	return {std::move(catalog), std::move(render), std::move(router), DeserializeGraph(proto_catalog), DeserializeStopId(proto_catalog)};
}

void SerializeStop(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog) {
	auto stops = catalog.GetStopsMap();
	for (auto& stop : stops) {
		proto_transport_catalog::Stop proto_stop;
		proto_stop.set_name(stop.second->name);
		proto_stop.mutable_coordinates()->set_lat(stop.second->coord.lat);
		proto_stop.mutable_coordinates()->set_lng(stop.second->coord.lng);
		for (auto& bus : stop.second->buses) {
			proto_stop.add_buses(bus);
		}
		*proto_catalog.add_stops() = std::move(proto_stop);
	}
}
    
void SerializeBus(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog) {
	auto buses = catalog.GetBus();
	for (auto& bus : buses) {
		proto_transport_catalog::Bus proto_bus;
		proto_bus.set_name(bus.second->name);
		for (auto* stop : bus.second->un_stop) {
			*proto_bus.mutable_stops()->Add() = stop->name;
		}
		proto_bus.set_is_roundtrip(bus.second->is_roundtrip);
		*proto_catalog.add_buses() = std::move(proto_bus);
	}
}

void SerializeDist(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog) {
	for (auto& information : catalog.GetDist()) {
		proto_transport_catalog::Distanses proto_stop_dist;
		proto_stop_dist.set_from((information.first).first);
		proto_stop_dist.set_to((information.first).second);
		proto_stop_dist.set_distance(information.second);
		*proto_catalog.add_dist() = std::move(proto_stop_dist);
	}
}

void SerializeRender(MapRender& render, proto_transport_catalog::TransportCatalogue& proto_catalog) {
    proto_map_render::MapRender proto_render;
    proto_render.set_width(render.width);
    proto_render.set_height(render.height);
    proto_render.set_padding(render.padding);
    proto_render.set_stop_radius(render.stop_radius);
    proto_render.set_line_width(render.line_width);
    proto_render.set_bus_label_font_size(render.bus_label_font_size);
    *proto_render.mutable_bus_label_offset() = SerializePoint(render.bus_label_offset);
    proto_render.set_stop_label_font_size(render.stop_label_font_size);
    *proto_render.mutable_stop_label_offset() = SerializePoint(render.stop_label_offset);
    *proto_render.mutable_underlayer_color() = SerializeColor(render.underlayer_color);
    proto_render.set_underlayer_width(render.underlayer_width);
    for (auto& color : render.color_palette) {
        *proto_render.add_color_palette() = SerializeColor(color);
    }
    *proto_catalog.mutable_render() = std::move(proto_render);
}

proto_map_render::Point SerializePoint(svg::Point& point) {
    proto_map_render::Point proto_point;
    proto_point.set_x(point.x);
    proto_point.set_y(point.y);
    return proto_point;
}

proto_map_render::Rgb SerializeRgb(svg::Rgb& rgb) {
    proto_map_render::Rgb proto_rgb;
    proto_rgb.set_red(rgb.red);
    proto_rgb.set_blue(rgb.blue);
    proto_rgb.set_green(rgb.green);
    return proto_rgb;
}

proto_map_render::Rgba SerializeRgba(svg::Rgba& rgba) {
    proto_map_render::Rgba proto_rgba;
    proto_rgba.set_red(rgba.red);
    proto_rgba.set_blue(rgba.blue);
    proto_rgba.set_green(rgba.green);
    proto_rgba.set_opacity(rgba.opacity);
    return proto_rgba;
}

proto_map_render::Color SerializeColor(svg::Color& color) {
    proto_map_render::Color proto_color;
    auto index = color.index();
    if(index == 1) {
        proto_color.set_name(std::get<1>(color));
    }
    else if (index == 2) {
        *proto_color.mutable_rgb() = SerializeRgb(std::get<2>(color));    
    }
    else if (index == 3) {
        *proto_color.mutable_rgba() = SerializeRgba(std::get<3>(color));   
    }
    return proto_color;
}

svg::Point DeserializePoint(proto_map_render::Point proto_point) {
    svg::Point point(proto_point.x(), proto_point.y());
    return point;
}

svg::Color DeserializeColor(proto_map_render::Color proto_color) {
    if (proto_color.has_rgb()) {
        svg::Rgb rgb(static_cast<uint8_t>(proto_color.rgb().red()),static_cast<uint8_t>(proto_color.rgb().green()), static_cast<uint8_t>(proto_color.rgb().blue()));
        return rgb;
    }
    else if (proto_color.has_rgba()) {
        svg::Rgba rgba(static_cast<uint8_t>(proto_color.rgba().red()),static_cast<uint8_t>(proto_color.rgba().green()),static_cast<uint8_t>(proto_color.rgba().blue()),proto_color.rgba().opacity());
        return rgba;
    }
    return proto_color.name();
}

void DeserializeStop(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog) {
	for (size_t i = 0; i < proto_catalog.stops_size(); ++i) {
		proto_transport_catalog::Stop proto_stop = proto_catalog.stops(i);
		catalog.AddStop(proto_stop.name(), { proto_stop.coordinates().lat(), proto_stop.coordinates().lng()});
        for(int j = 0; j <  proto_stop.buses_size(); ++j) {
            (catalog.FindStop(proto_stop.name()))->buses.insert((proto_stop.buses(j)));    
        } 
	}
}

void DeserializeDist(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog) {
	for (size_t i = 0; i < proto_catalog.dist_size(); ++i) {
		proto_transport_catalog::Distanses proto_stop_dist = proto_catalog.dist(i);
		transport_catalog::road_objects::Stop* from = catalog.FindStop(proto_stop_dist.from());
		transport_catalog::road_objects::Stop* to = catalog.FindStop(proto_stop_dist.to());
		catalog.SetDistance(from->name, to->name, proto_stop_dist.distance());
	}
}

void DeserializeBus(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog) {
	for (size_t i = 0; i < proto_catalog.buses_size(); ++i) {
		proto_transport_catalog::Bus proto_bus = proto_catalog.buses(i);
		std::vector<transport_catalog::road_objects::Stop*> stops(proto_bus.stops_size());
		for (size_t j = 0; j < stops.size(); ++j) {
			stops[j] = catalog.FindStop(proto_bus.stops(j));
		}
	    catalog.AddBus(proto_bus.name(), stops, proto_bus.is_roundtrip());
	}
}

MapRender DeserializeRender(proto_transport_catalog::TransportCatalogue& proto_catalog) {
    proto_map_render::MapRender proto_render = proto_catalog.render();
    MapRender render;
    render.width = proto_render.width();
    render.height = proto_render.height();
    render.padding = proto_render.padding();
    render.stop_radius = proto_render.stop_radius();
    render.line_width = proto_render.line_width();
    render.bus_label_font_size = proto_render.bus_label_font_size();
    render.bus_label_offset = DeserializePoint(proto_render.bus_label_offset());
    render.stop_label_font_size = proto_render.stop_label_font_size();
    render.stop_label_offset = DeserializePoint(proto_render.stop_label_offset());
    render.underlayer_color = DeserializeColor(proto_render.underlayer_color());
    render.underlayer_width = proto_render.underlayer_width();
    for (int i = 0; i < proto_render.color_palette_size(); ++i) {
        render.color_palette.push_back(DeserializeColor(proto_render.color_palette(i)));
    }
    return render;
}

void SerializeRouter(TransportRouter& router, proto_transport_catalog::TransportCatalogue& proto_catalog,transport_catalog::TransportCatalogue& catalog) {
    proto_router::TransportRouter proto_router;
    *proto_router.mutable_parametrs() = SerializeParametrs(router.GetParametrs(), proto_catalog);
    *proto_router.mutable_graph() = SerializeGraph(router, proto_catalog);
    for (auto i : router.GetStopIds()) {
        proto_router::StopId proto_stop_id;
        proto_stop_id.set_name(catalog.GetStopsMap().at(i.first)->name);
        proto_stop_id.set_id(i.second);
        *proto_router.add_stop_id() = proto_stop_id;
    }
    *proto_catalog.mutable_router() = std::move(proto_router);
}

proto_router::Parametrs SerializeParametrs(Parametrs& parametrs_router, proto_transport_catalog::TransportCatalogue& proto_catalog) {
    proto_router::Parametrs parametrs;
    parametrs.set_wait_time_(parametrs_router.wait_time_);
    parametrs.set_velocity_(parametrs_router.velocity_);
    return parametrs;
}

proto_graph::Graph SerializeGraph(TransportRouter& router, proto_transport_catalog::TransportCatalogue& proto_catalog) {
    proto_graph::Graph proto_graph;
    for (int i = 0; i < router.GetGraph().GetEdgeCount(); ++i) {
        graph::Edge edge = router.GetGraph().GetEdge(i);
        proto_graph::Edge proto_edge;
        proto_edge.set_name(edge.bus_name);
        proto_edge.set_span_count(edge.span_count);
        proto_edge.set_from(edge.from);
        proto_edge.set_to(edge.to);
        proto_edge.set_weight(edge.weight);
        *proto_graph.add_edge() = proto_edge;
    }
    return proto_graph;
}

Parametrs DeserializeParametrs(proto_transport_catalog::TransportCatalogue& proto_catalog) {
    Parametrs param;
    param.wait_time_ = proto_catalog.router().parametrs().wait_time_();
    param.velocity_ = proto_catalog.router().parametrs().velocity_();
    return param;
}

graph::DirectedWeightedGraph<double> DeserializeGraph(proto_transport_catalog::TransportCatalogue& proto_catalog) {
   const  proto_graph::Graph& proto_graph = proto_catalog.router().graph();
    std::vector<graph::Edge<double>> edges;
    for (int i = 0; i < proto_graph.edge_size(); ++i) {
        const proto_graph::Edge& proto_edge = proto_graph.edge(i);
        graph::Edge<double> edge;
        edge.from = static_cast<size_t>(proto_edge.from());
        edge.to = static_cast<size_t>(proto_edge.to());
        edge.weight = proto_edge.weight();
        edge.bus_name = proto_edge.name();
        edge.span_count = proto_edge.span_count();
        edges.push_back(edge);
    }
    graph::DirectedWeightedGraph<double> graph(proto_catalog.router().stop_id_size());
    for(auto i : edges) {
        graph.AddEdge(i);
    }
    return graph;
}

std::map<std::string, uint32_t> DeserializeStopId(proto_transport_catalog::TransportCatalogue& proto_catalog) {
    std::map<std::string, uint32_t> stop_ids;
    for (auto& proto_stop_id : proto_catalog.router().stop_id()) {
        stop_ids[proto_stop_id.name()] = proto_stop_id.id();
    }
    return stop_ids;
}

