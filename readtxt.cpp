#include <iostream>
#include <cstdio>
#include <sstream>
#include <vector>
#include <cstdlib>

#ifdef __linux__
	#include <sys/wait.h>
	#include <unistd.h>
#elif _WIN32
	#include <windows.h>
#endif

#include "pugixml/pugixml.cpp"

void show_usage(){
	std::cout << "Usage: ./readtxt [OPTIONS] [FILENAME]" << std::endl;
	//std::cout << "\tOPTIONS GO HERE" << std::endl;
	std::cout << "\t-h:\tDisplays this help message." << std::endl;
	std::cout << "\t-a ADDR: Specifies the conversation by address" << std::endl;
	std::cout << "\t-i:\tInteractive mode." << std::endl;
	std::cout << "\t-l:\tLists all conversations by address" << std::endl;
	std::cout << "\t-s:\tSends thread data directly to stdout" << std::endl;
	exit(EXIT_FAILURE);
}

std::string get_filename(){
	std::string file_name;

	std::cout << "File> ";
	std::getline(std::cin, file_name);

	return file_name;
}

std::vector<std::string> get_addresses(pugi::xml_node threads){
	std::vector<std::string> address_list;
	for (pugi::xml_node thread = threads.child("thread"); thread; thread = thread.next_sibling("thread"))
		address_list.push_back(thread.attribute("address").value());
	return address_list;
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

void send_to_less(std::string str){
	#ifdef __linux__
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
		write(pip[1], str.c_str(), str.length());
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
	#elif _WIN32
	// Somehow pipe to more here
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	if( !CreateProcess( NULL,   // No module name (use command line)
        "more",        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ){
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	#endif
}

int main(int argc, char **argv){
	bool interactive = false;
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

	while ((c = getopt(argc, argv, "iha:ls")) != -1){
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
			case 'i':
				interactive = true;
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
				show_usage();
		}
	}

	if(argc - optind > 1)
		show_usage();

	for (index = optind; index < argc; index++){
		file_name = argv[index];
	}

	if(file_name.empty() && interactive)
		file_name = get_filename();
	else
		show_usage();

	std::cout << "Reading from file " << file_name << std::endl;

	pugi::xml_parse_result result = doc.load_file(file_name.c_str());

	pugi::xml_node threads = doc.child("threads");

	std::cout << "Load result: " << result.description() << ", Thread count: " << threads.attribute("count").value() << std::endl;

	if(list_addresses){
		std::vector<std::string> addresses = get_addresses(threads);
		for(auto address: addresses){
			std::cout << address << std::endl;
		}
	}
	else{
		if(address.empty() && interactive){
			std::cout << "Address> ";
			std::getline(std::cin, input);
			address = input;
		}
		else
			show_usage();

		thread_string = threadToString(threads.find_child_by_attribute("thread", "address", address.c_str()));
		if(display_less){
			send_to_less(thread_string);
		}
		else{
			std::cout << thread_string;
		}
	}

	return EXIT_SUCCESS;
}