#include <iostream>
#include <Winsock2.h>
#include <thread>
#include <vector>
#include <string>
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
void chat(SOCKET cl_socket, vector<SOCKET> &clients)
{
    // After establishing connection , we use this to send & receive
    char buff[1024];
    while (1)
    {
        int msg = recv(cl_socket, buff, 1024, 0);
        if (msg <= 0)
        {
            cout << "Client disconnected" << endl;
            break;
        }
        string message(buff, msg);
        cout << "Message from client: " << message << endl;
        for (auto cl : clients)
        {
            if (cl != cl_socket)
            {
                send(cl, message.c_str(), message.length(), 0);
            }
        }
    }
    // if we are here, it means the client has disconnected
    for (auto it = clients.begin(); it != clients.end(); it++)
    {
        if (*it == cl_socket)
        {
            clients.erase(it);
            break;
        }
    }
}
int main()
{
    // For windows , we start by initialiing the WSA
    initialize();
    // We start server code
    // We start creating socket
    int nRet;
    vector<SOCKET> clients;
    SOCKET nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket == INVALID_SOCKET)
    {
        cout << " Socket not opened" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    // We now create address for the socket
    sockaddr_in srv;
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    srv.sin_addr.s_addr = INADDR_ANY; //
    // We will need to bind the Socket to port
    nRet = bind(nSocket, (sockaddr *)&srv, sizeof(srv));
    if (nRet < 0)
    {
        cout << " Failed to connect" << endl;
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    // We need to listen
    if (listen(nSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cout << "Failed to listen" << endl;
        closesocket(nSocket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    // We have successfully created the server
    cout << " The server is running" << endl;
    // We need to accept the connection
    while (1) // #####################
    {
        SOCKET cl_socket = accept(nSocket, NULL, NULL); // We are not interested in client address and code
        if (cl_socket == INVALID_SOCKET)
        {
            cout << "Failed to accept connection" << endl;
            closesocket(nSocket);
            WSACleanup(); // #####################
            exit(EXIT_FAILURE);
        }
        clients.push_back(cl_socket);
        thread t(chat, cl_socket, std::ref(clients)); // We pass the clients vector by reference so that new connections added gets reflected
    }

    // it is succesfull
}