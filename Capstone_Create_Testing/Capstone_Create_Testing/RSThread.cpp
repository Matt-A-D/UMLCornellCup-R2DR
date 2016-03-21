#include "RSThread.h"
#include "R200.h"
#include "Server.h"

#define TARGET_DISTANCE 2.0
#define DISTANCE_THRESHOLD 0.2

#define TARGET_X_POSITION 0.0
#define X_POSITION_THRESHOLD 0.25

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

			Console::WriteLine("Distance: {0}", CenterMass.world.point.z);

			if (CenterMass.world.point.x > (TARGET_X_POSITION + X_POSITION_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(TURN_IN_PLACE_CW, 0, 200, 100));
			}
			else if (CenterMass.world.point.x < (TARGET_X_POSITION - X_POSITION_THRESHOLD))
			{
				Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(TURN_IN_PLACE_CCW, 0, 200, 100));
			}
			else
			{
				if (CenterMass.world.point.z >(TARGET_DISTANCE + DISTANCE_THRESHOLD))
				{
					Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(FORWARD, 0, 300, 100));
				}
				else if (CenterMass.world.point.z < (TARGET_DISTANCE - DISTANCE_THRESHOLD))
				{
					Server::AddCreateCommandToQueue(Server::Create2.ProcessRadialDriveCommand(BACKWARD, 0, 300, 100));
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