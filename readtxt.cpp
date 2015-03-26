#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include "pugixml/pugixml.cpp"

void showUsage(){
	std::cout << "Usage: ./readtxt [OPTIONS] [FILENAME]" << std::endl;
	std::cout << "\tOPTIONS GO HERE" << std::endl;
	exit(EXIT_FAILURE);
}

std::string getFileName(){
	std::string file_name;

	std::cout << "File> ";
	std::getline(std::cin, file_name);

	return file_name;
}

std::string threadToString(pugi::xml_node thread){
	std::stringstream ss;

	for (pugi::xml_node sms = thread.child("sms"); sms; sms = sms.next_sibling("sms")){
		ss << "[" << sms.attribute("dateSent").value() << "] " << sms.attribute("address").value() << ": ";
		ss << sms.child_value() << std::endl;

	}

	return std::string(ss.str());

}

int main(int argc, char **argv){
	bool list_addresses = false;
	std::string file_name;
	std::string address;
	std::string input;
	pugi::xml_document doc; // text message doc
	int index;
	int c;

	//opterr = 0;

	while ((c = getopt(argc, argv, "a:l")) != -1){
		switch(c){
			case 'a':
				address = optarg;
				break;
			case 'l':
				list_addresses = true;
				break;
			case '?':
				if(optopt == 'c')
					std::cerr << "Option -" << optopt << "requires an argument." << std::endl;
				else if(isprint(optopt))
					std::cerr << "Unknown option `-" << optopt << "'." << std::endl;
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					return EXIT_FAILURE;
			default:
				showUsage();
		}
	}

	if(argc - optind > 1)
		showUsage();

	for (index = optind; index < argc; index++){
		file_name = argv[index];
	}

	if(file_name.empty())
		file_name = getFileName();

	std::cout << "Reading from file " << file_name << std::endl;

	pugi::xml_parse_result result = doc.load_file(file_name.c_str());

	pugi::xml_node threads = doc.child("threads");

	std::cout << "Load result: " << result.description() << ", Thread count: " << threads.attribute("count").value() << std::endl;

	if(list_addresses){
		for (pugi::xml_node thread = threads.child("thread"); thread; thread = thread.next_sibling("thread")){
			std::cout << thread.attribute("address").value() << std::endl;
			//threads.push_back(thread);

		}
	}
		else{
		if(address.empty()){
			std::cout << "Address> ";
			std::getline(std::cin, input);
			address = input;
		}
		std::cout << threadToString(threads.find_child_by_attribute("thread", "address", address.c_str()));
	}

	return EXIT_SUCCESS;
}