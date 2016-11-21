#include "UdpSocket.hpp"
#include "XboxController.h"
#include <string>
#include <iostream>
#include <memory>
#include <csignal>
#include <cstdint>

bool stop = false;

void HandleSignal(int signal)
{
    if (signal != SIGINT)
        return;

    stop = true;
}

int wmain(int argc, wchar_t *argv[], wchar_t *envp[])
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(struct sockaddr_in));
 
    // parse command line arguments
    bool dest_set = false;
    for (int index = 1; index < argc; ++index)
    {
        std::wstring op(argv[index]);

        if (op.compare(L"-d") == 0)
        {
            if (index + 1 < argc || InetPton(AF_INET, argv[index + 1], &(dest_addr.sin_addr)) < 1)
            {
                std::wcout << L"usage: " << argv[0] << L" -d <IP Address>" << std::endl;
                return 1;
            }

            dest_set = true;
        }
    }

    if (!dest_set)
    {
        std::wcout << L"usage: " << argv[0] << L" -d <IP Address>" << std::endl;
        return 1;
    }

    signal(SIGINT, HandleSignal);

    std::unique_ptr<XboxController> controller(new XboxController());
    std::unique_ptr<UdpSocket> socket(new UdpSocket());
    if (!socket->Open())
    {
        std::wcout << L"Failed to open the udp port!" << std::endl;
        return 1;
    }

    if (!controller->Acquire())
    {
        std::wcout << L"Failed to acquire the xbox 360 controller!" << std::endl;
        return 1;
    }

    while (!stop)
    {
        uint8_t buffer[] = { 0xBE, 0xEF, 0, 2, 0, 0, 0, 0x7F };

        // fill in left, right
        buffer[5] = controller->GetLeftY();
        buffer[6] = controller->GetRightY();

        printf("Left: %3d | Right: %3d\n", controller->GetLeftY(), controller->GetRightY());

        socket->Write(buffer, 8, (struct sockaddr *) &dest_addr);
    }

    return 0;
}