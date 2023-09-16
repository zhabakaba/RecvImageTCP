#include "TCPClient.h"


TCPClient::~TCPClient(void){
	this->Disconnect();
}

TCPClient::TCPClient(std::string& serverIP, int serverPort)
: m_serverIP(serverIP),
  m_serverPort(serverPort),
  m_sock(-1)
{
}

bool TCPClient::Connect(void){
	this->m_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (this->m_sock == -1) {
		perror("Failed to create socket");
		return false;
	}

	struct sockaddr_in serverAddress{};
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(this->m_serverPort);
	if (inet_pton(AF_INET, this->m_serverIP.c_str(), &(serverAddress.sin_addr)) <= 0) {
		perror("Invalid address/ Address not supported");
		close(this->m_sock);
		return false;
	}

	if (connect(this->m_sock, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		perror("Connection failed");
		close(this->m_sock);
		return false;
	}

	cout << "Connect to server - success" << endl;

	return true;
}

void TCPClient::Disconnect(void)
{
	cout << "Client stopped" << endl;
	close(this->m_sock);
}

bool TCPClient::RecvImage(cv::Mat &image){
	size_t imageSize;
	if (recv(this->m_sock, &imageSize, sizeof(imageSize), 0) == -1) {
		perror("Failed to receive image size");
		return false;
	}

	int bytes;

	std::vector<uchar> buffer(imageSize);
	if ((bytes = recv(this->m_sock, buffer.data(), imageSize, 0)) == -1) {
		perror("Failed to receive image data");
		return false;
	}

	image = cv::imdecode(buffer, cv::IMREAD_UNCHANGED);\

	cout << "Recv image bytes = " << bytes << endl;

	return true;

}

bool TCPClient::SendRequest(Request req){
    if (send(this->m_sock, &req, sizeof(req), 0) == -1) {
        perror("Failed to send image size");
        return false;
    }
    cout << "Send request - success" << endl;
    return true;
}

std::vector<cv::Mat> TCPClient::SplitImage(cv::Mat image){
	std::vector<cv::Mat> splitImages;
	cv::Size imageSize = image.size();
	int imageHeight = imageSize.height / 8;
	for (int i = 0; i < 8; ++i)
	{
		cv::Rect roi(0, i * imageHeight, imageSize.width, imageHeight);
		cv::Mat splitImage = image(roi);
		splitImages.push_back(splitImage);
	}
	return splitImages;
}
