#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;
using namespace cv;

enum Request{
	Stop,
	Get
};

class TCPServer{
public:
	TCPServer(int port);
	TCPServer();
	virtual ~TCPServer(void);
	void SetPort(int port);
	bool AcceptClient(void);
	bool Connect(void);
	void Disconnect(void);
	bool Listen(void);
	bool SendImage();
	void InitImages();
private:
    int m_listenPort;
    int m_serverSock;
    int m_clientSock;
    bool m_isStopped;
	cv::Mat m_image_send;
};


#endif /* TCPSERVER_H_ */
