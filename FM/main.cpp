#include <iostream>
#include <fstream>
#include <iostream>
#include <vector>
#include <complex>
#include "waveheader.h"
#include <math.h>

void CreateWavFile(const char* filePath, std::vector<float>& vect , int vectorSize)
{

	WAV_HEADER wavHeader ;
	wavHeader.RIFF[0] = 'R' ; wavHeader.RIFF[1] = 'I' ; wavHeader.RIFF[2] = 'F' ; wavHeader.RIFF[3] = 'F' ;

	wavHeader.WAVE[0] = 'W'; wavHeader.WAVE[1] = 'A' ; wavHeader.WAVE[2] = 'V' ; wavHeader.WAVE[3] = 'E' ;

	wavHeader.fmt[0] = 'f' ;  wavHeader.fmt[1] = 'm' ;  wavHeader.fmt[2] = 't' ;  wavHeader.fmt[3] = ' ' ;

	wavHeader.Subchunk2ID[0] = 'd' ; wavHeader.Subchunk2ID[1] = 'a' ; wavHeader.Subchunk2ID[2] = 't' ; wavHeader.Subchunk2ID[3] = 'a' ;

	wavHeader.Subchunk1Size = 16 ; //объем данных равен размеру стандартного формата WAV (16 байт)
	wavHeader.AudioFormat = 1 ;    //1 (0x0001) 	PCM/uncompressed
	wavHeader.NumOfChan = 1 ;	   //1 - mono
	wavHeader.SamplesPerSec = 24000 ; // sample rate
	wavHeader.bitsPerSample = 32 ;     //8, 16, 24 или 32
	wavHeader.blockAlign = wavHeader.bitsPerSample / 8 * wavHeader.NumOfChan ; //BlockAlign = BitsPerSample / 8 * NumChannels
	wavHeader.bytesPerSec = wavHeader.SamplesPerSec * wavHeader.blockAlign ; //AvgBytesPerSec = SampleRate * BlockAlign
	wavHeader.Subchunk2Size = vectorSize * wavHeader.blockAlign  ;

	wavHeader.ChunkSize = sizeof(WAV_HEADER) - 8 + wavHeader.Subchunk2Size ;

	size_t headerSize = sizeof(WAV_HEADER);

	std::ofstream wavfile(filePath, std::ofstream::trunc) ;
	if (!wavfile) {
		std::cerr << "Can't open file" << std::endl ;
		exit(1) ;
	}
	wavfile.write( (char*)&wavHeader , headerSize ) ;
	int32_t data;
	for(auto elem : vect)
		{
		data = int32_t(elem) ;
		wavfile.write((char*) &data, sizeof (int32_t));
		}
	wavfile.close() ;
}
const float sample_rate = 24000.0 ;
int BUFF_SIZE = 1024*500;
std::vector<std::complex<int32_t>> signal ;


int main()
{

	/*float x[128];
	for (int i = 0; i < 128; i++)
		x[i] = i/43.34 ;
	std::ofstream fout("/home/karabas/CPP/AM2/AM/am/test", std::ios_base::trunc) ;
	for (int i = 0; i < 128; i++)
		fout.write((char*)&x[i], sizeof (float)) ;
	fout.close();
	*/

	std::ifstream fin("/home/karabas/CPP/FM/FM/fm_sound.dat") ;
	if (!fin) {
		std::cout << "Can't open file" ;
		exit(1) ;
	}
	//std::ofstream fout("/home/karabas/CPP/AM2/AM/am/test.txt", std::ios_base::trunc) ;
	int32_t x[2] ;
	for ( int i = 0 ; i < BUFF_SIZE ; i++ ){
		fin.read((char*) x , sizeof (int32_t) * 2) ;
		//fout << x [0] << '\n' << x[1] << '\n' ;
		signal.push_back(std::complex<int32_t>(x[0], x[1])) ;
	}

	std::vector<float>demod_signal(signal.size()-1) ;
	for (int i = 0; i < demod_signal.size(); i++){
		demod_signal[i] = ( (float(signal[i+1].imag()) - float(signal[i].imag()))*float(signal[i].real())
						 -(float(signal[i+1].real()) - float(signal[i].real()))*float(signal[i].imag()))
						 * sample_rate * (float(signal[i].imag())*float(signal[i].imag()) + float(signal[i].real())*float(signal[i].real()) ) ;
	}



	std::vector<float> data;
	data.resize(48000) ;
		for (int i=0; i<48000; i++)
			data[i] = 10000.0 * sin(2 * 3.14 * 1000 * i / 24000);
	CreateWavFile("/home/karabas/CPP/FM/FM/test.wav", data, data.size()) ;

	std::ifstream fin2("/home/karabas/CPP/FM/FM/test.wav") ;
	if (!fin2) {
		std::cout << "Can't open file" ;
		exit(1) ;
	}

	WAV_HEADER wavHeader ;
	fin2.read((char*) &wavHeader , sizeof(WAV_HEADER)) ;
	std::cout << "RIFF = " << wavHeader.RIFF[0] << wavHeader.RIFF[1]
			  <<wavHeader.RIFF[2]<<wavHeader.RIFF[3]<< std::endl ;
	std::cout << "wavHeader.Subchunk1Size(16) = "    << wavHeader.Subchunk1Size << std::endl ;
	std::cout << "wavHeader.AudioFormat(1) = "		 << wavHeader.AudioFormat << std::endl ;
	std::cout << "wavHeader.NumOfChan(1) = "		 << wavHeader.NumOfChan << std::endl ;
	std::cout << "wavHeader.SamplesPerSec(24000) = " << wavHeader.SamplesPerSec << std::endl ;
	std::cout << "wavHeader.bitsPerSample(32) = "	 << wavHeader.bitsPerSample << std::endl ;
	std::cout << "wavHeader.blockAlign(4) = "		 << wavHeader.blockAlign << std::endl ;
	std::cout << "wavHeader.bytesPerSec(24000*4) = " << wavHeader.bytesPerSec << std::endl ;
	std::cout << "wavHeader.Subchunk2Size() = "		 << wavHeader.Subchunk2Size << std::endl ;
	int32_t ampl ;
	for (int i=0; i<100; i++){
		fin2.read((char*) &ampl , sizeof(int32_t)) ;
		std::cout << "ampl[" << i << "] = " << ampl << std::endl ;
	}
	CreateWavFile("/home/karabas/CPP/FM/FM/demod.wav", demod_signal, demod_signal.size()) ;

	fin.close();

	return 0;
}
