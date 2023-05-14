#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char *argv[]) {
    //terminate if command line arg not provided
    if(argc < 2) {
        cout << "no port number provided." << endl;
        return 1;
    }

    int sockfd, newsockfd, n;
    int portno;
    char buffer[256]; //store messages here

    struct sockaddr_in serv_addr, cli_addr; //gives us internet address
    socklen_t clilen; 

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create the socket
    if(sockfd < 0) {
        cout << "could not open socket!" << endl;
        exit(1);
    }

    //bzero clears data, ensures there is nothing in it
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = stoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); //host to network short

    //assigns address to the socket
    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Binding failed" << endl;
        exit(1);
    }

    listen(sockfd, 5); //maximum limits of clients that can connect to server at a time

    //accept the connection
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    //check if connection was succesful or not
    if(newsockfd < 0) {
        cout << "error on accept" << endl;
        exit(1);
    }

    //get data from client
    bzero(buffer, 256); //clear the buffer
    n = read(newsockfd, buffer, 256);
    if(n < 0) {cout << "error on read" << endl; exit(1);}
    cout << "Message from Client: " << buffer << endl;

    //send message to client
    bzero(buffer, 256); //clear the buffer
    strcpy(buffer, "server message");
    n = write(newsockfd, buffer, strlen(buffer));
    if(n < 0){cout << "error on write" << endl; exit(1);}

    //close the sockets
    close(newsockfd);
    close(sockfd);
}