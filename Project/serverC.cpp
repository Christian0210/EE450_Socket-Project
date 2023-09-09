/*
 * File: serverC.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the credential server interface and all the UDP connections to the main
 * server. It also includes the file read from "cred.txt" and check if the given user name and
 * password is valid.
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
 * Function: code(string str)
 * ---------------------------------------------------------------------------------------------
 * Encrypt the input information.
 */

string code(string str) {
	for (int i = 0; i < str.length(); i++) {
		if (str[i] >= 'A' && str[i] <= 'Z')
			str[i] = (str[i] - 61) % 26 + 65;
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] = (str[i] - 93) % 26 + 97;
		if (str[i] >= '0' && str[i] <= '9')
			str[i] = (str[i] - 44) % 10 + 48;
	}
	return str;
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
 * Read and preprocess the user data information from the file.
 */
 
 map<string, string> input() {
	
	string line, usn, pwd;
	map<string, string> usr;
	
	freopen ("cred.txt", "r", stdin);

	while ((getline(cin, line))) {
		if (wrong(line[line.length() - 1]))
			line.pop_back();
		usn = line.substr(0, line.find(','));
		pwd = line.substr(line.find(',') + 1);
		usr[usn] = pwd;
	}
     
    fclose(stdin);

	return usr;
}


/*
 * Function: main(void)
 * ---------------------------------------------------------------------------------------------
 * Main function of the serverC. Deal with the user information verification request.
 */
 
int main(void)
{
	cout<<"The ServerC is up and running using UDP on port "<<SERVERCU<<".\n";
	
	map<string, string> usr = input();

	while (true) {
		string info = recvUDP(SERVERCU);
		cout<<"The ServerC received an authentication request from the Main Server.\n";
		string usn = info.substr(0, info.find(','));
		string pwd = info.substr(info.find(',') + 1);

		usn = code(usn);
		pwd = code(pwd);

		if (usr.find(usn) == usr.end()) { // User name is invalid.
		    sendUDP("2", SERVERMU);
		}
        else if (usr[usn] != pwd) { // Password is invalid.
			sendUDP("3", SERVERMU);
		}
		else {
		    sendUDP("1", SERVERMU); // Login information is right.
		}

		cout<<"The ServerC finished sending the response to the Main Server.\n";
	}

	return 0;
}
