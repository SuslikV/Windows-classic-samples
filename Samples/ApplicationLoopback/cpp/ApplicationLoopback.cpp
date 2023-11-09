// ApplicationLoopback.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include "LoopbackCapture.h"

DWORD nSR = 48000; // nSamplesPerSec
WORD wBits = 16;   // wBitsPerSample
WORD nCh = 2;      // nChannels
WORD wFormat = 1;  // wFormatTag

void usage()
{
    std::wcout <<
        L"Usage: ApplicationLoopback <pid> <includetree|excludetree> <outputfilename> [nSR] [wBits] [nCh] [wFormat]\n"
        L"\n"
        L"<pid> is the process ID to capture or exclude from capture\n"
        L"includetree includes audio from that process and its child processes\n"
        L"excludetree includes audio from all processes except that process and its child processes\n"
        L"<outputfilename> is the WAV file to receive the captured audio\n"
        L"<nSR> is sample rate, samples per second of one channel (48000)\n"
        L"<wBits> is bits per sample (16)\n"
        L"<nCh> is number of channels (2 - stereo)\n"
        L"<wFormat> is format type (1 - WAVE_FORMAT_PCM)\n"
        L"\n"
        L"Examples:\n"
        L"\n"
        L"ApplicationLoopback 1234 includetree CapturedAudio.wav\n"
        L"\n"
        L"  Captures audio from process 1234 and its children, at 48000 Hz, 16 bit, 2 channels, PCM.\n"
        L"\n"
        L"ApplicationLoopback 1234 excludetree CapturedAudio.wav\n"
        L"\n"
        L"  Captures audio from all processes except process 1234 and its children, at 48000 Hz, 16 bit, 2 channels, PCM.\n"
        L"ApplicationLoopback 1234 includetree CapturedAudio.wav 44100 16 2\n"
        L"\n"
        L"  Captures audio from process 1234 and its children, at 44100 Hz, 16 bit, 2 channels, PCM.\n";
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 4)
    {
        usage();
        return 0;
    }

    DWORD processId = wcstoul(argv[1], nullptr, 0);
    if (processId == 0)
    {
        usage();
        return 0;
    }

    bool includeProcessTree;
    if (wcscmp(argv[2], L"includetree") == 0)
    {
        includeProcessTree = true;
    }
    else if (wcscmp(argv[2], L"excludetree") == 0)
    {
        includeProcessTree = false;
    }
    else
    {
        usage();
        return 0;
    }

    PCWSTR outputFile = argv[3];

    if (argc > 4)
    {
        nSR = wcstoul(argv[4], nullptr, 0);
    }
    
    if (argc > 5)
    {
        wBits = wcstoul(argv[5], nullptr, 0);
    }

    if (argc > 6)
    {
        nCh = wcstoul(argv[6], nullptr, 0);
    }

    if (argc > 7)
    {
        wFormat = wcstoul(argv[7], nullptr, 0);
    }

    CLoopbackCapture loopbackCapture;
    HRESULT hr = loopbackCapture.StartCaptureAsync(processId, includeProcessTree, outputFile);
    if (FAILED(hr))
    {
        wil::unique_hlocal_string message;
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_ALLOCATE_BUFFER, nullptr, hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (PWSTR)&message, 0, nullptr);
        std::wcout << L"Failed to start capture\n0x" << std::hex << hr << L": " << message.get() << L"\n";
    }
    else
    {
        std::wcout << L"Capturing audio. Hit Enter to stop." << std::endl;
        getwchar();

        loopbackCapture.StopCaptureAsync();

        std::wcout << L"Finished.\n";
    }

    return 0;
}
