// from https://stackoverflow.com/q/52056127

struct {
    HWAVEOUT hWaveout;
    WAVEFORMATEX wavFormat;
    WAVEHDR wavHdr;
    char *raw;
    DWORD rawSize;
    DWORD totalSamples;
    int startSample;
    int isPlaying;
} sound;

#define MAXRAWSIZE 5000000
#define SAMPLES_PER_SEC 44100
#define BYTES_PER_SAMPLE 2
#define CHANNELS 1
#define BYTES_PER_SEC (SAMPLES_PER_SEC * BYTES_PER_SAMPLE * CHANNELS)

void sound_init()
{
	HANDLE hFile;

	sound.wavFormat.cbSize = 0;
	sound.wavFormat.wFormatTag = WAVE_FORMAT_PCM;
	sound.wavFormat.nChannels = CHANNELS;
	sound.wavFormat.wBitsPerSample = BYTES_PER_SAMPLE * 8;
	sound.wavFormat.nSamplesPerSec = SAMPLES_PER_SEC;
	sound.wavFormat.nBlockAlign = BYTES_PER_SAMPLE * CHANNELS;
	sound.wavFormat.nAvgBytesPerSec = BYTES_PER_SEC;
	sound.isPlaying = 0;

	sound.raw = HeapAlloc(GetProcessHeap(), 0, MAXRAWSIZE); 
	hFile = CreateFile("plucks2.raw", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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

	sound.totalSamples = sound.rawSize / CHANNELS / BYTES_PER_SAMPLE;

	if (waveOutOpen(&sound.hWaveout, WAVE_MAPPER, &sound.wavFormat, (DWORD_PTR) NULL, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
		MessageBoxA(NULL, "waveOutOpen err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
}

void sound_play(int startSample)
{
	DWORD offset;

	sound.isPlaying = 0;
	waveOutReset(sound.hWaveout);
	offset = startSample * BYTES_PER_SAMPLE * CHANNELS;
	if (offset >= sound.rawSize) {
		return;
	}
	sound.startSample = startSample;
	sound.wavHdr.lpData = sound.raw + offset;
	sound.wavHdr.dwBufferLength = sound.rawSize - offset;
	sound.wavHdr.dwFlags = 0;
	sound.wavHdr.dwLoops = 0;
	if (waveOutPrepareHeader(sound.hWaveout, &sound.wavHdr, sizeof(sound.wavHdr)) != MMSYSERR_NOERROR) {
		MessageBoxA(NULL, "waveOutPrepareHeader err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
	// because parties are loud?
	waveOutSetVolume(sound.hWaveout, 0xCCCCCCCC);
	if (waveOutWrite(sound.hWaveout, &sound.wavHdr, sizeof(sound.wavHdr))) {
		MessageBoxA(NULL, "waveOutWrite err", DEMONAME, MB_OK);
		ExitProcess(0);
	}
	sound.isPlaying = 1;
}

void sound_seek_relative_seconds(int seek_seconds)
{
	MMTIME mmtime;
	DWORD newsample;

	if (!sound.isPlaying) {
		sound_play(0);
		return;
	}
	mmtime.wType = TIME_SAMPLES; // TIME_MS seems to not work as I want (too high values)
	waveOutGetPosition(sound.hWaveout, &mmtime, sizeof(mmtime));
	newsample = mmtime.u.sample + sound.startSample + seek_seconds * BYTES_PER_SEC;
	if (newsample < sound.totalSamples) {
		sound_play(newsample < 0 ? 0 : newsample);
	}
}

#define SOUND_END -1
int sound_get_pos_ms()
{
	MMTIME mmtime;

	if (!sound.isPlaying) {
		return (int) (sound.totalSamples * 1000.0f / SAMPLES_PER_SEC);
	}
	mmtime.wType = TIME_SAMPLES; // TIME_MS seems to not work as I want (too high values)
	waveOutGetPosition(sound.hWaveout, &mmtime, sizeof(mmtime));
	if (mmtime.u.sample >= sound.totalSamples) {
		return SOUND_END;
	}
	return (int) ((mmtime.u.sample + sound.startSample) * 1000.0f / SAMPLES_PER_SEC);
}
