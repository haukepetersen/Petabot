/*
 ============================================================================
 Name        : ImageReciever.c
 Author      : Stefan Sonski
 Version     :
 Copyright   : 
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include <stdio.h>

#define MAXPENDING 5
#define PORT 9999

#define WIDTH 640
#define HEIGHT 480

IplImage *image;

struct sockaddr_in serverAddr;
struct sockaddr_in clientAddr;
int serverSocket;
int clientSocket;
unsigned int clntLen;

void dieWithError(char *errorMessage) {
	perror(errorMessage);
	exit(1);
}

void initialize() {
	image = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
	cvNamedWindow("result", 0);

	if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		dieWithError("socket() failed");

	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = 0;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;

	if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr))
			< 0)
		dieWithError("bind() failed");

	if (listen(serverSocket, MAXPENDING) < 0)
		dieWithError("listen() failed");

	if ((clientSocket = accept(serverSocket, NULL, NULL)) < 0)
		dieWithError("accept() failed");
}

IplImage * recieveImage(int clientSocket) {
	int imgsize = image->imageSize;
	unsigned int bytesReceived = 0;
	char sockdata[imgsize];

	int i;

	for (i = 0; i < imgsize; i += bytesReceived) {
		if ((bytesReceived = recv(clientSocket, sockdata + i, imgsize - i, 0))
				== -1) {
			dieWithError("recv failed");
		}
	}

	image->imageData = sockdata;

	return image;
}

void loadImage() {
	while (1) {
		image = recieveImage(clientSocket);

		cvShowImage("result", image);
		cvWaitKey(10);
	}
}

int main(void) {
	initialize();
	loadImage();
}

