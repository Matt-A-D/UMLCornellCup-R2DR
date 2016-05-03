#include "RSThread.h"
#include "R200.h"
#include "Server.h"
#include "iRobotCreate2.h"
#include <ctgmath>
#using <System.dll>

#define TARGET_DISTANCE 2.5
#define DISTANCE_THRESHOLD 0.2

#define TARGET_X_POSITION 0.0
#define X_POSITION_THRESHOLD 0.25

#define FALL_HEIGHT  -0.25

#define FH_SLOPE -0.0636364
#define FH_INTERCEPT -0.104545

RSThread::RSThread()
{
	iSleepMS = 0;
}

void RSThread::Start()
{
	ThreadWrapper::Start();
}

void RSThread::Entry()
{

	while (true)
	{
		// This function blocks until all samples are ready
		if (R200::SenseManager->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

		// retrieve the color and depth samples
		PXCCapture::Sample *sample = R200::SenseManager->QueryPersonTrackingSample();

		// Get person tracking data objects
		PXCPersonTrackingData* PTData = R200::PTModule->QueryOutput();

		// Make sure a person is in frame
		if (PTData->QueryNumberOfPeople() > 0)
		{
			// The person will have index 0, get their tracking data
			PXCPersonTrackingData::Person* PersonData = PTData->QueryPersonData(PXCPersonTrackingData::ACCESS_ORDER_BY_ID, 0);
			PXCPersonTrackingData::PersonTracking* PersonTracking = PersonData->QueryTracking();

			// Get center of mass (CenterMass gives access to data containing xyz coordinates of the tracked person)
			PXCPersonTrackingData::PersonTracking::PointCombined CenterMass = PersonTracking->QueryCenterMass();

			//Console::WriteLine("ANGLE = {0}", atan(CenterMass.world.point.y / CenterMass.world.point.z) * 180/ 3.141592653589793238462643383279502884197169399375105820974944592307816406286);

			//Console::WriteLine("Distance: {0}, Height: {1}", CenterMass.world.point.z, CenterMass.world.point.y);
			if (CenterMass.world.point.y < ( FH_SLOPE *(atan(CenterMass.world.point.y / CenterMass.world.point.z) * 180 / 3.141592653589793238462643383279502884197169399375105820974944592307816406286) + FH_INTERCEPT ))
			{
				__int16 br00tal_0[] = {0, 13, 39, 8, 39, 8, 39, 8, 51, 8, 57, 8, 58, 8, 57, 16, 53, 8, 57, 8, 56, 16, 52, 8, 56, 8, 55, 16};
				Server::AddCreateCommandToQueue(Server::Create2.ProcessCommand(Opcode::OPCODE_SONG, br00tal_0, 28));

				__int16 databits[] = { 0 };
				Server::AddCreateCommandToQueue(Server::Create2.ProcessCommand(Opcode::OPCODE_PLAY, databits, 1));
		
				Console::WriteLine("YOU DIED.");
				Console::WriteLine("....\n");

				
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(STOP, 0, 100, 100));

				/*
				System::Threading::Thread::Sleep(10000);	

				iRobotThread::FlushCommandQueue();
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(STOP, 0, 100, 100));*/

			}

			if (CenterMass.world.point.x > (TARGET_X_POSITION + X_POSITION_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(TURN_IN_PLACE_CW, 0, 100, 100));
			}
			else if (CenterMass.world.point.x < (TARGET_X_POSITION - X_POSITION_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(TURN_IN_PLACE_CCW, 0, 100, 100));
			}
			else
			{
				if (CenterMass.world.point.z >(TARGET_DISTANCE + DISTANCE_THRESHOLD))
				{
					Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(FORWARD, 0, 250, 100));
				}
				else if (CenterMass.world.point.z < (TARGET_DISTANCE - DISTANCE_THRESHOLD))
				{
					Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(BACKWARD, 0, 250, 100));
				}
				else
				{
					Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(STOP, 0, 100, 100));
				}
			}

			/*
			if (CenterMass.world.point.z > (TARGET_DISTANCE + DISTANCE_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(FORWARD, 0, 200, 100));
			}
			else if (CenterMass.world.point.z < (TARGET_DISTANCE - DISTANCE_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(BACKWARD, 0, 200, 100));
			}
			else
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(STOP, 0, 100, 100));
			}
			
			*/
			/*
			if (CenterMass.world.point.x > (TARGET_X_POSITION + X_POSITION_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(TURN_IN_PLACE_CW, 0, 100, 100));
			}
			else if (CenterMass.world.point.x < (TARGET_X_POSITION - X_POSITION_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(TURN_IN_PLACE_CCW, 0, 100, 100));
			}
			else
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(STOP, 0, 100, 100));
			}
			*/
	
		}
		else
		{
			Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(STOP, 0, 100, 100));
		}

		// go fetching the next samples
		R200::SenseManager->ReleaseFrame();

	}
}