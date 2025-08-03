///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//
// Compression.h header file
// 
// Author  :   Nikita Shulgin      :    ShulginNA@Lumex.ru
// Company :   Lumex Instruments   :    https://www.lumexinstruments.com/
//
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace PK_ {
	class Compression {
	public:
		static int DataCompression(int* Dst, int* Src, int DstLen, int SrcLen);
		static int DataDecompression(int** Dst, int* Src);
		static int ZeroSupression(int* Dest, int* Src, int DstLen, int SrcLen, int Noiselevel);
		static int ZeroDepression(int* Dest, int* Src, int DstLen, int SrcLen);
	private:
	};
}