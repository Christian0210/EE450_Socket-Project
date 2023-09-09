/*
 * File: client.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the client interface and all the TCP connections to the main server. It
 * also includes the read from command line to send request to the main server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <vector>

using namespace std;

#define HOSTNAME "studentVM"
#define MAXN 10000

#define SERVERCU "21381"
#define SERVERCSU "22381"
#define SERVEREEU "23381"
#define SERVERMU "24381"
#define SERVERMT "25381"


struct acc {
	string usn;
	string pwd;
};


/*
 * Function: input (int sockfd, int loc_port)
 * ---------------------------------------------------------------------------------------------
 * Get the input of user name and password.
 */

acc input (int sockfd, int loc_port) {
	int chc = 3;
	acc inputusr;

	while (chc) {
		cout<<"Please enter the username: ";
		cin >> inputusr.usn;
		cout<<"Please enter the password: ";
		cin >> inputusr.pwd;

		char res[MAXN];
		string data = inputusr.usn + "," + inputusr.pwd;

		send(sockfd, data.c_str(), data.length(), 0);

		cout<<inputusr.usn<<" sent an authentication request to the main server.\n";

		recv(sockfd, res, MAXN-1, 0);
	
		if (res[0] == '1') { // The information is valid.
			cout<<inputusr.usn<<" received the result of authentication using TCP over port "<<loc_port<<". Authentication is successful\n";
			break;
		}
		
		chc--;

		if (res[0] == '2') { // Username is wrong.
			cout<<inputusr.usn<<" received the result of authentication using TCP over port "<<loc_port<<". Authentication failed: Username Does not exist\n\nAttempts remaining: "<<chc<<"\n";
		}
		if (res[0] == '3') { // Password is wrong
			cout<<inputusr.usn<<" received the result of authentication using TCP over port "<<loc_port<<". Authentication failed: Password does not match\n\nAttempts remaining: "<<chc<<"\n";
		}
	}

	send(sockfd, ",", 1, 0);

	if (chc == 0) { // The information is worng for 3 times.
		cout<<"Authentication Failed for 3 attempts. Client will shut down.\n";
		exit(0);
	}
	return inputusr;
}


/*
 * Function: input (int sockfd, int loc_port)
 * ---------------------------------------------------------------------------------------------
 * Get the input of user name and password (with spaces).
 */

/*
acc input (int sockfd, int loc_port) {
	int chc = 3;
	acc inputusr;
	char x;

	while (chc) {
		cout<<"Please enter the username: ";
		inputusr.usn = "";
		while ((x = cin.get()) != '\n') {
			inputusr.usn = inputusr.usn + x;
		}
		cout<<"Please enter the password: ";
		inputusr.pwd = "";
		while ((x = cin.get()) != '\n') {
			inputusr.pwd = inputusr.pwd + x;
		}

		char res[MAXN];
		string data = inputusr.usn + "," + inputusr.pwd;

		send(sockfd, data.c_str(), data.length(), 0);

		cout<<inputusr.usn<<" sent an authentication request to the main server.\n";

		recv(sockfd, res, MAXN-1, 0);
	
		if (res[0] == '1') { // The information is valid.
			cout<<inputusr.usn<<" received the result of authentication using TCP over port "<<loc_port<<". Authentication is successful\n";
			break;
		}
		
		chc--;

		if (res[0] == '2') { // Username is wrong.
			cout<<inputusr.usn<<" received the result of authentication using TCP over port "<<loc_port<<". Authentication failed: Username Does not exist\n\nAttempts remaining: "<<chc<<"\n";
		}
		if (res[0] == '3') { // Password is wrong
			cout<<inputusr.usn<<" received the result of authentication using TCP over port "<<loc_port<<". Authentication failed: Password does not match\n\nAttempts remaining: "<<chc<<"\n";
		}
	}

	send(sockfd, ",", 1, 0);

	if (chc == 0) { // The information is worng for 3 times.
		cout<<"Authentication Failed for 3 attempts. Client will shut down.\n";
		exit(0);
	}
	return inputusr;
}
*/

/*
 * Function: singleq(acc usr, int sockfd, string clist, int loc_port)
 * ---------------------------------------------------------------------------------------------
 * Deal with the user's request with single course.
 */

void singleq(acc usr, int sockfd, string clist, int loc_port) {
	
	string ctg;
	char res[MAXN];
	int msg_len;
		
	map<string, string> out = {{"Credit", "credit"}, {"Professor", "professor"}, {"Days", "days"}, {"CourseName", "course name"}};

	while (true) {
		cout<<"Please enter the category (Credit / Professor / Days / CourseName): ";
		cin>>ctg;

		if (out.find(ctg) == out.end()) { // Ask user to input category again.
			cout<<"Wrong category input.\n";
			continue;
		}

		string data =  "S" + clist + "," + ctg;

		send(sockfd, data.c_str(), data.length(), 0);

		cout<<usr.usn<<" sent a request to the main server.\n";

		msg_len = recv(sockfd, res, MAXN-1, 0);

		cout<<"The client received the response from the Main server using TCP over port "<<loc_port<<".\n";

		if (res[0] == '0') { // Course code is wrong.
			cout<<"Didn’t find the course: "<<clist<<"\n";
		}
		else { // Course code is right. Send back related information.
			string output;
			for (int i = 1; i < msg_len; i++)
				output = output + res[i];
			cout<<"The "<< out[ctg].c_str()<<" of "<<clist.c_str()<<" is "<<output.c_str()<<".\n";
		}

		break;
	}
}


/*
 * Function: multipleq(acc usr, int sockfd, string clist, int loc_port)
 * ---------------------------------------------------------------------------------------------
 * Deal with the user's request with multiple courses.
 */

void multipleq(acc usr, int sockfd, string clist, int loc_port) {
	
	char res[MAXN];
	int msg_len;

	clist =  "M" + clist;

	send(sockfd, clist.c_str(), clist.length(), 0);

	cout<<usr.usn<<" sent a request with multiple CourseCode to the main server.\n";

	msg_len = recv(sockfd, res, MAXN-1, 0);

	cout<<"The client received the response from the Main server using TCP over port "<<loc_port<<".\n";
	cout<<"CourseCode: Credits, Professor, Days, Course Name\n";

	for (int i = 0; i < msg_len; i++)
		cout<<res[i];
}


/*
 * Function: work (acc usr, int sockfd, int loc_port)
 * ---------------------------------------------------------------------------------------------
 * Deal with the user's request. Acquire the course code and decide if the request
 * is a single request of mutiple request.
 */

void work (acc usr, int sockfd, int loc_port) {

	string clist, c;
	char x;
	
	while (true) {
		cout<<"Please enter the course code to query: ";
		
		bool tag = false;
		c="";
		cin>>clist;
		while ((x = cin.get()) != '\n') {
			if (x == ' ') {
				if (c.length() > 0) {
					clist = clist + ',' + c;
					tag = true;
					c = "";
				}
			}
			else
				c = c + x;
		}
		if (c.length() > 0) {
			if (clist.length() > 0) {
				clist = clist + ',';
				tag = true;
			}
			clist = clist + c;

		}

		if (clist == "EXIT")
			break;

		if (tag) // A mutiple request
			multipleq(usr, sockfd, clist, loc_port);	
		else // A single request
			singleq(usr, sockfd, clist, loc_port);
		
		cout<<"\n\n-----Start a new request-----\n";
	}
}


/*
 * Function: buildsock()
 * ---------------------------------------------------------------------------------------------
 * Build the socket for TCP connection. Connect to the serverM.
 * [Part from "Beej's Guide to Network Programming"]
 */

int buildsock() {
	int mainsocketfd, connection;  
	struct addrinfo setting;
	struct addrinfo *server;
	struct addrinfo *iter;
	
	memset(&setting, 0, sizeof setting);
	setting.ai_family = AF_INET;
	setting.ai_socktype = SOCK_STREAM;
	getaddrinfo(HOSTNAME, SERVERMT, &setting, &server);

	for(iter = server; iter != NULL; iter = iter->ai_next) {
		mainsocketfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
		if (mainsocketfd != -1) {
			connection = connect(mainsocketfd, iter->ai_addr, iter->ai_addrlen);
			if (connection != -1)
				break;
		}
	}

	freeaddrinfo(server);

	return mainsocketfd;
}


/*
 * Function: main(void)
 * ---------------------------------------------------------------------------------------------
 * Main function of the client.
 */

int main(void) {

	int mainsocketfd = buildsock();
	struct sockaddr_in loc_add;
	socklen_t loc_add_len = sizeof(loc_add);

	memset(&loc_add, 0, loc_add_len);
	getsockname(mainsocketfd, (struct sockaddr *)&loc_add, &loc_add_len);

	int loc_port = ntohs(loc_add.sin_port); // Get the client port number.

	cout<<"The client is up and running.\n";

	acc usr = input(mainsocketfd, loc_port);

	work(usr, mainsocketfd, loc_port);

	cout<<"\nClient will shut down.\n";

	close(mainsocketfd);

	return 0;
}

