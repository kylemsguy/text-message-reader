#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>
#include "pugixml/pugixml.cpp"

void showUsage(){
	std::cout << "Usage: ./readtxt [OPTIONS] [FILENAME]" << std::endl;
	//std::cout << "\tOPTIONS GO HERE" << std::endl;
	std::cout << "\t-h:\tDisplays this help message." << std::endl;
	std::cout << "\t-a ADDRESS:\tSpecifies the conversation by address" << std::endl;
	std::cout << "\t-l:\tLists all conversations by address" << std::endl;
	std::cout << "\t-s:\tSends thread data directly to stdout" << std::endl;
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
		std::string date = sms.attribute("dateSent").value();
		if(date.empty())
			date = sms.attribute("date").value();
		ss << "[" << date << "] " << sms.attribute("address").value() << ": ";
		ss << sms.child_value() << std::endl;

	}

	return std::string(ss.str());

}

int main(int argc, char **argv){
	bool list_addresses = false;
	bool display_less = true;
	std::string file_name;
	std::string address;
	std::string input;
	std::string thread_string;
	pugi::xml_document doc; // text message doc
	int index;
	int c;

	opterr = 0;

	while ((c = getopt(argc, argv, "ha:ls")) != -1){
		switch(c){
			case 'a':
				address = optarg;
				break;
			case 'l':
				list_addresses = true;
				break;
			case 's':
				display_less = false;
				break;
			case '?':
				if(optopt == 'c')
					std::cerr << "Option -" << optopt << "requires an argument." << std::endl;
				else if(isprint(optopt))
					std::cerr << "Unknown option `-" << optopt << "'." << std::endl;
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					return EXIT_FAILURE;
			case 'h':
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
		thread_string = threadToString(threads.find_child_by_attribute("thread", "address", address.c_str()));
		if(display_less){
			int pip[2];
			int result;
			pid_t pid;
			result = pipe(pip);
			if(result == -1){
				perror("pipe");
				exit(EXIT_FAILURE);
			}
			pid = fork();
			if(pid > 0){
				// parent
				int status;
				close(pip[0]);
				write(pip[1], thread_string.c_str(), thread_string.length());
				close(pip[1]);
				wait(&status);
				fprintf(stderr, "Exit status: %d\n", WEXITSTATUS(status));
			}
			else if(pid == 0){
				// child
				close(pip[1]);
				dup2(pip[0], STDIN_FILENO);
				execlp("/usr/bin/less", "less", NULL);
			}
			else{
				perror("fork");
				exit(EXIT_FAILURE);
			}
		}
		else{
			std::cout << thread_string;
		}
	}

	return EXIT_SUCCESS;
}