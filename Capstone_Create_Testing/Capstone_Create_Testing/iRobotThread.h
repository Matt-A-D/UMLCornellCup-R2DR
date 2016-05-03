#pragma once
#include "iRobotCreate2.h"
#include "ThreadWrapper.h"

public ref class iRobotThread : public ThreadWrapper
{
public: 
	// constructor
	iRobotThread();
	// call this function to start thread
	void Start() override;
	// thread entry point
	void Entry() override;
	// queue for sending commands to the create
	static ConcurrentQueue<iRobotCreate2::CreateCommand^>^ CommandQueue;
	//clear all commands
	static void FlushCommandQueue();
};
