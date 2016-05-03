#include "R200.h"

bool R200::Init()
{
	// Create SenseManager instance
	SenseManager = PXCSenseManager::CreateInstance();

	// Enable person tracking module (the appropriate streams will be enabled by the sdk implicitly)
	SenseManager->EnablePersonTracking();

	// Get instances of the person tracking module and configuration
	PTModule = SenseManager->QueryPersonTracking();
	PTConfig = PTModule->QueryConfiguration();

	// Enable tracking for the person tracking module
	PTConfig->QueryTracking()->Enable();

	// Initialize the pipeline
	if (SenseManager->Init() < PXC_STATUS_NO_ERROR)
	{
		// Pipeline failed to initialize
		return false;
	}
	
	// Pipeline initialized successfully
	return true;
}

void R200::Cleanup()
{
	//SenseManager->Release();
}