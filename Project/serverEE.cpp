/*
 * File: serverEE.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the EE server interface and all the UDP connections to the main server.
 * It also includes the file read from "ee.txt" and check if the given course code is valid.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#define HOSTNAME "studentVM"

#define SERVERCU "21381"
#define SERVERCSU "22381"
#define SERVEREEU "23381"
#define SERVERMU "24381"
#define SERVERMT "25381"

#define MAXN 10000

using namespace std;

struct acc {
	string usn;
	string pwd;
};


/*
 * Function: sendUDP(string info, string port)
 * ---------------------------------------------------------------------------------------------
 * Build UDP connection and send a piece of information from the selected port. 
 * [Part from "Beej's Guide to Network Programming"]
 */

void sendUDP(string info, string port) {
	int sockUDP;
	struct addrinfo pre;
	struct addrinfo *ser;
	struct addrinfo *pser;
	memset(&pre, 0, sizeof(pre));
	pre.ai_socktype = SOCK_DGRAM;
	pre.ai_family = AF_INET;

	getaddrinfo(HOSTNAME, port.c_str(), &pre, &ser);
	for (pser = ser; pser != NULL; pser = pser->ai_next) {
		sockUDP = socket(pser->ai_family, pser->ai_socktype, pser->ai_protocol);
		if (sockUDP != -1)
			break;
	}

	sendto(sockUDP, info.c_str(), info.length(), 0, pser->ai_addr, pser->ai_addrlen);
	freeaddrinfo(ser);
	close(sockUDP);
}


/*
 * Function: recvUDP(string port)
 * ---------------------------------------------------------------------------------------------
 * Build UDP connection and receive a piece of information from the selected port. 
 * [Part from "Beej's Guide to Network Programming"]
 */

string recvUDP(string port) {
	int sockUDP, bindUDP, infolen;
	struct addrinfo pre;
	struct addrinfo *ser;
	struct addrinfo *pser;
	struct sockaddr_storage STaddr;
	char info[MAXN];
	string result;
	socklen_t addlen;

	memset(&pre, 0, sizeof(pre));
	pre.ai_family = AF_INET;
	pre.ai_socktype = SOCK_DGRAM;
	pre.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, port.c_str(), &pre, &ser);

	for(pser = ser; pser != NULL; pser = pser->ai_next) {
		sockUDP = socket(pser->ai_family, pser->ai_socktype, pser->ai_protocol);
		if (sockUDP != -1) {
			bindUDP = bind(sockUDP, pser->ai_addr, pser->ai_addrlen);
			if (bindUDP != -1)
				break;
		}
	}

	freeaddrinfo(ser);
	addlen = sizeof(STaddr);
	infolen = recvfrom(sockUDP, info, MAXN - 1 , 0, (struct sockaddr *)&STaddr, &addlen);
	info[infolen] = '\0';
	for (int i = 0; i < infolen; i++)
		result = result + info[i];
	close(sockUDP);

	return result;
}


/*
 * Function: wrong (char c)
 * ---------------------------------------------------------------------------------------------
 * Check the invalid character in the Linux text file.
 */

bool wrong (char c) {
	if (c == '\n' || c == '\b' || c == '\f' || c == '\r' || c == '\t' || c == '\v' || c == '\0' || c == '\a')
		return true;
	else
		return false;
}


/*
 * Function: input()
 * ---------------------------------------------------------------------------------------------
 * Read and preprocess the EE course information from the file.
 */
 
 map<string, vector<string>> input() {
	
	string line, ccd, pwd;
	map<string, vector<string>> clm;
	vector<string> tmp;

	freopen ("ee.txt", "r", stdin);

	while (getline(cin, line)){
		int st = line.find(','), nt;
		if (wrong(line[line.length() - 1]))
			line.pop_back();
		ccd = line.substr(0, st);
		clm[ccd] = tmp;
		for (int i = 0; i < 4; i++) {
			nt = line.find(',', st + 1);
			clm[ccd].push_back(line.substr(st + 1, nt - st - 1));
			st = nt;
		}
		clm[ccd].push_back(line.substr(line.find(',') + 1));
	}
     
    fclose(stdin);

	return clm;
}


/*
 * Function: main(void)
 * ---------------------------------------------------------------------------------------------
 * Main function of the serverEE. Deal with the EE course request.
 */

int main(void)
{
	cout<<"The ServerEE is up and running using UDP on port "<<SERVEREEU<<".\n";
	
	map<string, vector<string>> clm = input();
	map<string, int> no = {{"Credit", 0}, {"Professor", 1}, {"Days", 2}, {"CourseName", 3}, {"All", 4}};
	map<string, string> out = {{"Credit", "credit"}, {"Professor", "professor"}, {"Days", "days"}, {"CourseName", "course name"}, {"All", "all information"}};

	while (true) {
		string info = recvUDP(SERVEREEU);
		string ccd = info.substr(0, info.find(','));
		string ctg = info.substr(info.find(',') + 1);

		cout<<"The ServerEE received a request from the Main Server about the "<<out[ctg]<<" of "<<ccd<<".\n";

		if (clm.find(ccd) == clm.end()) { // Course code is invalid.
			cout<<"Didn’t find the course: "<<ccd<<".\n";
		    sendUDP("0", SERVERMU);
		}
    	else { // Course found.
			cout<<"The course information has been found: The "<<out[ctg]<<" of "<<ccd<<" is "<<clm[ccd][no[ctg]]<<".\n";
			sendUDP("1" + clm[ccd][no[ctg]], SERVERMU);
		}

		cout<<"The ServerEE finished sending the response to the Main Server.\n";
	}

	return 0;
}
