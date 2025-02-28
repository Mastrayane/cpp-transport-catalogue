#include "json_reader.h"
#include "json_builder.h"

#include <fstream>
#include <sstream>

using namespace std::literals;

const json::Node& JsonReader::GetBaseRequests() const {
	auto it = input_.GetRoot().AsDict().find("base_requests");
	if (it != input_.GetRoot().AsDict().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

const json::Node& JsonReader::GetStatRequests() const {
	auto it = input_.GetRoot().AsDict().find("stat_requests");
	if (it != input_.GetRoot().AsDict().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

const json::Node& JsonReader::GetRenderSettings() const {
	auto it = input_.GetRoot().AsDict().find("render_settings");
	if (it != input_.GetRoot().AsDict().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

const json::Node& JsonReader::GetRoutingSettings() const {
	auto it = input_.GetRoot().AsDict().find("routing_settings");
	if (it != input_.GetRoot().AsDict().end()) {
		return it->second;
	}
	else {
		return dummy_;
	}
}

void JsonReader::ProcessRequests(const json::Node& stat_requests, RequestHandler& rh) const {
	json::Array result;
	for (const auto& request : stat_requests.AsArray()) {
		const auto& request_map = request.AsDict();
		const auto& type = request_map.at("type"s).AsString();

		if (type == "Stop"s) {
			result.push_back(PrintStop(request_map, rh).AsDict());
		}
		else if (type == "Bus"s) {
			result.push_back(PrintRoute(request_map, rh).AsDict());
		}
		else if (type == "Map"s) {
			// Обрабатываем запрос "Map" и добавляем результат в массив
			result.push_back(ProcessMapRequest(request, rh).AsDict());
		}
		else if (type == "Route"s) {
			result.push_back(PrintRouting(request_map, rh).AsDict());
		}
	}

	// Выводим весь результат
	json::Print(json::Document{ result }, std::cout);
}



void JsonReader::ProcessStopRequests(transport::Catalogue& catalogue) {
	const json::Array& arr = GetBaseRequests().AsArray();
	for (auto& request_stops : arr) {
		const auto& request_stops_map = request_stops.AsDict();
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
		const auto& request_bus_map = request_bus.AsDict();
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
	auto& distances = request_map.at("road_distances").AsDict();
	for (auto& [stop_name, dist] : distances) {
		stop_data.distances.try_emplace(stop_name, dist.AsInt());
	}
	return stop_data;
}

void JsonReader::FillStopDistances(transport::Catalogue& catalogue) const {
	const json::Array& arr = GetBaseRequests().AsArray();
	for (auto& request_stops : arr) {
		const auto& request_stops_map = request_stops.AsDict();
		const auto& type = request_stops_map.at("type"s).AsString();
		if (type == "Stop"s) {
			auto [stop_name, coordinates, stop_distances] = FillStop(request_stops_map);
			for (auto& [to_name, dist] : stop_distances) {
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

/*
renderer::MapRenderer JsonReader::FillRenderSettings(const json::Node& settings) const {
	json::Dict request_map = settings.AsDict();
	renderer::RenderSettings render_settings;
	render_settings.width = request_map.at("width"s).AsDouble();
	render_settings.height = request_map.at("height"s).AsDouble();
	render_settings.padding = request_map.at("padding"s).AsDouble();
	render_settings.stop_radius = request_map.at("stop_radius"s).AsDouble();
	render_settings.line_width = request_map.at("line_width"s).AsDouble();
	render_settings.bus_label_font_size = request_map.at("bus_label_font_size"s).AsInt();
	const json::Array& bus_label_offset = request_map.at("bus_label_offset"s).AsArray();
	render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
	render_settings.stop_label_font_size = request_map.at("stop_label_font_size"s).AsInt();
	const json::Array& stop_label_offset = request_map.at("stop_label_offset"s).AsArray();
	render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

	if (request_map.at("underlayer_color"s).IsString()) render_settings.underlayer_color = request_map.at("underlayer_color"s).AsString();
	else if (request_map.at("underlayer_color"s).IsArray()) {
		const json::Array& underlayer_color = request_map.at("underlayer_color"s).AsArray();
		if (underlayer_color.size() == 3) {
			render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
		}
		else if (underlayer_color.size() == 4) {
			render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
		}
		else throw std::logic_error("wrong underlayer colortype"s);
	}
	else throw std::logic_error("wrong underlayer color"s);

	render_settings.underlayer_width = request_map.at("underlayer_width"s).AsDouble();

	const json::Array& color_palette = request_map.at("color_palette"s).AsArray();
	for (const auto& color_element : color_palette) {
		if (color_element.IsString()) render_settings.color_palette.push_back(color_element.AsString());
		else if (color_element.IsArray()) {
			const json::Array& color_type = color_element.AsArray();
			if (color_type.size() == 3) {
				render_settings.color_palette.push_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
			}
			else if (color_type.size() == 4) {
				render_settings.color_palette.push_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
			}
			else throw std::logic_error("wrong color_palette type"s);
		}
		else throw std::logic_error("wrong color_palette"s);
	}

	return render_settings;
}
*/

renderer::MapRenderer JsonReader::FillRenderSettings(const json::Node& settings) const {
	json::Dict request_map = settings.AsDict();
	renderer::RenderSettings render_settings;

	// Чтение только цветовой палитры
	const json::Array& color_palette = request_map.at("color_palette").AsArray();
	for (const auto& color_element : color_palette) {
		if (color_element.IsString()) {
			render_settings.color_palette.push_back(color_element.AsString());
		}
		else if (color_element.IsArray()) {
			const json::Array& color_type = color_element.AsArray();
			if (color_type.size() == 3) {
				render_settings.color_palette.push_back(
					svg::Rgb(
						color_type[0].AsInt(),
						color_type[1].AsInt(),
						color_type[2].AsInt()
					)
				);
			}
			else if (color_type.size() == 4) {
				render_settings.color_palette.push_back(
					svg::Rgba(
						color_type[0].AsInt(),
						color_type[1].AsInt(),
						color_type[2].AsInt(),
						color_type[3].AsDouble()
					)
				);
			}
			else {
				throw std::logic_error("Wrong color format in palette");
			}
		}
		else {
			throw std::logic_error("Wrong color type in palette");
		}
	}

	return render_settings;
}


transport::RouterSettings JsonReader::FillRoutingSettings(const json::Node& settings) const {
	return transport::RouterSettings{ settings.AsDict().at("bus_wait_time"s).AsInt(), settings.AsDict().at("bus_velocity"s).AsDouble() };
}

const json::Node JsonReader::PrintRoute(const json::Dict& request_map, RequestHandler& rh) const {
	json::Node result;
	const std::string& route_number = request_map.at("name"s).AsString();
	const int id = request_map.at("id"s).AsInt();

	if (!rh.IsBusNumber(route_number)) {
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build();
	}
	else {
		const auto& route_info = rh.GetBusStat(route_number);
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("curvature"s).Value(route_info->curvature)
			.Key("route_length"s).Value(route_info->route_length)
			.Key("stop_count"s).Value(static_cast<int>(route_info->stops_count))
			.Key("unique_stop_count"s).Value(static_cast<int>(route_info->unique_stops_count))
			.EndDict()
			.Build();
	}
	return result;
}

const json::Node JsonReader::PrintStop(const json::Dict& request_map, RequestHandler& rh) const {
	json::Node result;
	const std::string& stop_name = request_map.at("name"s).AsString();
	const int id = request_map.at("id"s).AsInt();

	if (!rh.IsStopName(stop_name)) {
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build();
	}
	else {
		json::Array buses;
		for (const auto& bus : rh.GetBusesByStop(stop_name)) {
			buses.push_back(bus);
		}
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("buses"s).Value(buses)
			.EndDict()
			.Build();
	}
	return result;
}

const json::Node JsonReader::PrintMap(const json::Dict& request_map, RequestHandler& rh) const {
	json::Node result;
	const int id = request_map.at("id"s).AsInt();
	std::ostringstream strm;
	svg::Document map = rh.RenderMap();
	map.Render(strm);

	result = json::Builder{}
		.StartDict()
		.Key("request_id"s).Value(id)
		.Key("map"s).Value(strm.str())
		.EndDict()
		.Build();

	return result;
}

const json::Node JsonReader::PrintRouting(const json::Dict& request_map, RequestHandler& rh) const {
	json::Node result;
	const int id = request_map.at("id"s).AsInt();
	const std::string_view stop_from = request_map.at("from"s).AsString();
	const std::string_view stop_to = request_map.at("to"s).AsString();
	const auto& routing = rh.GetOptimalRoute(stop_from, stop_to);

	if (!routing) {
		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("error_message"s).Value("not found"s)
			.EndDict()
			.Build();
	}
	else {
		json::Array items;
		double total_time = 0.0;
		items.reserve(routing.value().edges.size());
		for (const auto& edge_info : routing.value().edges) {
			if (edge_info.quality == 0) {
				items.emplace_back(json::Node(json::Builder{}
					.StartDict()
					.Key("stop_name"s).Value(edge_info.name)
					.Key("time"s).Value(edge_info.weight)
					.Key("type"s).Value("Wait"s)
					.EndDict()
					.Build()));

				total_time += edge_info.weight;
			}
			else {
				items.emplace_back(json::Node(json::Builder{}
					.StartDict()
					.Key("bus"s).Value(edge_info.name)
					.Key("span_count"s).Value(static_cast<int>(edge_info.quality))
					.Key("time"s).Value(edge_info.weight)
					.Key("type"s).Value("Bus"s)
					.EndDict()
					.Build()));

				total_time += edge_info.weight;
			}
		}

		result = json::Builder{}
			.StartDict()
			.Key("request_id"s).Value(id)
			.Key("total_time"s).Value(total_time)
			.Key("items"s).Value(items)
			.EndDict()
			.Build();
	}

	return result;
}

void JsonReader::SaveSvgToFile(const std::string& svg_content, const std::string& filename) const {
	std::ofstream file(filename);
	if (file.is_open()) {
		file << svg_content;
		file.close();
	}
	else {
		throw std::runtime_error("Failed to save SVG to file: " + filename);
	}
}

json::Node JsonReader::ProcessMapRequest(const json::Node& request, RequestHandler& rh) const {
	// Рендерим карту в SVG
	std::ostringstream svg_stream;
	svg::Document map = rh.RenderMap();
	map.Render(svg_stream);

	// Сохраняем SVG в файл
	SaveSvgToFile(svg_stream.str(), "map.svg");

	// Формируем JSON-ответ
	return json::Builder{}
		.StartDict()
		.Key("map").Value(svg_stream.str())
		.Key("request_id").Value(request.AsDict().at("id").AsInt())
		.EndDict()
		.Build();
}