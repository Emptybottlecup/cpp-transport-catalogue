#pragma once
#include "input_reader.h"
namespace transport_catalog {
    namespace reader {
void StatReader(transport_catalog::TransportCatalogue& catalog, std::istream& input);
std::ostream& PrintBus(transport_catalog::TransportCatalogue& catalog, const std::string& bus, std::ostream& out);
std::ostream& PrintStop(transport_catalog::TransportCatalogue& catalog, const std::string& stop, std::ostream& out);        
    }
}