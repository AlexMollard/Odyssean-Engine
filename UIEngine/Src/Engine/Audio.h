#pragma once
#include "fmod_studio.h"
#include "fmod_studio.hpp"
#include <iostream>
#include "fmod.hpp"
#include "fmod.h"
#include "fmod_common.h"

#include "fmod_errors.h"
#include "ErrorHandling.h"
#include "fmod_studio_common.h"
#include <vector>
#include <string>

struct UserData
{
	FMOD::Studio::System* system = nullptr;
	FMOD::System* coreSystem     = nullptr;
	std::string filePath;
};

class Audio
{

public:

	Audio();
	~Audio();

	void AudioInit();
	void Update();

	const char* Common_MediaPath(const char* fileName);
	FMOD::Studio::System* system = NULL;
	FMOD::System* coreSystem     = NULL;
	float mainVolume             = 0.5f;
	FMOD::Studio::Bus* bus       = NULL;
private:
	UserData context = UserData();
	FMOD::Sound* sound;
	std::vector<char*> gPathList;
	FMOD::Studio::EventDescription* programmerDescription      = NULL;
	FMOD::Studio::EventInstance* programmerInstance            = NULL;
	FMOD::Studio::Bank* masterBank                             = NULL;
	FMOD::Studio::Bank* stringsBank                            = NULL;
	FMOD::Studio::Bank* sfxBank                                = NULL;

	void ERRCHECK(FMOD_RESULT result, int errorCode);
	void PlayProgrammerSound();
};