#pragma once

#include"Engine.h"

class CWaveSoundRead
	{
		public: WAVEFORMATEX* m_pwfx; // Pointer to WAVEFORMATEX structure 
				HMMIO m_hmmioIn; // MM I/O handle for the WAVE 
				MMCKINFO m_ckIn; // Multimedia RIFF chunk 
				MMCKINFO m_ckInRiff; // Use in opening a WAVE file 
		public: 
			CWaveSoundRead(); 
			~CWaveSoundRead();
			HRESULT Open( CHAR* strFilename ); 
			HRESULT Reset(); 
			HRESULT Read( UINT nSizeToRead, BYTE* pbData, UINT* pnSizeRead ); 
			HRESULT Close(); 
	};

namespace Batman {
	class cSound {
	private:
		CWaveSoundRead* m_pWaveSoundRead;
		LPDIRECTSOUNDBUFFER8 m_pBuffer;
		int m_bufferSize;

		static std::map< CWaveSoundRead*, int > m_waveMap;
		void Init();
	public: cSound(char* filename);
			cSound(cSound& in);
			cSound& operator=(const cSound &in);
			virtual ~cSound();
			void Restore();
			void Fill();
			void Play(bool bLoop = false);
			bool IsPlaying();
			void Stop();
			void setPosition(int pos);
	};

}
