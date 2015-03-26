#include <iostream>

std::string getFileName(){
	std::string file_name;

	std::cout << "File> ";
	std::cin >> file_name;

	return file_name;
}

int main(int argc, char **argv){
	std::string file_name;
	if(argc == 1){
		file_name = getFileName();
	}
}