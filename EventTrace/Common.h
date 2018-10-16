#pragma once

#include <windows.h>

struct ETWEventCounters
{
	UINT64 ullIORead;                   // Read
	UINT64 ullIOWrite;                  // Write
	UINT64 ullMMTransitionFault;        // Transition fault
	UINT64 ullMMDemandZeroFault;        // Demand Zero fault
	UINT64 ullMMCopyOnWrite;            // Copy on Write
	UINT64 ullMMGuardPageFault;         // Guard Page fault
	UINT64 ullMMHardPageFault;          // Hard page fault
	UINT64 ullNetTcpSend;               // Send
	UINT64 ullNetTcpReceive;            // Receive
	UINT64 ullNetUdpSend;               // Send
	UINT64 ullNetUdpReceive;            // Receive
	UINT64 ullNetConnect;               // Connect
	UINT64 ullNetDisconnect;            // Disconnect
	UINT64 ullNetRetransmit;            // ReTransmit
	UINT64 ullNetAccept;                // Accept
	UINT64 ullNetReconnect;             // ReConnect
	UINT64 ullRegCreate;                // NtCreateKey
	UINT64 ullRegOpen;                  // NtOpenKey
	UINT64 ullRegDelete;                // NtDeleteKey
	UINT64 ullRegQuery;                 // NtQueryKey
	UINT64 ullRegSetValue;              // NtSetValueKey
	UINT64 ullRegDeleteValue;           // NtDeleteValueKey
	UINT64 ullRegQueryValue;            // NtQueryValueKey
	UINT64 ullRegEnumerateKey;          // NtEnumerateKey
	UINT64 ullRegEnumerateValueKey;     // NtEnumerateValueKey
	UINT64 ullRegQueryMultipleValue;    // NtQueryMultipleValueKey
	UINT64 ullRegSetInformation;        // NtSetInformationKey
	UINT64 ullRegFlush;                 // NtFlushKey
	UINT64 ullThreadStart;
	UINT64 ullThreadEnd;
	UINT64 ullProcessStart;
	UINT64 ullProcessEnd;
	UINT64 ullImageLoad;
};