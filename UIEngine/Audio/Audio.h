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

class Audio
{

public:

	Audio();
	~Audio();

	void AudioInit();
	const char* Common_MediaPath(const char* fileName);
	FMOD::Studio::System* system = NULL;


	private:
	FMOD_RESULT result;

	FMOD::Sound* sound;
	std::vector<char*> gPathList;
	FMOD::Studio::EventDescription* loopingAmbienceDescription = NULL;
	FMOD::Studio::EventInstance* loopingAmbienceInstance       = NULL;
	FMOD::Studio::Bank* masterBank                             = NULL;
	FMOD::Studio::Bank* stringsBank                            = NULL;
	FMOD::Studio::Bank* sfxBank                                = NULL;
	FMOD::Studio::Bus* bus;

	//float* mainVolume = 0.5f;
};