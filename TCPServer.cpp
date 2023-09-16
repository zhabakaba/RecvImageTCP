#include "TCPServer.h"

TCPServer::TCPServer(int port){
	this->m_listenPort = port;
	this->m_serverSock = -1;
	this->m_clientSock = -1;
	this->m_isStopped = true;
}

TCPServer::TCPServer(){
	this->m_listenPort = 0;
	this->m_serverSock = -1;
	this->m_clientSock = -1;
	this->m_isStopped = true;
}

TCPServer::~TCPServer(){
	this->Disconnect();
}

void TCPServer::SetPort(int port){
	this->m_listenPort = port;
}

bool TCPServer::Connect(){
	int error_code;
	socklen_t error_code_size = sizeof(error_code);
	getsockopt(this->m_serverSock, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size);

	if (error_code == 0)
		return true;

	this->m_serverSock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->m_serverSock == -1) {
		perror("Failed to create socket");
		return false;
	}

	struct sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(this->m_listenPort);

	if (bind(this->m_serverSock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("Bind failed");
		close(this->m_serverSock);
		return false;
	}

	if (listen(this->m_serverSock, 1) < 0) {
		perror("Listen failed");
		close(this->m_serverSock);
		return false;
	}

	printf("Server is listening on port %d\n", this->m_listenPort);

	this->m_isStopped = false;

	this->AcceptClient();

	return true;
}

void TCPServer::Disconnect(){
	this->m_isStopped = true;
    close(this->m_clientSock);
    close(this->m_serverSock);
    cout << "Server Stopped" << endl;
}

bool TCPServer::AcceptClient(){
    this->m_clientSock = accept(this->m_serverSock, nullptr, nullptr);
	if (this->m_clientSock < 0)
	{
	  perror("Accept failed");
	  close(this->m_serverSock);
	  return false;
	}

	cout << "Accept - success" << endl;

	return true;
}

bool TCPServer::Listen(){
	Request req;

	while (!this->m_isStopped)
	{
		if (recv(this->m_clientSock, &req, sizeof(req), 0) == -1) {
			perror("Failed to receive image size");
			return false;
		}

		switch (req){
		case Request::Stop:
			cout << "Request Stop" << endl;
			this->Disconnect();
			break;
		case Request::Get:
			cout << "Request Get" << endl;
			this->SendImage();
			break;
		default:
			this->Connect();
			cout << "Undefined request " << req << endl;
			break;
		}
	}

	return true;
}

bool TCPServer::SendImage(){
	this->InitImages();

    std::vector<uchar> buffer;
    cv::imencode(".png", this->m_image_send, buffer);

    size_t imageSize = buffer.size();

    cout << "ImageSize = " << imageSize << endl;
    int bytes;

    if (send(this->m_clientSock, &imageSize, sizeof(imageSize), 0) == -1) {
        perror("Failed to send image size");
        return false;
    }

    if ((bytes = send(this->m_clientSock, buffer.data(), buffer.size(), 0)) == -1) {
        perror("Failed to send image data");
        return false;
    }

    cout << "Image send succes = " << bytes << endl;

    return true;
}

void TCPServer::InitImages(){
	vector<cv::Mat> images;

	// Первая камера
	for(int i = 0; i < 5; i++)
	{
		images.push_back(cv::imread("Images/ViTecLogo.png", cv::IMREAD_COLOR));
	}

//	 Вторая камера
	for(int i = 0; i < 5; i++)
	{
		images.push_back(cv::imread("Images/ViTecLogoNeg.png", cv::IMREAD_COLOR));
	}

	cv::vconcat(images, this->m_image_send);
}


