#pragma once

#include <deque>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "geo.h"

namespace transport_catalogue {

	enum RouteType {
		Direct,
		Round
	};

	namespace detail {

		template<typename Type>
		class StopHasher {
		public:
			size_t operator()(std::pair<const Type*, const Type*> stop) const {
				return hasher_(stop.first) + 47 * hasher_(stop.second);
			}

		private:
			std::hash<const Type*> hasher_;
		};

	}//namespace detail

	struct Stop {
		std::string name;
		geo::Coordinates coordinates;
	};

	using StopPtr = const Stop*;

	struct Bus {
		std::string number;
		std::vector<StopPtr> stops;
	};

	using BusPtr = const Bus*;

	struct BusStat {
		size_t number_of_stops = 0;
		size_t unique_stops = 0;
		double distance = 0.;
		uint64_t real_distance = 0;
		double curvature = 0.;
	};

	class TransportCatalogue {
	public:


		void AddStop(std::string_view name, double latitude, double longitude);

		void AddRoute(std::string_view number, std::vector<std::string_view> stops);

		BusPtr GetRoute(std::string_view name) const;

		StopPtr GetStop(std::string_view name) const;

		std::set<std::string_view> GetBuses(std::string_view stop) const;

		void SetStopDistance(std::string_view stop1, uint64_t dist, std::string_view stop2);

		uint64_t GetStopDistance(StopPtr stop1, StopPtr stop2) const;

		BusStat GetStatistics(BusPtr bus) const;

		std::set<std::string_view> SetBusByStop(StopPtr stopptr) const;


	private:
		// автобусные маршруты
		std::deque<Bus> buses_;
		// остановки
		std::deque<Stop> stops_;
		// контенер unordered_map автобусов по названию: ключ - название автобуса, значение - указатель на данные по автобусу
		std::unordered_map<std::string_view, BusPtr> bus_by_name_;

		// контейнер unordered_map остановок по названию: ключ - название остановки, значение - указатель на данные по остановке
		std::unordered_map<std::string_view, StopPtr> stop_by_name_;

		// контейнер unordered_map автобусов проходящих через остановку: ключ - указатель на остановку, значение - set автобусов, проходящих через неё
		std::unordered_map<StopPtr, std::set<std::string_view>> bus_by_stop_;
		std::unordered_map<std::pair<StopPtr, StopPtr>, uint64_t, detail::StopHasher<Stop>> di_to_stop;
	};

}//namespace transport_catalogue
