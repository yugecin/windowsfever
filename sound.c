// from https://stackoverflow.com/q/52056127

struct {
    HWAVEOUT hWaveout;
    WAVEFORMATEX wavFormat;
    WAVEHDR wavHdr;
    char *raw;
    DWORD rawSize;
} sound;

#define MAXRAWSIZE 2000000

void sound_init()
{
	HANDLE hFile;

	sound.wavFormat.cbSize = 0;
	sound.wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	sound.wavFormat.nChannels = 1;
	sound.wavFormat.wBitsPerSample = 16;
	sound.wavFormat.nSamplesPerSec = 44100;
	sound.wavFormat.nBlockAlign = sound.wavFormat.nChannels * sound.wavFormat.wBitsPerSample / 8;
	sound.wavFormat.nAvgBytesPerSec = sound.wavFormat.nSamplesPerSec * sound.wavFormat.nBlockAlign;

	sound.raw = HeapAlloc(GetProcessHeap(), 0, MAXRAWSIZE); 
	hFile = CreateFile("shrug2point0.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBoxA(NULL, "CreateFile err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
	if (!ReadFile(hFile, sound.raw, MAXRAWSIZE, &sound.rawSize, NULL)) {
		MessageBoxA(NULL, "ReadFile err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
	CloseHandle(hFile);

	if (sound.rawSize == MAXRAWSIZE) {
		MessageBoxA(NULL, "sound file doesn't fit in allocated mem", DEMONAME, MB_OK);
		ExitProcess(0);
	}

	if (waveOutOpen(&sound.hWaveout, WAVE_MAPPER, &sound.wavFormat, (DWORD_PTR) NULL, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
		MessageBoxA(NULL, "waveOutOpen err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
	sound.wavHdr.lpData = sound.raw;
	sound.wavHdr.dwBufferLength = sound.rawSize;
	sound.wavHdr.dwFlags = 0;
	sound.wavHdr.dwLoops = 0;
}

void sound_play()
{
	if (waveOutPrepareHeader(sound.hWaveout, &sound.wavHdr, sizeof(sound.wavHdr)) != MMSYSERR_NOERROR) {
		MessageBoxA(NULL, "waveOutPrepareHeader err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
	if (waveOutWrite(sound.hWaveout, &sound.wavHdr, sizeof(sound.wavHdr))) {
		MessageBoxA(NULL, "waveOutWrite err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
}
