
#include <log_loader.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace Log_Loader;

std::vector<Log_Entry> Log_Loader::Load(const std::string& path) {

	std::cout << "[LOG_LOADER] Loading log ... " << std::endl;

	std::ifstream log_file;
	log_file.open(path.c_str(), std::ios_base::in);

	std::vector<Log_Entry> memmap;

	char buffer[1024];
	char* strtok_context = nullptr;

	while (!log_file.eof()) {

		log_file.getline(&buffer[0], 1024, '\n');
	
		char* token = strtok_s(buffer, " ", &strtok_context);

		// -- "[INFO]"

		if (token != nullptr && (strcmp(token, "[INFO]") == 0)) {

			token = strtok_s(NULL, " ", &strtok_context);

			// -- "Allocating"
			if (token != nullptr && (strcmp(token, "Allocating") == 0)) {

				int pages_allocated;
				int pages_location;

				// -- Number of pages allocated

				token = strtok_s(NULL, " ", &strtok_context);
				token += 2;
				pages_allocated = strtol(token, NULL, 0x10);

				// -- "pages"
				
				token = strtok_s(NULL, " ", &strtok_context);

				// -- "AT"

				token = strtok_s(NULL, " ", &strtok_context);
				
				// -- Memory location of allocated pages

				token = strtok_s(NULL, " ", &strtok_context);
				token += 2;
				pages_location = strtol(token, NULL, 0x10);

				memmap.emplace_back(pages_allocated, pages_location, lAlloc);

			}
			// -- "Freeing"	
			else if (token != nullptr && (strcmp(token, "Freeing") == 0)) {

				int pages_allocated;
				int pages_location;

				// -- Number of pages allocated

				token = strtok_s(NULL, " ", &strtok_context);
				token += 2;
				pages_allocated = strtol(token, NULL, 0x10);

				// -- "pages"
				
				token = strtok_s(NULL, " ", &strtok_context);

				// -- "AT"

				token = strtok_s(NULL, " ", &strtok_context);

				// -- Memory location of allocated pages

				token = strtok_s(NULL, " ", &strtok_context);
				token += 2;
				pages_location = strtol(token, NULL, 0x10);					

				memmap.emplace_back(pages_allocated, pages_location, lFree);

			}

		}
	
	}

	return memmap;

}