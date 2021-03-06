#include <stdio.h>
#include <stdlib.h>
#include <Winioctl.h>   //DISK_GEOMETRY
#include <windows.h>
#include <stddef.h>

#include <ctime>
#include <mutex>

#include "etw.h"
#include "EventTrace.h"
#include "Common.h"


/*****************************************************************************/
// structures and global variables
//
struct ETWEventCounters g_EtwEventCounters;
BOOL volatile g_bTracing = TRUE;                //true means that ETW is turned on

std::mutex mReadWriteEvent; // Mutex to protect this vector
std::vector<sDiskioTypeGroup1> vReadIO;
std::vector<sDiskioTypeGroup1> vWriteIO;


/*****************************************************************************/
// thread for gathering ETW data (etw functions are defined in etw.cpp)
//
DWORD WINAPI etwThreadFunc(LPVOID cookie)
{
	UNREFERENCED_PARAMETER(cookie);

	g_bTracing = TRUE;
	printf("ETW Thread Func -----------------------------\n");
	BOOL result = TraceEvents();

	printf("Stop ETW -----------------------------\n");
	g_bTracing = FALSE;

	return result ? 0 : 1;
}
/*****************************************************************************/
// thread for gathering ETW data (etw functions are defined in etw.cpp)
//
DWORD WINAPI etwDebug(LPVOID cookie)
{
	////UNREFERENCED_PARAMETER(cookie);
	sDebugArgs DebugArgs = *((sDebugArgs*)cookie);
	sDiskioTypeGroup1 DiskioTypeGroup1;
	clock_t begin_time = clock();
	FILE * pFile;
	errno_t err = fopen_s(&pFile, DebugArgs.logfile, "w");
	static UINT32 index = 0;
	while (g_bTracing)
	{
		if (vReadIO.size() > 0) {
			std::lock_guard<std::mutex> lg(mReadWriteEvent);
			DiskioTypeGroup1 = vReadIO[vReadIO.size() - 1];
			fprintf(pFile, "%lu	%f	%lu	%lu	%s	%lu	%lu	\n",
				++index,
				clock() / (double)CLOCKS_PER_SEC,
				DiskioTypeGroup1.HighResResponseTime,
				DiskioTypeGroup1.DiskNumber,
				"Read",
				(DiskioTypeGroup1.ByteOffset) / 512,
				(DiskioTypeGroup1.TransferSize) / 512);
			if (DebugArgs.bVerbose)
			{
				printf("%f	%5lu	%10s	%16lu	%5lu\n",
					clock() / (double)CLOCKS_PER_SEC,
					DiskioTypeGroup1.DiskNumber,
					"Read",
					(DiskioTypeGroup1.ByteOffset) / 512,
					(DiskioTypeGroup1.TransferSize) / 512);
			}
			vReadIO.pop_back();
		}
		if (vWriteIO.size() > 0) {
			std::lock_guard<std::mutex> lg(mReadWriteEvent);
			DiskioTypeGroup1 = vWriteIO[vWriteIO.size() - 1];
			fprintf(pFile, "%lu	%f	%lu	%lu	%s	%lu	%lu	\n",
				++index,
				clock() / (double)CLOCKS_PER_SEC,
				DiskioTypeGroup1.HighResResponseTime,
				DiskioTypeGroup1.DiskNumber,
				"Write",
				(DiskioTypeGroup1.ByteOffset) / 512,
				(DiskioTypeGroup1.TransferSize) / 512);

			if (DebugArgs.bVerbose)
			{
				printf("%f	%5lu	%10s	%16lu	%5lu\n",
					clock() / (double)CLOCKS_PER_SEC,
					DiskioTypeGroup1.DiskNumber,
					"Read",
					(DiskioTypeGroup1.ByteOffset) / 512,
					(DiskioTypeGroup1.TransferSize) / 512);
			}
			vWriteIO.pop_back();
		}
	}
	fclose(pFile);
	return 1;
}

bool EventTrace::GererateIORequest()
{
	HANDLE hEtwThread, hDebug;
	memset(&g_EtwEventCounters, 0, sizeof(struct ETWEventCounters));  // reset all etw event counters

	printf("starting trace session\n");
	//
	// start etw session
	//
	TRACEHANDLE hTraceSession = NULL;

	hTraceSession = StartETWSession();
	if (NULL == hTraceSession)
	{
		printf("Could not start ETW session\n");
		//_TerminateWorkerThreads(vhThreads);
		return false;
	}
	hEtwThread = CreateThread(NULL, 64 * 1024, etwThreadFunc, NULL, 0, NULL);
	if (NULL == hEtwThread)
	{
		printf("Warning: unable to create thread for ETW session\n");
		//_TerminateWorkerThreads(vhThreads);
		return false;
	}
	sDebugArgs DebugArgs = { GetLogfile(), GetVerbose() };
	hDebug = CreateThread(NULL, 0, etwDebug, &DebugArgs, 0, NULL);
	if (NULL == hDebug)
	{
		printf("Warning: unable to create thread for ETW session\n");
		//_TerminateWorkerThreads(vhThreads);
		return false;
	}



	assert(NULL != hStopEvent);
	UINT32 timeOut = (GetTimeSpan() > 0) ? 1000 * GetTimeSpan() : INFINITE;

	DWORD dwWaitStatus = WaitForSingleObject(hStopEvent, timeOut);
	if (WAIT_OBJECT_0 != dwWaitStatus && WAIT_TIMEOUT != dwWaitStatus)
	{
		printf("Error during WaitForSingleObject\n");
		return FALSE;
	}

	//Stop ETW session
	PEVENT_TRACE_PROPERTIES pETWSession = NULL;

	//printfv(profile.GetVerbose(), "stopping ETW session\n");
	pETWSession = StopETWSession(hTraceSession);
	if (NULL == pETWSession)
	{
		PrintError("Error stopping ETW session\n");
		return false;
	}

	WaitForSingleObject(hEtwThread, INFINITE);
	WaitForSingleObject(hDebug, INFINITE);
	//TerminateThread(hDebug, INFINITE);
	if (NULL != hEtwThread)
	{
		CloseHandle(hEtwThread);
	}
	if (NULL != hDebug)
	{
		CloseHandle(hDebug);
	}

	printf("Read count %lu\n", g_EtwEventCounters.ullIORead);
	printf("Write count %lu\n", g_EtwEventCounters.ullIOWrite);
	printf("tracing events\n", g_EtwEventCounters);
	return true;
}
