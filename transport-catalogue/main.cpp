#include <iostream>
#include <string>
#include <cassert>
#include <vector>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;


int main() {

	transport_catalogue::TransportCatalogue catalogue;
	// РЕАЛИЗОВАНЫ 2 ВАРИАНТА НА ВЫЗОВ ЗАПОЛНЕНИЯ БАЗЫ ДАННЫХ:

	// Вариант 1 (Поток ввода выведен в аргумент)
	{
		InputReader reader;
		reader.ReadingStream(catalogue, reader, cin);
	}



	/*
	// Вариант 2 (Поток ввода вне)
	int base_request_count;
	cin >> base_request_count >> ws;

	{
		InputReader reader;
		reader.ReadingStream(catalogue, reader, base_request_count);

	}
	*/


	//_________________________________________________
	// РЕАЛИЗОВАНЫ 2 ВАРИАНТА ВЫЗОВА ЗАПРОСА СТАТИСТИКИ

	// Вариант 1 (Поток ввода выведен в аргумент);
	RequestStatistics(catalogue, cin);

	/*
	// Вариант 2 (Поток ввода вне)
	int stat_request_count;
	cin >> stat_request_count >> ws;

	RequestStatistics(catalogue, cin, stat_request_count);
	*/

}
