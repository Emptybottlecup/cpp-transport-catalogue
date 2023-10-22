#pragma once
#include "json.h"
#include "svg.h"
#include "map_renderer.h"
#include "svg.pb.h"
#include "map_renderer.pb.h"
#include "transport_catalogue.pb.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "transport_router.h"

void Serialize(transport_catalog::TransportCatalogue& catalog, MapRender& render, TransportRouter& router,std::ostream& output);
std::tuple<transport_catalog::TransportCatalogue, MapRender,TransportRouter, graph::DirectedWeightedGraph<double>, std::map<std::string, uint32_t>> Deserialize(std::istream& input);
void SerializeStop(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog);
void SerializeDist(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog);
void SerializeBus(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog);
void SerializeRender(MapRender& render,proto_transport_catalog::TransportCatalogue& proto_catalog);
proto_map_render::Point SerializePoint(svg::Point& point);
proto_map_render::Rgb SerializeRgb(svg::Rgb& rgb);
proto_map_render::Rgba SerializeRgba(svg::Rgba& rgba);
void DeserializeStop(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog);
void DeserializeDist(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog);
proto_map_render::Color SerializeColor(svg::Color& color);
void DeserializeBus(transport_catalog::TransportCatalogue& catalog, proto_transport_catalog::TransportCatalogue& proto_catalog);
MapRender DeserializeRender(proto_transport_catalog::TransportCatalogue& proto_catalog);
svg::Point DeserializePoint(proto_map_render::Point proto_point);
svg::Color DeserializeColor(proto_map_render::Color proto_color);
void SerializeRouter(TransportRouter& router, proto_transport_catalog::TransportCatalogue& proto_catalog, transport_catalog::TransportCatalogue& catalog);
proto_router::Parametrs SerializeParametrs(Parametrs& router, proto_transport_catalog::TransportCatalogue& proto_catalog);
proto_graph::Graph SerializeGraph(TransportRouter& router, proto_transport_catalog::TransportCatalogue& proto_catalog);
Parametrs DeserializeParametrs(proto_transport_catalog::TransportCatalogue& proto_catalog);
graph::DirectedWeightedGraph<double> DeserializeGraph(proto_transport_catalog::TransportCatalogue& proto_catalog);
std::map<std::string, uint32_t> DeserializeStopId(proto_transport_catalog::TransportCatalogue& proto_catalog);

