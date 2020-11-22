#pragma once

#include "ByteEngine/Handle.hpp"

#include "ByteEngine/Game/System.h"
#include <AAL/Platform/Windows/WindowsAudioDevice.h>
#include <GTSL/Array.hpp>
#include <GTSL/Buffer.h>
#include "ByteEngine/Id.h"

class Sound;

MAKE_HANDLE(uint32, AudioListener)
MAKE_HANDLE(uint32, AudioEmitter)

class AudioSystem : public System
{
public:
	AudioSystem();
	~AudioSystem();
	
	void Initialize(const InitializeInfo& initializeInfo) override;
	void Shutdown(const ShutdownInfo& shutdownInfo) override;

	AudioListenerHandle CreateAudioListener();
	AudioEmitterHandle CreateAudioEmitter();

	void PlayAudio(AudioEmitterHandle audioEmitter, Id audioToPlay);
	
private:
	using AudioDevice = AAL::WindowsAudioDevice;
	
	AudioDevice audioDevice;

	GTSL::Array<uint8, 8> audioListeners;
	GTSL::Array<uint8, 8> audioEmitters;

	GTSL::Array<AudioEmitterHandle, 8> playingEmitters;
	GTSL::Array<uint32, 8> playingAudioFilesPlayedSamples;
	GTSL::Array<Id, 8> playingAudioFiles;

	GTSL::Array<Id, 8> lastRequestedAudios;

	GTSL::Buffer audioBuffer;
	
	void requestAudioStreams();
	void render();

	void removePlayingSound(uint32 i)
	{
		playingEmitters.Pop(i); playingAudioFilesPlayedSamples.Pop(i); playingAudioFiles.Pop(i);
	}
};
