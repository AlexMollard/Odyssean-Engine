#include "pch.h"

#include "Audio.h"
//#include <string.h>
#include "imgui.h"

// Programmer sound callback forward declaration
FMOD_RESULT F_CALLBACK programmerSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void* parameters);

Audio::Audio()
{
	ERRCHECK(FMOD::Studio::System::create(&system), -0); // Create the main system object.
	context.system = system;

	// Retrieving the Core system
	ERRCHECK(system->getCoreSystem(&coreSystem), -2);
	// Initializing the core system
	ERRCHECK(coreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0), -3);
	context.coreSystem = coreSystem;
	ERRCHECK(system->initialize(512, FMOD_STUDIO_INIT_NORMAL | FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL, 0), -4);

	auto s = Common_MediaPath("Audio\\Desktop\\Master.bank");
	ERRCHECK(system->loadBankFile(s, FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank), -5);
	free((void*)s);

	s = Common_MediaPath("Audio\\Desktop\\Master.strings.bank");
	S_INFO(FMOD_ErrorString(system->loadBankFile(s, FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank)));
	free((void*)s);

	s = Common_MediaPath("Audio\\Desktop\\Music.bank");
	S_INFO(FMOD_ErrorString(system->loadBankFile(s, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank)));
	free((void*)s);

	ERRCHECK(system->getEvent("event:/ProgrammerInstrument", &programmerDescription), -6);
	S_INFO(FMOD_ErrorString(programmerDescription->createInstance(&programmerInstance)));
}

Audio::~Audio()
{
	S_INFO(FMOD_ErrorString(sfxBank->unload()));
	sfxBank = NULL;

	S_INFO(FMOD_ErrorString(stringsBank->unload()));
	stringsBank = NULL;

	S_INFO(FMOD_ErrorString(masterBank->unload()));
	masterBank = NULL;

	system->resetBufferUsage();
	system->flushCommands();
	system->unloadAll();
	S_INFO(FMOD_ErrorString(system->release()));
	system = NULL;
	coreSystem->release();
	coreSystem = NULL;
}

void Audio::AudioInit()
{
	S_INFO(FMOD_ErrorString(system->getBus("bus:/", &bus)));
	bus->setVolume(mainVolume);
}

void Audio::Update()
{
	static char* buf = new char[256];
	strcpy_s(buf, 256, context.filePath.c_str());
	ImGui::Begin("Audio");
	ImGui::InputText("File Path", buf, 256);
	context.filePath = buf;
	if (ImGui::Button("Play File"))
		PlayProgrammerSound();
	ImGui::SliderFloat("Master", &mainVolume, 0, 1);
	bus->setVolume(mainVolume);
	ImGui::End();
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

	strcat_s(filePath, 256, pathPrefix);
	strcat_s(filePath, 256, fileName);

	return filePath;
}

void Audio::ERRCHECK(FMOD_RESULT result, int errorCode)
{
	if (result != FMOD_OK)
	{
		printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
		exit(errorCode);
	}
}

void Audio::PlayProgrammerSound()
{
	ERRCHECK(programmerInstance->setUserData(&context), -7);
	// Assign the callback
	ERRCHECK(programmerInstance->setCallback(programmerSoundCallback, FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND | FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED), -8);

	// Play the programmer sound
	ERRCHECK(programmerInstance->start(), -9);
}

FMOD_RESULT F_CALLBACK programmerSoundCallback(FMOD_STUDIO_EVENT_CALLBACK_TYPE type, FMOD_STUDIO_EVENTINSTANCE* event, void* parameters)
{
	FMOD::Studio::EventInstance* eventInstance = (FMOD::Studio::EventInstance*)event;

	if (!eventInstance->isValid())
		exit(-12);
	if (type == FMOD_STUDIO_EVENT_CALLBACK_CREATE_PROGRAMMER_SOUND)
	{
		FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;

		// Get our context from the event instance user data
		UserData* context = NULL;
		eventInstance->getUserData((void**)&context);

		if (!context->system->isValid())
			exit(-13);

		FMOD::Sound* sound = NULL;
		context->coreSystem->createSound(context->filePath.c_str(), FMOD_LOOP_NORMAL | FMOD_CREATECOMPRESSEDSAMPLE | FMOD_NONBLOCKING, 0, &sound);

		// Pass the sound to FMOD
		props->sound         = (FMOD_SOUND*)sound;
		props->subsoundIndex = -1;
	}
	else if (type == FMOD_STUDIO_EVENT_CALLBACK_SOUND_STOPPED)
	{
		FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;

		// Obtain the sound
		FMOD::Sound* sound = (FMOD::Sound*)props->sound;

		// Release the sound
		sound->release();
	}
	else if (type == FMOD_STUDIO_EVENT_CALLBACK_DESTROY_PROGRAMMER_SOUND)
	{
		FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES* props = (FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES*)parameters;

		// Obtain the sound
		FMOD::Sound* sound = (FMOD::Sound*)props->sound;

		// Release the sound
		if (sound != NULL)
			sound->release();
	}
	return FMOD_OK;
}
