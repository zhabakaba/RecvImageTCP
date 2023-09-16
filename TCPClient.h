#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <iostream>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

enum Request{
	Stop,
	Get
};

class TCPClient{
public:
	TCPClient(std::string& serverIP, int serverPort);
	virtual ~TCPClient(void);

	bool Connect(void);
	void Disconnect(void);

	bool RecvImage(cv::Mat &image);
	bool SendRequest(Request req);

	std::vector<cv::Mat> SplitImage(cv::Mat);
private:
    std::string m_serverIP;
    int m_serverPort;
    int m_sock;
};




#endif /* TCPCLIENT_H_ */
