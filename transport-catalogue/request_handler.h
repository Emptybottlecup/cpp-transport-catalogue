#pragma once
#include "json_reader.h"

namespace transport_catalog {
    
void Requests(TransportCatalogue& cata, json::Document& doc,MapRender map_);

void ReadRequests(TransportCatalogue& cata, std::istream& input);
 
}
