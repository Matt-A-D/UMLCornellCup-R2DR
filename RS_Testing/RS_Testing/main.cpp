#include <iostream>
#include <conio.h>
#include <util_render.h>
#include <pxcsensemanager.h>
#include <pxcpersontrackingmodule.h>
#include <pxcpersontrackingdata.h>
#include <pxcpersontrackingconfiguration.h>

using namespace std;

int main()
{
	// Create SenseManager instance
	PXCSenseManager* SenseManager = PXCSenseManager::CreateInstance();

	// Enable person tracking module (the appropriate streams will be enabled by the sdk implicitly)
	SenseManager->EnablePersonTracking();

	// Get instances of the person tracking module and configuration
	PXCPersonTrackingModule* PTModule = SenseManager->QueryPersonTracking();
	PXCPersonTrackingConfiguration* PTConfig = PTModule->QueryConfiguration();

	// Enable tracking for the person tracking module
	PTConfig->QueryTracking()->Enable();

	// Initialize the pipeline
	if (SenseManager->Init() < PXC_STATUS_NO_ERROR)
	{
		cout << "FAILED TO INITIALIZE PIPELINE\n";
		system("pause");
		return 0;
	}
	else
	{
		cout << "Pipeline initialized successfully!\n";
		system("pause");
		//return 0;
	}

	// Create stream render
	UtilRender ColorStreamRender(L"Color Stream");

	// Main program loop
	for (;;)
	{
		// This function blocks until all samples are ready
		if (SenseManager->AcquireFrame(true) < PXC_STATUS_NO_ERROR) break;

		// retrieve the color and depth samples
		PXCCapture::Sample *sample = SenseManager->QueryPersonTrackingSample();

		// Get person tracking data objects
		PXCPersonTrackingData* PTData = PTModule->QueryOutput();

		// Make sure a person is in frame
		if (PTData->QueryNumberOfPeople() > 0)
		{
			// The person will have index 0, get their tracking data
			PXCPersonTrackingData::Person* PersonData = PTData->QueryPersonData(PXCPersonTrackingData::ACCESS_ORDER_BY_ID, 0);
			PXCPersonTrackingData::PersonTracking* PersonTracking = PersonData->QueryTracking();

			// Get center of mass (CenterMass gives access to data containing xyz coordinates of the tracked person)
			PXCPersonTrackingData::PersonTracking::PointCombined CenterMass = PersonTracking->QueryCenterMass();

			// Try to print distance/x/y
			cout << "Distance: " << CenterMass.world.point.z << " X: " << CenterMass.world.point.x << " Y: " << CenterMass.world.point.y;
			cout << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
		}

		// Render the color stream
		ColorStreamRender.RenderFrame(sample->color);

		// go fetching the next samples
		SenseManager->ReleaseFrame();

		// Break loop
		if (_kbhit())
		{
			int c = _getch() & 255;
			if (c == 27 || c == 'q' || c == 'Q') break; // ESC|q|Q for Exit
		}
	}

	// Close down
	SenseManager->Release();
}