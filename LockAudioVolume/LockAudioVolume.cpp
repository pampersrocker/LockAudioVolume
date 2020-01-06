// LockAudioVolume.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>


// The target Volume in the range [0..1] we like to set the microphone to.
const float TargetVolume = 0.80f;

int WINAPI WinMain(
	HINSTANCE hInstance,     /* [input] handle to current instance */
	HINSTANCE hPrevInstance, /* [input] handle to previous instance */
	LPSTR lpCmdLine,         /* [input] pointer to command line */
	int nCmdShow             /* [input] show state of window */
)
{
	const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
	IMMDeviceEnumerator* pEnumerator;

	// Initialize the com library
	CoInitialize(nullptr);

	while (true)
	{
		HRESULT hr;

		// Create a device iterator, while we don't require the iterator itself,
		// we can retrieve the default devices from it.
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&pEnumerator);

		if (FAILED(hr))
		{
			return 1;
		}

		IMMDevice* pDevice;
		// Retrieve the default capturing communication device (the Microphone which is set as Default Communications device)
		hr = pEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &pDevice);
		if (FAILED(hr))
		{
			return 1;
		}
		pEnumerator->Release();

		IAudioSessionManager* pManager;
		const IID IID_IMMAudioSessionManager = __uuidof(IAudioSessionManager);
		// Retrieve the AudioSessionManager interface from the device
		hr = pDevice->Activate(IID_IMMAudioSessionManager, CLSCTX_ALL, nullptr, (void**)&pManager);

		if (FAILED(hr))
		{
			return 1;
		}

		pDevice->Release();

		ISimpleAudioVolume* pSimpleAudioVolume;
		// Retrieve the SimpleAudioVolume interface from the AudioSessionManager
		hr = pManager->GetSimpleAudioVolume(nullptr, true, &pSimpleAudioVolume);

		if (FAILED(hr))
		{
			return 1;
		}

		pManager->Release();


		float Volume = 0.0f;

		int Iterations = 0;

		// Re-fetch the audio device every 10 seconds, in case it changed.
		while (Iterations++ < 10)
		{
			// Fetch the current volume
			hr = pSimpleAudioVolume->GetMasterVolume(&Volume);
			if (FAILED(hr))
			{
				break;
			}
			// If the volume has changed, reset it to our target volume
			if (Volume != TargetVolume)
			{
				// Apply our target volume
				hr = pSimpleAudioVolume->SetMasterVolume(TargetVolume, nullptr);
				if (FAILED(hr))
				{
					break;
				}
			}
			Sleep(1000);
		}

		pSimpleAudioVolume->Release();

		// Wait one second before trying to reinitialize again.
		Sleep(1000);
	}

	return 0;
}