#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

int main(int argc, char *argv[]) {
    //terminate if command line args not provided
    if(argc < 3) {
        cout << "not enough arguments provided, expected: hostname, portnumber" << endl;
        return 1;
    }

    int sockfd, n;
    int portno;
    char buffer[256]; //store messages here

    struct sockaddr_in serv_addr; //gives us internet address
    struct hostent *server;

    portno = stoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        cout << "error opening socket" << endl;
        exit(1);
    }

    //ip address of server
    server = gethostbyname(argv[1]);
    if(server == NULL) {
        cout << "error, no such host" << endl;
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); //clear the serv address
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length); //copies bytes from first argument to second
    serv_addr.sin_port = htons(portno);

    //try connecting to server
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "connection failed" << endl;
        exit(1);
    }

    //send message to server
    bzero(buffer, 256); //clear the buffer
    strcpy(buffer, "client message");
    n = write(sockfd, buffer, strlen(buffer));
    if(n < 0){cout << "error on write" << endl; exit(1);}

    //read message from server
    bzero(buffer, 256); //clear the buffer
    n = read(sockfd, buffer, 256);
    if(n < 0) {cout << "error on read" << endl; exit(1);}
    cout << "Message from Server: " << buffer << endl;

    //close the socket
    close(sockfd);
}