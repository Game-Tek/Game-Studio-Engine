#pragma once

#include "ByteEngine/Handle.hpp"

#include "ByteEngine/Game/System.h"
#include <AAL/Platform/Windows/WindowsAudioDevice.h>
#include <GTSL/Array.hpp>
#include <GTSL/Buffer.hpp>
#include <GTSL/Math/Quaternion.h>

#include "ByteEngine/Id.h"
#include "ByteEngine/Game/Tasks.h"
#include "ByteEngine/Resources/AudioResourceManager.h"

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

	void BindAudio(AudioEmitterHandle audioEmitter, Id audioToPlay);
	void PlayAudio(AudioEmitterHandle audioEmitter);
	
	void SetPosition(AudioEmitterHandle audioEmitterHandle, const GTSL::Vector3 position) { audioEmittersLocation[audioEmitterHandle()] = position; }
	void SetPosition(AudioListenerHandle audioListenerHandle, const GTSL::Vector3 position) { audioListenersLocation[audioListenerHandle()] = position; }
	
	GTSL::Vector3 GetPosition(const AudioListenerHandle audioListenerHandle) const { return audioListenersLocation[audioListenerHandle()]; }
	GTSL::Vector3 GetPosition(const AudioEmitterHandle audioEmitterHandle) const { return audioEmittersLocation[audioEmitterHandle()]; }
	
	void SetOrientation(AudioListenerHandle audioListenerHandle, const GTSL::Quaternion orientation) { audioListenersOrientation[audioListenerHandle()] = orientation; }
	GTSL::Quaternion GetOrientation(AudioListenerHandle audioListenerHandle) const { return audioListenersOrientation[audioListenerHandle()]; }
	
	void SetAudioListener(const AudioListenerHandle audioListenerHandle) { activeAudioListenerHandle = audioListenerHandle; }

	void SetLooping(const AudioEmitterHandle audioEmitterHandle, bool loop) { audioEmittersSettings[audioEmitterHandle()].Loop = loop; }
	bool GetLooping(const AudioEmitterHandle audioEmitterHandle) { return audioEmittersSettings[audioEmitterHandle()].Loop; }
	
	MAKE_HANDLE(uint8, Channel);
	MAKE_HANDLE(uint8, SoundSource);

	ChannelHandle GetMasterChannel() const { return masterChannel; }

private:
	using AudioDevice = AAL::WindowsAudioDevice;
	
	AudioDevice audioDevice;
	AudioDevice::MixFormat mixFormat;

	struct SoundSource {
		byte* Data;
	};
	GTSL::Array<SoundSource, 16> soundSources;
	
	struct MixerChannel {
		struct Effect {
			
		};		
		GTSL::Array<Effect, 8> Effects;

		GTSL::Array<SoundSourceHandle, 16> SoundSources;
		
		float32 Volume = 0.0f;
	};
	GTSL::Array<MixerChannel, 16> channels;

	ChannelHandle masterChannel;
	
	GTSL::Array<uint8, 8> audioListeners;
	GTSL::Array<GTSL::Vector3, 8> audioListenersLocation;
	GTSL::Array<GTSL::Quaternion, 8> audioListenersOrientation;
	
	GTSL::Array<GTSL::Vector3, 8> audioEmittersLocation;

	MAKE_HANDLE(uint32, PrivateSound);
	
	struct AudioEmitterSettings
	{
		bool Loop = false;
		//PrivateSoundHandle PrivateSoundHandle;
		Id Name;
		uint32 Samples = 0;
	};
	GTSL::Array<AudioEmitterSettings, 8> audioEmittersSettings;
	
	GTSL::Array<AudioEmitterHandle, 8> playingEmitters;

	GTSL::Array<Id, 8> lastRequestedAudios;

	GTSL::Array<AudioEmitterHandle, 8> onHoldEmitters;
	
	GTSL::Buffer<BE::PAR> audioBuffer;
	DynamicTaskHandle<AudioResourceManager*, AudioResourceManager::AudioInfo> onAudioInfoLoadHandle;
	DynamicTaskHandle<AudioResourceManager*, AudioResourceManager::AudioInfo, GTSL::Range<const byte*>> onAudioLoadHandle;

	AudioListenerHandle activeAudioListenerHandle;

	GTSL::Vector<Id, BE::PAR> loadedSounds;

	template<typename T>
	auto getSample(byte* buffer, const uint8 channelCount, const uint32 sample, const uint32 channel) -> T&
	{
		return *(reinterpret_cast<T*>(buffer) + sample * channelCount + channel);
	};
	
	void requestAudioStreams();
	void render(TaskInfo);

	void removePlayingEmitter(uint32 i)
	{
		audioEmittersSettings[playingEmitters[i]()].Samples = 0;
		playingEmitters.Pop(i);
	}
	
	void onAudioInfoLoad(TaskInfo taskInfo, AudioResourceManager*, AudioResourceManager::AudioInfo audioInfo);
	void onAudioLoad(TaskInfo taskInfo, AudioResourceManager*, AudioResourceManager::AudioInfo audioInfo, GTSL::Range<const byte*> buffer);
};
