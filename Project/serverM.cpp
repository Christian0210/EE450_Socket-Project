/*
 * File: serverM.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the main server interface, all the TCP connections to the client and
 * and all the UDP connections to the credential server, the CS server and the EE server.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <map>

#define HOSTNAME "studentVM"

#define MAXN 10000

#define SERVERCU "21381"
#define SERVERCSU "22381"
#define SERVEREEU "23381"
#define SERVERMU "24381"
#define SERVERMT "25381"

using namespace std;


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
 * Function: main(void)
 * ---------------------------------------------------------------------------------------------
 * Main function of the serverM.cpp. Deal with all the requests. 
 * [Part from "Beej's Guide to Network Programming"]
 */

int main(void)
{
	int mainsocketfd, subsockfd, bindsig, opt, T = 1, msg_len;
	struct addrinfo setting;
	struct addrinfo *server;
	struct addrinfo *iter;
	struct sockaddr_storage STaddr;
	socklen_t addlen;
	map<string, int> no = {{"Credit", 0}, {"Professor", 1}, {"Days", 2}, {"CourseName", 3}, {"All", 4}};
	map<string, string> out = {{"Credit", "credit"}, {"Professor", "professor"}, {"Days", "days"}, {"CourseName", "course name"}, {"All", "all information"}};

	memset(&setting, 0, sizeof(setting));
	setting.ai_family = AF_INET;
	setting.ai_socktype = SOCK_STREAM;
	setting.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, SERVERMT, &setting, &server);
	for(iter = server; iter != NULL; iter = iter -> ai_next) {
		mainsocketfd = socket(iter -> ai_family, iter -> ai_socktype, iter -> ai_protocol);
		if (mainsocketfd != -1) {
			opt = setsockopt(mainsocketfd, SOL_SOCKET, SO_REUSEADDR, &T, sizeof(int));
			if (opt != -1) {
				bind(mainsocketfd, iter->ai_addr, iter->ai_addrlen);
				if (bindsig != -1)
					break;
				else
					close(mainsocketfd);
			}
		}
	}
	freeaddrinfo(server);

	if (iter == NULL) {
		fprintf(stderr, "bind error\n");
		exit(1);
	}

	listen(mainsocketfd, 15);

	cout<<"The main server is up and running.\n";

	while(true) {
		addlen = sizeof STaddr;
		subsockfd = accept(mainsocketfd, (struct sockaddr *)&STaddr, &addlen);
		if (subsockfd == -1) {
			perror("accept");
			continue;
		}

		if (!fork()) { 
			close(mainsocketfd);

			char data[MAXN], query[MAXN], res[MAXN];
			res[0] = '1';

			string usn, pwd, ccd, ctg;

			while((msg_len = recv(subsockfd, data, MAXN - 1, 0)) != -1) { // Receive login information.
				
				if (msg_len == 1 && data[0] == ',') {
					break;
				}

				data[msg_len] = '\0';

				usn = "";
				pwd = "";

				int ind = 0;
				while (ind < msg_len && data[ind] != ',') {
					usn = usn + data[ind];
					ind++;
				}
				ind++;
				while (ind < msg_len) {
					pwd = pwd + data[ind];
					ind++;
				}

				cout<<"The main server received the authentication for "<<usn<<" using TCP over port "<<SERVERMT<<".\n";
				
				sendUDP(usn + "," + pwd, SERVERCU);
				cout<<"The main server sent an authentication request to serverC.\n";
				
				string result = recvUDP(SERVERMU);
				res[0] = result[0];

				cout<<"The main server received the result of the authentication request from ServerC using UDP over port "<<SERVERMU<<".\n";

				send(subsockfd, res, 5, 0);
				cout<<"The main server sent the authentication result to the client.\n";
			}

			while((msg_len = recv(subsockfd, query, MAXN - 1, 0)) > 0) { // Receice course information request.
				
				query[msg_len] = '\0';

				if (query[0] == 'M') { // A multiple request.

					ctg = "All";

					cout<<"The main server received from "<<usn<<" to query course "<<query + 1<<" about "<<out[ctg]<<" using TCP over port "<<SERVERMT<<".\n";

					string result, cresult;
					int ind = 1;
					while (ind < msg_len) {

						ccd = "";

						while (ind < msg_len && query[ind] != ',') {
							ccd = ccd + query[ind];
							ind++;
						}
						ind++;

						if (ccd.size() > 1 && ccd[0] == 'C' && ccd[1] =='S') { // A CS course request.
							sendUDP(ccd + "," + ctg, SERVERCSU);
							cout<<"The main server sent a request to serverCS.\n";
							result = recvUDP(SERVERMU);
							cout<<"The main server received the response from serverCS using UDP over port "<<SERVERMU<<".\n";
						}
						else if (ccd.size() > 1 && ccd[0] == 'E' && ccd[1] =='E') { // A EE course request.
							sendUDP(ccd + "," + ctg, SERVEREEU);
							cout<<"The main server sent a request to serverEE.\n";
							result = recvUDP(SERVERMU);
							cout<<"The main server received the response from serverEE using UDP over port "<<SERVERMU<<".\n";
						}
						else {
							cout<<ccd<<" is neither a CS nor a EE course. The main server will not sent a request for "<<ccd<<".\n";
							result = "0";
						}

						if (result[0] == '0')
							cresult = cresult + "Didn’t find the course: " + ccd + "\n";
						else
							cresult = cresult + ccd + ": " + result.substr(1) + "\n";
					}

					send(subsockfd, cresult.c_str(), cresult.length(), 0);
					cout<<"The main server sent the query information to the client.\n";
				}
				else { // A single request.
					
					ccd = "";
					ctg = "";

					int ind = 1;
					while (ind < msg_len && query[ind] != ',') {
						ccd = ccd + query[ind];
						ind++;
					}
					ind++;
					while (ind < msg_len) {
						ctg = ctg + query[ind];
						ind++;
					}

					cout<<"The main server received from "<<usn<<" to query course "<<ccd<<" about "<<out[ctg]<<" using TCP over port "<<SERVERMT<<".\n";

					string result;
					if (ccd.size() > 1 && ccd[0] == 'C' && ccd[1] =='S') { // A CS course request.
						sendUDP(ccd + "," + ctg, SERVERCSU);
						cout<<"The main server sent a request to serverCS.\n";
						result = recvUDP(SERVERMU);
						cout<<"The main server received the response from serverCS using UDP over port "<<SERVERMU<<".\n";
					}
					else if (ccd.size() > 1 && ccd[0] == 'E' && ccd[1] =='E') { // A EE course request.
						sendUDP(ccd + "," + ctg, SERVEREEU);
						cout<<"The main server sent a request to serverEE.\n";
						result = recvUDP(SERVERMU);
						cout<<"The main server received the response from serverEE using UDP over port "<<SERVERMU<<".\n";
					} 
					else {
						cout<<ccd<<" is neither a CS nor a EE course. The main server will not sent a request for "<<ccd<<".\n";
						result = "0";
					}

					send(subsockfd, result.c_str(), result.length(), 0);
					cout<<"The main server sent the query information to the client.\n";
				}
			}

			close(subsockfd);
			exit(0);
		}
		close(subsockfd);
	}

	return 0;
}

