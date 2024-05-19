#include "json_reader.h"


const json::Node& JsonReader::GetBaseRequests() const {
	auto it = input_.GetRoot().AsMap().find("base_requests");
	if (it != input_.GetRoot().AsMap().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

const json::Node& JsonReader::GetStatRequests() const {
	auto it = input_.GetRoot().AsMap().find("stat_requests");
	if (it != input_.GetRoot().AsMap().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

const json::Node& JsonReader::GetRenderSettings() const {
	auto it = input_.GetRoot().AsMap().find("render_settings");
	if (it != input_.GetRoot().AsMap().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
	json::Array result;
	for (const auto& request : stat_requests.AsArray()) {
		const auto& request_map = request.AsMap();
		const auto& type = request_map.at("type").AsString();
		if (type == "Stop") {
			result.emplace_back(PrintStop(request_map, rh).AsMap());
		}
		if (type == "Bus") {
			result.emplace_back(PrintRoute(request_map, rh).AsMap());
		}
		if (type == "Map") {
			result.emplace_back(PrintMap(request_map, rh).AsMap());
		}
	}

	json::Print(json::Document{ result }, std::cout);
}

void JsonReader::ProcessStopRequests(transport::Catalogue& catalogue) {
	const json::Array& arr = GetBaseRequests().AsArray();
	for (auto& request_stops : arr) {
		const auto& request_stops_map = request_stops.AsMap();
		const auto& type = request_stops_map.at("type").AsString();
		if (type == "Stop") {
			auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
			catalogue.AddStop(stop_name, coordinates);
		}
	}
}

void JsonReader::ProcessBusRequests(transport::Catalogue& catalogue) {
	const json::Array& arr = GetBaseRequests().AsArray();
	for (auto& request_bus : arr) {
		const auto& request_bus_map = request_bus.AsMap();
		const auto& type = request_bus_map.at("type").AsString();
		if (type == "Bus") {
			auto [bus_number, stops, circular_route] = FillRoute(request_bus_map, catalogue);
			catalogue.AddRoute(bus_number, stops, circular_route);
		}
	}
}

void JsonReader::FillCatalogue(transport::Catalogue& catalogue) {
	ProcessStopRequests(catalogue);
	FillStopDistances(catalogue);
	ProcessBusRequests(catalogue);
}

StopData JsonReader::FillStop(const json::Dict& request_map) const {
	StopData stop_data;
	stop_data.name = request_map.at("name").AsString();
	stop_data.coordinates = { request_map.at("latitude").AsDouble(), request_map.at("longitude").AsDouble() };
	auto& distances = request_map.at("road_distances").AsMap();
	for (auto& [stop_name, dist] : distances) {
		stop_data.distances.try_emplace(stop_name, dist.AsInt());
	}
	return stop_data;
}

void JsonReader::FillStopDistances(transport::Catalogue& catalogue) const {
	const json::Array& arr = GetBaseRequests().AsArray();
	for (auto& request_stops : arr) {
		const auto& request_stops_map = request_stops.AsMap();
		const auto& type = request_stops_map.at("type").AsString();
		if (type == "Stop") {
			auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
			for (auto const& [to_name, dist] : stop_distances) {
				auto from = catalogue.FindStop(stop_name);
				auto to = catalogue.FindStop(to_name);
				catalogue.SetDistance(from, to, dist);
			}
		}
	}
}

RouteData JsonReader::FillRoute(const json::Dict& request_map, transport::Catalogue& catalogue) const {
	RouteData route_data;
	route_data.number = request_map.at("name").AsString();
	for (auto& stop : request_map.at("stops").AsArray()) {
		route_data.stops.push_back(catalogue.FindStop(stop.AsString()));
	}
	route_data.circular_route = request_map.at("is_roundtrip").AsBool();

	return route_data;
}

svg::Color ParseColor(const json::Node& color_node) {
	if (color_node.IsString()) {
		return color_node.AsString();
	}
	else if (color_node.IsArray()) {
		const json::Array& color_array = color_node.AsArray();
		if (color_array.size() == 3) {
			return svg::Rgb(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt());
		}
		else if (color_array.size() == 4) {
			return svg::Rgba(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt(), color_array[3].AsDouble());
		}
	}
	throw std::logic_error("wrong color");
}

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request_map) const {
	renderer::RenderSettings render_settings;
	render_settings.width = request_map.at("width").AsDouble();
	render_settings.height = request_map.at("height").AsDouble();
	render_settings.padding = request_map.at("padding").AsDouble();
	render_settings.stop_radius = request_map.at("stop_radius").AsDouble();
	render_settings.line_width = request_map.at("line_width").AsDouble();
	render_settings.bus_label_font_size = request_map.at("bus_label_font_size").AsInt();
	const json::Array& bus_label_offset = request_map.at("bus_label_offset").AsArray();
	render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
	render_settings.stop_label_font_size = request_map.at("stop_label_font_size").AsInt();
	const json::Array& stop_label_offset = request_map.at("stop_label_offset").AsArray();
	render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

	render_settings.underlayer_color = ParseColor(request_map.at("underlayer_color"));
	render_settings.underlayer_width = request_map.at("underlayer_width").AsDouble();

	const json::Array& color_palette = request_map.at("color_palette").AsArray();
	for (const auto& color_element : color_palette) {
		render_settings.color_palette.emplace_back(ParseColor(color_element));
	}

	return render_settings;
}

const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
	json::Dict result;
	const std::string& route_number = request_map.at("name").AsString();
	result["request_id"] = request_map.at("id").AsInt();
	if (!rh.IsBusNumber(route_number)) {
		result["error_message"] = json::Node{ static_cast<std::string>("not found") };
	}
	else {
		result["curvature"] = rh.GetBusStat(route_number)->curvature;
		result["route_length"] = rh.GetBusStat(route_number)->route_length;
		result["stop_count"] = static_cast<int>(rh.GetBusStat(route_number)->stops_count);
		result["unique_stop_count"] = static_cast<int>(rh.GetBusStat(route_number)->unique_stops_count);
	}

	return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
	json::Dict result;
	const std::string& stop_name = request_map.at("name").AsString();
	result["request_id"] = request_map.at("id").AsInt();
	if (!rh.IsStopName(stop_name)) {
		result["error_message"] = json::Node{ static_cast<std::string>("not found") };
	}
	else {
		json::Array buses;
		for (auto& bus : rh.GetBusesByStop(stop_name)) {
			buses.emplace_back(bus);
		}
		result["buses"] = buses;
	}

	return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
	json::Dict result;
	result["request_id"] = request_map.at("id").AsInt();
	std::ostringstream strm;
	svg::Document map = rh.RenderMap();
	map.Render(strm);
	result["map"] = strm.str();

	return json::Node{ result };
}
