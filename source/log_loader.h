#pragma once

#include <string>
#include <vector>

enum LogOperation {

	lAlloc = 0,
	lFree

};

struct Log_Entry {

	int pages_allocated;
	int pages_location;
	LogOperation operation;

	Log_Entry() = delete;
	Log_Entry(int pages_allocated, int pages_location, LogOperation operation) : pages_allocated(pages_allocated), pages_location(pages_location), operation(operation) {};

};

namespace Log_Loader {

	std::vector<Log_Entry> Load(const std::string& path);

}