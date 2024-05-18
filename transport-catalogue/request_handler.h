#pragma once
 
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
 
using namespace transport_catalogue;
using namespace map_renderer;
using namespace transport_catalogue::detail::json;
using namespace transport_catalogue::detail::json::builder;
 
namespace request_handler {
 
class RequestHandler {
public:
           
    RequestHandler() = default;
    
    std::vector<geo::Coordinates> get_stops_coordinates(TransportCatalogue& catalogue_) const;
    std::vector<std::string_view> get_sort_buses_names(TransportCatalogue& catalogue_) const;
    
    BusQueryResult bus_query(TransportCatalogue& catalogue, std::string_view str);
    StopQueryResult stop_query(TransportCatalogue& catalogue, std::string_view stop_name);
    
    Node execute_make_node_stop(int id_request, const StopQueryResult& query_result);
    Node execute_make_node_bus(int id_request, const BusQueryResult& query_result);
    Node execute_make_node_map(int id_request, TransportCatalogue& catalogue, RenderSettings render_settings);
    void execute_queries(TransportCatalogue& catalogue, std::vector<StatRequest>& stat_requests, RenderSettings& render_settings);
    void execute_render_map(MapRenderer& map_catalogue, TransportCatalogue& catalogue_) const;
       
    const Document& get_document();
 
private:
    Document doc_out;
};
    
}//end namespace request_handler
