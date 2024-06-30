#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <iostream>

struct StopData {
    std::string_view name;
    geo::Coordinates coordinates;
    std::map<std::string_view, int> distances;
};

struct RouteData {
    std::string_view number;
    std::vector<const transport::Stop*> stops;
    bool circular_route;
};

class JsonReader {
public:
    JsonReader(std::istream& input)
        : input_(json::Load(input))
    {}

    const json::Node& GetBaseRequests() const;
    const json::Node& GetStatRequests() const;
    const json::Node& GetRenderSettings() const;
    const json::Node& GetRoutingSettings() const;

    void ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const;

    void ProcessStopRequests(transport::Catalogue& catalogue);
    void ProcessBusRequests(transport::Catalogue& catalogue);

    void FillCatalogue(transport::Catalogue& catalogue);
    renderer::MapRenderer FillRenderSettings(const json::Node& settings) const;
    transport::Router FillRoutingSettings(const json::Node& settings) const;

    const json::Node PrintRoute(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintStop(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintMap(const json::Dict& request_map, RequestHandler& rh) const;
    const json::Node PrintRouting(const json::Dict& request_map, RequestHandler& rh) const;

private:
    json::Document input_;
    json::Node dummy_ = nullptr;

    StopData FillStop(const json::Dict& request_map) const;
    void FillStopDistances(transport::Catalogue& catalogue) const;
    RouteData FillRoute(const json::Dict& request_map, transport::Catalogue& catalogue) const;
};