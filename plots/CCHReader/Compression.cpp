#include "pch.h"
#include "Compression.h"
namespace PK_ {

	//используются коды Фибоначчи с добавками
	//Переменные, которые используются как глобальные в пределах модуля
	//и несут на себе значения нужные для процесса упаковки
	int intOffset_PK, bitOffset_PK;
	unsigned int* Dest;
	unsigned int* Source;
	int MaxCodeLen;
	//добавляет код и само слово в сжатый буфер
	const int Mask = 0xFFFFFFFE;

	//Варианты таблиц кодирования нечетные - значения, четные - битность
	//самая простая - прямое кодирование
	int CodeTable[64];

	int CodeTableFlat[] =
	{ 0,5,  1,5,  2,5,  3,5,
	 4,5,  5,5,  6,5,  7,5,  8,5,
	 9,5, 10,5, 11,5, 12,5,
	13,5, 14,5, 15,5, 16,5,
	17,5, 18,5, 19,5, 20,5,
	21,5, 22,5, 23,5, 24,5,
	25,5, 26,5, 27,5, 28,5,
	29,5, 30,5, 31,5 };
	//на 5 значений
	int CodeTable5[] =
	{ 0,1,  4,3,  5,3,  6,3, 7,3 };
	//на 7 значений
	int CodeTable7[] =
	{ 0,1,  4,3,  5,3,  12,4, 13,4, 14,4, 15,4 };
	//на 9 значений
	int CodeTable9[] =
	{ 0,1,  4,3,  5,3,  12,4, 13,4, 28,5, 29,5, 30,5, 31,5 };
	//на 11 значений
	int CodeTable11[] =
	{ 0,1,  4,3,  5,3,  24,5, 25,5, 26,5, 27,5, 28,5, 29,5, 30,5, 31,5 };
	//на 15 значений
	int CodeTable15[] =
	{ 0,1,  4,3,  5,3, 24,5,
	25,5, 26,5, 27,5, 56,6,
	57,6, 58,6, 59,6, 60,6,
	61,6, 62,6, 63,6 };
	//на 32 значения
	int CodeTable32[] =
	{ 0,1,   4,3,   5,3,  24,5,
	 25,5,  26,5,  27,5,  56,6,
	 57,6,  58,6,  59,6, 240,8,
	241,8, 242,8, 243,8, 244,8,
	245,8, 246,8, 247,8, 496,9,
	497,9, 498,9, 499,9, 500,9,
	501,9, 502,9, 503,9, 504,9,
	505,9, 506,9, 507,9, 508,9
	};

	int* DecodeTable;

	inline void AddWord(int Word, int Len) {
		if (Len <= bitOffset_PK) {
			//Если в том же целом числе
			bitOffset_PK -= Len;
			Dest[intOffset_PK] |= Word << bitOffset_PK;
		}
		else {
			//если переходит на следующий int
			Dest[intOffset_PK] |= Word >> (Len - bitOffset_PK);
			intOffset_PK++;
			bitOffset_PK = 32 - (Len - bitOffset_PK);
			Dest[intOffset_PK] = Word << bitOffset_PK;
		}
	}



	inline void AddSeq(int Word) {
		int Len, Buf = Word;
		for (Len = 0; Buf > 0; Len++, Buf = Buf >> 1);
		//Первую единицу слова можно удалить поскольку ее
		//однозначно определяет код длины
		if (Word > 1) {
			//если не ноль
			Word &= (Mask << (Len - 1)) | (0xFFFFFFFF >> (33 - Len));
			//добавляем код
			AddWord(CodeTable[Len << 1], CodeTable[(Len << 1) + 1]);
			//Добавляем слово
			AddWord(Word, Len - 1);
		}
		else {
			if (Word) {
				//если единица
				AddWord(CodeTable[2], CodeTable[3]);
			}
			else {
				//если ноль
				AddWord(CodeTable[0], CodeTable[1]);
			}
		}
	}

	//извлекает слово длиной Length из битовой последовательности
	inline int GetWord(int Length) {
		if (Length == 0) return 0;
		if (Length <= bitOffset_PK) {
			//чистка старших разрядов
			return (Source[intOffset_PK] << (32 - bitOffset_PK))
				//чистка младших разрядов
				>> (32 - Length);
		}
		else {
			return ((Source[intOffset_PK] << (32 - bitOffset_PK)) >>
				(32 - Length)) |
				(Source[intOffset_PK + 1] >> (32 - (Length - bitOffset_PK)));
		}
	}

	//сдвигает битовый указатель на Offs позиций offs<32
	inline void MovePointer(int Offs) {
		if (bitOffset_PK > Offs) {
			bitOffset_PK -= Offs;
		}
		else {
			Offs -= bitOffset_PK;
			intOffset_PK++;
			bitOffset_PK = 32 - Offs;
		}
	}

	//функция извлекает из упаковки очередное слово и возвращает его как результат работы
	inline int GetSeq()
	{
		//берем из битовой последовательности бит по максимальной длине кода
		int Prefix = GetWord(MaxCodeLen);
		//вытаскиваем из таблицы декодирования - фактичесекую длину кода и слова
		//передвигаем битовый указатель на длину кода
		MovePointer(DecodeTable[Prefix << 1]);
		//разбираемся с нулями
		if (DecodeTable[(Prefix << 1) + 1] == 0) return 0;
		//извлекаем слово
		int Word = GetWord(DecodeTable[(Prefix << 1) + 1] - 1);
		//передвигаем битовый указатель на слово
		MovePointer(DecodeTable[(Prefix << 1) + 1] - 1);
		//Добавляем единицу старшего разряда (можно поставить += но не знаю что будет эффективнее)
		Word |= 1 << (DecodeTable[(Prefix << 1) + 1] - 1);
		return Word;
	}

	//таблица декодирования имеет следующий формат:
	//четные - длинна кода, нечетные - длинна слова за этим кодом

	int MakeDecodingTable(int* CodeTable, int** DecodeTable, int Length) {
		//Выясняем максимальную длинну слова
		int Max = MaxCodeLen;
		//создаем таблицу потребного размера
		(*DecodeTable) = new int[1 << (Max + 1)];

		for (int i = 0; i < Length; i++) {
			if (!CodeTable[(i << 1) + 1]) {
				continue;
			}
			//рассчитываем начало фрагмента кодовой таблицы от i - го кода
			int Offs = (CodeTable[i << 1] << (Max - CodeTable[(i << 1) + 1]));
			//Для каждого слова заполняем его фрагмегнт таблицы декодирования
			int debug = (1 << (Max - CodeTable[(i << 1) + 1]));
			for (int j = 0; j < (1 << (Max - CodeTable[(i << 1) + 1])); j++) {
				//помещаем длину кода
				(*DecodeTable)[(j + Offs) << 1] = CodeTable[(i << 1) + 1];
				//помещаем длину слова
				(*DecodeTable)[((j + Offs) << 1) + 1] = i;
			}
		}

		return 0;
	}

	int Compression::DataCompression(int* Dst, int* Src, int DstLen, int SrcLen)
	{
		//Определяем длинны кодов
		//заводим массив хранения длинн
	 //   int *Lens = new int[SrcLen];
		int Len, Sr, i;
		int Stat[32];
		//ZeroMemory(Stat, 128);
		memset(Stat, 0, 128);
		//ZeroMemory(CodeTable, 256);
		memset(CodeTable, 0, 256);
		//Цикл определения длинн
		for (i = 0; i < SrcLen; i++) {
			Len = 0;
			for (Sr = Src[i]; Sr > 0; Len++) Sr = Sr >> 1;
			//      Lens[i] = Len;
			Stat[Len]++;
		}
		//Здесь можно вывести таблицу распределения длинн

		//формирование префиксной зоны
		//Считаем число не нулевых значений
		int Count = 0;
		for (i = 0; i < 32; i++) {
			if (Stat[i] != 0) Count++;
		}

		int* WorkCodeTable;
		//Подбираем исходную таблицу
		if (Count <= 5) { WorkCodeTable = CodeTable5; MaxCodeLen = 3; }
		else
			if (Count <= 7) { WorkCodeTable = CodeTable7; MaxCodeLen = 4; }
			else
				if (Count <= 9) { WorkCodeTable = CodeTable9; MaxCodeLen = 5; }
				else
					if (Count <= 11) { WorkCodeTable = CodeTable11; MaxCodeLen = 5; }
					else
						if (Count <= 15) { WorkCodeTable = CodeTable15; MaxCodeLen = 6; }
						else { WorkCodeTable = CodeTable32; MaxCodeLen = 9; }
		//   WorkCodeTable=CodeTableFlat;

		   //по статистике формируем нужную кодовую таблицу
		//ZeroMemory(CodeTable, 64);
		memset(CodeTable, 0, 64);
		int Max;
		//нули всегда нули
		CodeTable[0] = WorkCodeTable[0];
		CodeTable[1] = WorkCodeTable[1];
		for (i = 1; i < Count; i++) {
			Max = 1;
			for (int j = 1; j < 32; j++) {
				if (Stat[j] >= Stat[Max]) Max = j;
			}
			CodeTable[(Max << 1)] = WorkCodeTable[i << 1];
			CodeTable[(Max << 1) + 1] = WorkCodeTable[(i << 1) + 1];
			Stat[Max] = 0;
		}
		//для Flat-модели без статистики
	 /*   CopyMemory(CodeTable,CodeTableFlat,256);
		int Count = 32;*/
		//инициируем структуры потока
		Dst[0] = SrcLen;
		bitOffset_PK = 32;
		intOffset_PK = 1;
		Dest = (unsigned int*)Dst;
		Source = (unsigned int*)Src;
		//Записываем кодовую таблицу в поток
		AddWord(MaxCodeLen, 4);
		AddWord(Count, 5);
		AddWord(CodeTable[0], MaxCodeLen);
		AddWord(CodeTable[1], 4);
		for (i = 2; i < 64; i += 2) {
			if (CodeTable[i]) {
				AddWord(i >> 1, 5);
				AddWord(CodeTable[i], MaxCodeLen);
				AddWord(CodeTable[i + 1], 4);
			}
		}

		//собственно цикл упаковки
		for (i = 0; i < SrcLen; i++) {
			AddSeq(Source[i]);
		}
		return intOffset_PK;
	}

	//Обеспечить запас при декомпрессии в 1 int чтобы не допустить access violation
	int Compression::DataDecompression(int** Dst, int* Src)
	{
		int i;
		(*Dst) = new int[Src[0] + 1];
		Dest = (unsigned int*)(*Dst);
		Source = (unsigned int*)Src;
		bitOffset_PK = 32;
		intOffset_PK = 1;
		//Читаем кодовую таблицу
		//ZeroMemory(CodeTable, 256);
		memset(CodeTable, 0, 256);
		MaxCodeLen = GetWord(4);
		MovePointer(4);
		int Count = GetWord(5);
		MovePointer(5);
		CodeTable[0] = GetWord(MaxCodeLen);
		MovePointer(MaxCodeLen);
		CodeTable[1] = GetWord(4);
		MovePointer(4);
		for (i = 1; i < Count; i++) {
			int Index = GetWord(5);
			MovePointer(5);
			CodeTable[Index << 1] = GetWord(MaxCodeLen);
			MovePointer(MaxCodeLen);
			CodeTable[(Index << 1) + 1] = GetWord(4);
			MovePointer(4);
		}

		MakeDecodingTable(CodeTable, &DecodeTable, 32);

		for (i = 0; i < Src[0]; i++) {
			Dest[i] = GetSeq();
		}
		//ERR4
		delete[] DecodeTable;
		return Src[0];
	}

	// В принципе в качестве Dest и Src может быть
	// использован один и тот же буфер памяти
	// Возврат - получившийся объем
	int Compression::ZeroSupression(int* Dest, int* Src, int DstLen, int SrcLen, int Noiselevel)
	{
		int DCount = 0, ZCount = 0;
		for (int i = 0; i < SrcLen; i++) {
			if (Src[i] > Noiselevel) {
				if (ZCount > 0) {
					Dest[DCount] = 0;
					DCount++;
					Dest[DCount] = ZCount;
					DCount++;
					ZCount = 0;
				}
				Dest[DCount] = Src[i];
				DCount++;
			}
			else {
				ZCount++;
			}
		}
		return DCount;
	}

	// Здесь буферы Dest и Src должны быть разными
	// Возврат - число сжатых символов в Src
	// если он больше чем размер Dest то информация
	// расшифрована неполностью

	int Compression::ZeroDepression(int* Dest, int* Src, int DstLen, int SrcLen)
	{
		int DstCount = 0;
		memset(Dest, 0, DstLen * sizeof(int));
		for (int i = 0; (i < SrcLen) && (DstCount < DstLen); i++) {
			if (Src[i] == 0) {
				i++;
				DstCount += Src[i];
			}
			else {
				Dest[DstCount] = Src[i];
				DstCount++;
			}
		}
		return 0;
	}

}