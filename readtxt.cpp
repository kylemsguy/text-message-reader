#include <iostream>
#include <vector>
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

std::string threadToString(std::xml_node thread){
	std::stringstream ss;

	for (pugi::xml_node sms = thread.child("sms"); sms; sms = sms.next_sibling("sms")){
		ss << "[" << sms.attribute("dateSent").value() << "] " << sms.attribute("address").value() << ": ";
		ss << sms.value(); << std::endl;

	}

	return ss.str();

}

int main(int argc, char **argv){
	std::string file_name;
	std::string input;
	std::vector<pugi::xml_node> threads;
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

	pugi::xml_node threads = doc.child("threads");

	std::cout << "Load result: " << result.description() << ", Thread count: " << threads.attribute("count").value() << std::endl;

	std::cout << "Address> ";
	std::cin >> input;

	/*for (pugi::xml_node thread = threads.child("thread"); thread; thread = thread.next_sibling("thread")){
		//std::cout << thread.attribute("address").value() << std::endl;
		threads.push_back(thread);

	}*/

	return EXIT_SUCCESS;
}