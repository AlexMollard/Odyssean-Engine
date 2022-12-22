#include "pch.h"

#include "Audio.h"
//#include <string.h>
#include "imgui.h"

Audio::Audio()
{

	result = FMOD::Studio::System::create(&system); // Create the main system object.
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}

	result = system->initialize(512, FMOD_STUDIO_INIT_NORMAL | FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(-1);
	}



	FMOD_ErrorString(system->loadBankFile(Common_MediaPath(bank1), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank));

	
	system->loadBankFile(Common_MediaPath(bank2), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);
	{

	
	S_INFO(FMOD_ErrorString(system->loadBankFile(Common_MediaPath(bank3), FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank)));
	}

	system->getEvent("event:/Music/Level 01", &loopingAmbienceDescription);
	S_INFO(FMOD_ErrorString(loopingAmbienceDescription->createInstance(&loopingAmbienceInstance)));
}

Audio::~Audio() 
{
	{
	S_INFO(FMOD_ErrorString(sfxBank->unload()));
		sfxBank = NULL;
	}
	{
	S_INFO(FMOD_ErrorString(stringsBank->unload()));
		stringsBank = NULL;
	}
	{
	S_INFO(FMOD_ErrorString(masterBank->unload()));
		masterBank = NULL;
	}
	{
	S_INFO(FMOD_ErrorString(system->release()));
		system = NULL;
	}
	//delete bank1;
	//delete bank2;
	//delete bank3;
	gPathList.clear();
	//delete bus;
	//delete loopingAmbienceDescription.
}

void Audio::AudioInit()
{
	loopingAmbienceInstance->start();
	
	loopingAmbienceInstance->release();

	S_INFO(FMOD_ErrorString(system->getBus("bus:/", &bus)));
	bus->setVolume(1.f);
}

const char* Audio::Common_MediaPath(const char* fileName)
{
	char* filePath = (char*)calloc(256, sizeof(char));
	errno_t error_code;
	FILE* file;
	static const char* pathPrefix = nullptr;
	if (!pathPrefix)
	{

		const char* emptyPrefix = "";
		const char* mediaPrefix = "../media/";
		error_code              = fopen_s(&file, fileName, "r");
		if (file)
		{
			fclose(file);
			pathPrefix = emptyPrefix;
		}
		else
		{
			pathPrefix = mediaPrefix;
		}
	}

	strcat(filePath, pathPrefix);
	strcat(filePath, fileName);

	gPathList.push_back(filePath);

	return filePath;
}