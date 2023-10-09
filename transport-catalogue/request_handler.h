#pragma once 
#include "json_reader.h" 
#include "transport_router.h"
namespace transport_catalog { 
     
void Requests(TransportCatalogue& cata, json::Document& doc,MapRender map_, transport::Router& router); 
 
void ReadRequests(TransportCatalogue& cata, std::istream& input); 
  
}