#pragma once


#include "etw.h"


struct sDebugArgs
{
	const char* logfile;
	bool bVerbose;
};

class EventTrace
{
public:
	EventTrace() :
		sLogFile(nullptr),
		uTimeSpan(0)
	{}
	bool GererateIORequest();

public:
	void SetLogFile(const char* log) { sLogFile = log; }
	const char* GetLogfile() { return sLogFile; }
	void SetTimeSpan(UINT32 time) { uTimeSpan = time; }
	UINT32 GetTimeSpan() const { return uTimeSpan; }
	void SetVerbose(bool b) { bVerbose = b; }
	bool GetVerbose() const { return bVerbose; }

private:
	const char* sLogFile;
	UINT32 uTimeSpan;
	bool bVerbose;

	HANDLE hStopEvent;      //an event to be signalled if the scenario is to be stop before time ellapses
};