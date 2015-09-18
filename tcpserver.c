#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


#define bufsize 1024

void processClient(int socket);

void eatZombies(int n){
  // This function removes the zombie process state left

  wait3(NULL,WNOHANG,NULL); // Nom Nom
}

int main(int argc, char *argv[]){
  int sock, length, msgsock, status, newsockfd, n;
  struct sockaddr_in server, client;


  // for forking, and cleaning up zombie child state afterwards

  pid_t id;
  signal(SIGCHLD, &eatZombies);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);//Set up a new socket

  if(sockfd < 0)//If the socket descriptor is negative then it failed to create a new socket
  {
    fprintf(stderr,"Failed creating socket\n");
  }
  else{
    fprintf(stderr,"created socket\n");
  }

  // Next, create the socket addressing structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(atoi(argv[1])); // this time 1st arg is port#

  if(bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)//Bind the socket to the specified port
  {
    fprintf(stderr,"Failed to bind\n");
    exit(-1);
  }
  else {
    fprintf(stderr,"bound socket\n");//Print to the console saying that we have successfully bound
  }

  // Start by listening
  listen(sockfd, 5);
  fprintf(stderr,"listening\n");

  while(1){//Continue to accept connections until the program is ended

    // you need to accept the connection request
    int clientlen = sizeof(client);//Get the size of the client structure
    newsockfd = accept(sockfd, (struct sockaddr *) &client, &clientlen);//Accept a new connection and return its descriptor
    if(newsockfd < 0)
    {
      fprintf(stderr,"ERROR accepting\n");
      exit(-1);
    }

    // the next call makes a new child process that will actually handle the client.
    id = fork();

    // when id == 0, this is the child and needs to do the work for the server.
    if(id == 0)
    {
    //  close(sockfd);//close the
      processClient(newsockfd);//Read from the new client connection then send a reply
      exit(0);//exit this client after reading and writing
    }

    
    // when id < 0, we had an error.
    if(id < 0)
    {
      fprintf(stderr,"ERROR when trying to accept\n");
    }
// when if > 0, this is the parent, and it should just loop around,
  }

}

void processClient(int socket)
{
  char buf[256];//Set up a buffer to read the data into
  bzero(buf, 256);
  
  //Read the data that is on this socket
  if(read(socket, buf, 255) < 0)//If less that 0 then there was an error trying to read what the client sent
  {
    fprintf(stderr,"ERROR when trying to read\n");
    exit(-1);
  }

  printf("Message: %s\n",buf);//Print out to the console what we recieved

  //Send back a reply to the current client
  if(write(socket,"Message recieved by Server",26) < 0)//If there was an error writing then print an error and exit
  {
    fprintf(stderr,"ERROR when trying to write\n");
    exit(-1);
  }
  close(newsockfd);//Close the socket for the current child

}
