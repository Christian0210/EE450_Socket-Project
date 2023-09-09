------------------------------------------------------------------------------------------------------------------------------
 
                                  -------------------------------------------------
                                 |   EE450 Socket Programming Project, Fall 2022   |
                                  -------------------------------------------------

------------------------------------------------------------------------------------------------------------------------------

Name: Yuncong Cui
Student ID: 6204295381
Semester: Fall 2022
Course Code: EE450

------------------------------------------------------------------------------------------------------------------------------

Project Description:

    Build a students request system by TCP/UDP sockets, which includes a client, a main server, a credential server, a CS
server and a EE server. The client uses TCP to connect to the main server to send login request and course query request.
The main server uses UDP to connect to a credential server to acquire user information verification. It also uses UDP
connects to a CS server and a EE server to acquire courses information. The main server will sent back all the login and
course information to the client.
    The main server can finish two types of request: single request and multiple request (Bonus Part). Single request allows
the client to request a specific category of a specific course information. Multiple request allows the client to request
all the information of multiple courses.

                                     
                                                                                             ------> [ServerEE Port: 23381]    
                                                                                            |
                                                                                            |
  [Client Port: Dynamic] <----(TCP)----> [Port(TCP): 25381 ServerM Port(UDP): 24381] <----(UDP)----> [ServerC Port: 21381]
                                                                                            |  
                                                                                            |
                                                                                             ------> [ServerCS Port: 22381]

------------------------------------------------------------------------------------------------------------------------------

File Description:

/*
 * File: client.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the client interface and all the TCP connections to the main server. It
 * also includes the read from command line to send request to the main server.
 *
 *
 * File: serverM.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the main server interface, all the TCP connections to the client and
 * and all the UDP connections to the credential server, the CS server and the EE server.
 *
 *
 * File: serverC.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the credential server interface and all the UDP connections to the main
 * server. It also includes the file read from "cred.txt" and check if the given user name and
 * password is valid.
 *
 *
 * File: serverCS.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the CS server interface and all the UDP connections to the main server.
 * It also includes the file read from "cs.txt" and check if the given course code is valid.
 *
 *
 * File: serverEE.cpp
 * ---------------------------------------------------------------------------------------------
 * This file implements the EE server interface and all the UDP connections to the main server.
 * It also includes the file read from "ee.txt" and check if the given course code is valid.
 *
 */

------------------------------------------------------------------------------------------------------------------------------

Running Steps:

    1. Enter into command line of the root directory.
    2. Run "make all".
    3. Open four terminals to run "./serverC", "./serverCS", "./serverEE" and "./serverM" (in any order).
    4. Open a terminal to run "./client".
    5. Follow instructions to type in user name and password to login. The client will shut down if wrong login information
       has been inputted for 3 times.
    6. Follow instructions to type in single request or multiple request. Single request needs to type course code first and 
       request category then. If the input category is wrong, the client will ask user to type again. Multiple request needs 
       to type in course code splitted by space.
    7. Then the interface will show the result. If the course code is invalid, the interface will also show hints. Then user
       can start a new request.

------------------------------------------------------------------------------------------------------------------------------

Format of Message Exchange:

    1. severM --> client:   '1': Credential is valid.
                            '2': Username is wrong.
                            '3': Password is wrong.

    2. serverC --> severM:  '1': Credential is valid.
                            '2': Username is wrong.
                            '3': Password is wrong.

    3. client --> serverM   'M': Multiple request.
                            'S': Single request.

    4. serverM --> client   '0': Invalid course code.
                            '1': Course information found.
    
    5. serverEE --> serverM:'0': Invalid course code.
                            '1': Course information found.
    
    6. serverCS --> serverM:'0': Invalid course code.
                            '1': Course information found.


------------------------------------------------------------------------------------------------------------------------------

Idiosyncrasy:

    1. If the socket is binded. The dead processes need to be killed by "kill -9 [process number]".
    2. The servers can only be shut down by "Ctrl + C". The client can also be shut down by "Ctrl + C". The client will also
       be shut down if user types in the wrong user name or password for 3 times or inputs "EXIT" in the course information
       request part for the course code.
    3. ** The clietn now cannot deal with the usernames and passwords with spaces from the command line. If user inputs spaces
       during these, the system will crush. However, below the input() function of client.cpp, there is also a input() in
       comments which could be used to replace the original input() in the code. This funciton can deal with spaces. Since the 
       test input does not includes spaces, this part is in comments.
 
------------------------------------------------------------------------------------------------------------------------------

Reference Code:

    1. Several lines of code from Beej's Guide to Network Programming (socket programming) about UDP socket.
       ----serverC.cpp: sendUDP(), recvUDP
       ----serverCS.cpp: sendUDP(), recvUDP
       ----serverEE.cpp: sendUDP(), recvUDP
       ----serverM.cpp: sendUDP(), recvUDP

    2. Several lines of code from Beej's Guide to Network Programming (socket programming) about TCP socket.
       ----serverM.cpp: main()
       ----client.cpp: buildsock()
    




