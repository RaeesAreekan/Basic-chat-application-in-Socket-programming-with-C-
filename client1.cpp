#include <iostream>
#include <Winsock2.h>
#include <thread>
using namespace std;
#define port 9909
void initialize()
{
    WSAData ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
    {
        cout << " WSA Startup failed" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << " WSA Startup success" << endl;
    }
}
void sendmsg(SOCKET clientsocket)
{
    cout << " Enter your name " << endl;
    string name;
    cin >> name;
    string msg;
    while (1)
    {
        cin >> msg;
        if (msg == "stop")
        {
            break;
        }
        string display = name + " : " + msg;
        int status = send(clientsocket, display.c_str(), display.size(), 0);
        if (status < 0)
        {
            cout << " Send failed" << endl;
        }
    }
    closesocket(clientsocket);
    WSACleanup();
}
void recvmsg(SOCKET clientsocket)
{
    char buff[1024];
    while (1)
    {
        int status = recv(clientsocket, buff, sizeof(buff), 0);
        if (status < 0)
        {
            cout << " Recv failed" << endl;
            break;
        }
        string msg(buff);
        cout << msg << endl;
    }
}
int main()
{
    // WSA startup
    initialize();
    // Socket creation
    SOCKET clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientsocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    // creatig address for the socket
    sockaddr_in srv;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(clientsocket, (sockaddr *)&srv, sizeof(srv)) == SOCKET_ERROR)
    {
        cout << "Connection failed" << endl;
        closesocket(clientsocket);
        WSACleanup();

        exit(EXIT_FAILURE);
    }
    cout << "Connection success" << endl;
    thread tsend(sendmsg, clientsocket);
    thread trecv(recvmsg, clientsocket);
    tsend.join();
    trecv.join(); // the main thread will wait for the tsend and trecv to complete
    WSACleanup();
}
