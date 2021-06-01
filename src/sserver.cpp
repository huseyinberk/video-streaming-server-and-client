#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <iostream>
#include <map>
#include <fstream>
using namespace std;
#define LINE 13
#define LENGTH 255


int _socket;
int serverPort,IP,channelSize;
struct sockaddr_in sockaddrin;

string channelFile[3];
int clientSize[3];
sem_t *readS[3], *writeS[3];//consumer and producer
void *sheradMemory[3];
string port;//it used for split in client side

//initialize semaphore and shared memory for every channel
void initialize(int value){
    int shm_fd;
    switch(value){
        case 0:
    readS[0] = sem_open("readS_0", O_CREAT, 0600, 0);
    sem_init(readS[0], 1, 0);
    writeS[0] = sem_open("writeS_0", O_CREAT, 0600, 0);
    sem_init(writeS[0], 1, 0);
    shm_fd = shm_open("shm_0", O_CREAT | O_RDWR, 0600);
    ftruncate(shm_fd, LINE*LENGTH);
    sheradMemory[0] = mmap(NULL, LINE*LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        break;
        case 1:
            readS[1] = sem_open("readS_1", O_CREAT, 0600, 0);
    sem_init(readS[1], 1, 0);
    writeS[1] = sem_open("writeS_1", O_CREAT, 0600, 0);
    sem_init(writeS[1], 1, 0);
    shm_fd = shm_open("shm_1", O_CREAT | O_RDWR, 0600);
        ftruncate(shm_fd, LINE*LENGTH);
    sheradMemory[1] = mmap(NULL, LINE*LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        break;
        case 2:
            readS[2] = sem_open("readS_2", O_CREAT, 0600, 0);
    sem_init(readS[2], 1, 0);
    writeS[2] = sem_open("writeS_2", O_CREAT, 0600, 0);
    sem_init(writeS[2], 1, 0);
    shm_fd = shm_open("shm_2", O_CREAT | O_RDWR, 0600);
    ftruncate(shm_fd, LINE*LENGTH);
    sheradMemory[2] = mmap(NULL, LINE*LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        break;
        default:
        break;
    }


}
//read every 14 lines in the file and add line sperator with port number then return as a string
//client side port number is used for sperate lines 
string readFile(std::ifstream& pointer)
{
    string myText;
    string result = "";
    int i=0;
    for(i=0;i<14;i++){
        getline (pointer, myText);
        if(myText.find('\n') != string::npos){
            cout << "var" << endl;
            fflush(stdout);
        }
        if(myText.find('\r') != string::npos){
            cout << "@ var" << endl;
            fflush(stdout);
        }
        if(i != 0)
            result += myText +port+"\0" ;
        
        if(pointer.eof())
            pointer.seekg (0, pointer.beg);
    }
    
    return result+port+"\0";
}
//read from file and write it to the channel's shared memory 
//it is a producer
//value is channel number
void writer(int value, std::ifstream& pointer){
    char buf[LINE*LENGTH];
    while(1) {
        sem_wait(readS[value]);
        memset(buf, 0, LINE*LENGTH);
        const char *cstr = readFile(pointer).c_str();
        sprintf(buf,"%s",cstr);
        memcpy(sheradMemory[value], buf, LINE*LENGTH);
        sem_post(writeS[value]);
    }
}

//read from channel's shared memory and send it to the client
//check connection's status
//it is a consumer
void reader(){
    int addrlen = sizeof(sockaddrin);
    int max_sd,i,sd,new_socket,channelID;
    char channel[LENGTH], buf[LINE*LENGTH];

    fd_set readfds;
    while(1){
        FD_ZERO(&readfds);
        FD_SET(_socket, &readfds);
        max_sd = _socket;
        for (i=0; i<channelSize; i++) {
            sd = clientSize[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd)
                max_sd = sd;
        }

        select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (FD_ISSET(_socket, &readfds)) {
            new_socket = accept(_socket, (struct sockaddr *)&(sockaddrin), (socklen_t*)&addrlen); 
  
            for (i=0; i<channelSize; i++) {
                if (clientSize[i] == 0) {
                    clientSize[i] = new_socket;
    
                    break;
                }
            }
            printf("Client connected...\n");
            fflush(stdout);
            send(new_socket, " \n", strlen(" \n"), 0); 
        }

        for (i=0; i<channelSize; i++) {
            sd = clientSize[i];
            if (FD_ISSET(sd, &readfds)) {
                if (read(sd, channel, LENGTH - 1) == 0) {
                    getpeername(sd, (struct sockaddr *)&(sockaddrin), (socklen_t*)&addrlen);
                    printf("Client disconnected...\n");
                    fflush(stdout);
                    close(sd);
                    clientSize[i] = 0;
                } else {
                    channelID = channel[2] - '1';//calculates channel id
                    memset(buf, 0, LINE*LENGTH);
                    sem_post(readS[channelID]);
                    sem_wait(writeS[channelID]);
                    memcpy(buf, sheradMemory[channelID], LINE*LENGTH);
                    buf[strlen(buf)] = '\n';

                    send(sd, buf, strlen(buf), 0);
                }
            }
        }
    }
}

int main(int argc, char **argv){
    int i;
	map<string,int> arguments;
	arguments.insert(make_pair("-s",0));
	arguments.insert(make_pair("-p",1));
	arguments.insert(make_pair("-ch1",2));
	arguments.insert(make_pair("-ch2",3));
	arguments.insert(make_pair("-ch3",4));
    for (i=1; i<argc-1; i++) {
        switch (arguments[argv[i]])
        {
        case 0:
            channelSize = atoi(argv[++i]);
            break;
        case 1:
            serverPort = atoi(argv[++i]);
            port=argv[i];
            break;
        case 2:
            channelFile[0]=argv[++i];
            break;
        case 3:
            channelFile[1]= argv[++i];
            break;
        case 4:
            channelFile[2]=argv[++i];
            break;
        default:
            break;
        }
    }
    for (i=0; i<channelSize; i++) {
        initialize(i);
        clientSize[i] = 0;
    }
    for (i=0; i<channelSize; i++) {
        if (fork() == 0) {
	        ifstream MyReadFile(channelFile[i]);
            writer(i, MyReadFile);
        }        
    }
      _socket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddrin.sin_family = AF_INET;
	sockaddrin.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddrin.sin_port = htons(serverPort);
    bind(_socket, (struct sockaddr *)&(sockaddrin), sizeof(sockaddrin));
    listen(_socket, channelSize);
    cout << endl << "--------------------------------" << endl; 
    cout << endl << "----    Server Listening    ----" << endl; 
    cout << endl << "--------------------------------" << endl; 

    reader();
    return 0;
}