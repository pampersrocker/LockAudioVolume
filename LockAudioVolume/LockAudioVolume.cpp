// LockAudioVolume.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>

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

	CoInitialize(nullptr);

	HRESULT hr;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&pEnumerator);

	if (hr != S_OK)
	{
		return 1;
	}

	IMMDevice* pDevice;
	hr = pEnumerator->GetDefaultAudioEndpoint(EDataFlow::eCapture, ERole::eCommunications, &pDevice);
	if (hr != S_OK)
	{
		return 1;
	}
	pEnumerator->Release();

	IAudioSessionManager* pManager;
	const IID IID_IMMAudioSessionManager = __uuidof(IAudioSessionManager);
	hr = pDevice->Activate(IID_IMMAudioSessionManager, CLSCTX_ALL, nullptr, (void**)&pManager);

	if (hr != S_OK)
	{
		return 1;
	}

	pDevice->Release();

	ISimpleAudioVolume* pSimpleAudioVolume;
	hr = pManager->GetSimpleAudioVolume(nullptr, true, &pSimpleAudioVolume);

	if (hr != S_OK)
	{
		return 1;
	}

	pManager->Release();


	float Volume = 0.0f;
	const float TargetVolume = 0.99f;

	while (true)
	{
		hr = pSimpleAudioVolume->GetMasterVolume(&Volume);
		if (Volume != TargetVolume)
		{
			pSimpleAudioVolume->SetMasterVolume(TargetVolume, nullptr);
		}
		Sleep(1000);
	}

	pSimpleAudioVolume->Release();

	return 0;
}