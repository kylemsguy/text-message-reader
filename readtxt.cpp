#include <iostream>
#include <cstdlib>
#include "pugixml/pugixml.cpp"

void showUsage(){
	std::cout << "Usage: ./readtxt [OPTIONS] [FILENAME]" << std::endl;
	std::cout << "\tOPTIONS GO HERE" << std::endl;
}

std::string getFileName(){
	std::string file_name;

	std::cout << "File> ";
	std::cin >> file_name;

	return file_name;
}

int main(int argc, char **argv){
	std::string file_name;
	pugi::xml_document doc; // text message doc

	if(argc == 1){
		file_name = getFileName();
	} 
	else if(argc == 2){
		// parse file name
		char *filename = argv[1];
		file_name = filename;
	}
	else{
		showUsage();
		return EXIT_FAILURE;
	}

	std::cout << "Reading from file " << file_name << std::endl;

	pugi::xml_parse_result result = doc.load_file(file_name.c_str());

	std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;

	return EXIT_SUCCESS;
}