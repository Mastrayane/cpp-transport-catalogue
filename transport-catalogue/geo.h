#pragma once

#include <cmath>



namespace transport_catalogue {

	namespace geo {

		const int RADIUS_EARTH = 6371000;

		struct Coordinates {
			double lat;
			double lng;
		};

		inline double ComputeDistance(Coordinates from, Coordinates to) {
			using namespace std;
			static const double dr = 3.1415926535 / 180.;
			return acos(sin(from.lat * dr) * sin(to.lat * dr)
				+ cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
				* RADIUS_EARTH;
		}

	}//namespace geo
}//namespace transport_catalogue
