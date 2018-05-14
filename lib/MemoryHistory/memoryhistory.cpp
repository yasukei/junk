#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <new>

#define BYTE uint8_t

class MemoryRecord
{
public:
	MemoryRecord(
		BYTE* addressToMemory,
		size_t sizeofMemory
		) :
		addressToMemory_(addressToMemory),
		sizeofMemory_(sizeofMemory)
	{
		bufferForRecord_ = (BYTE*)malloc(sizeofMemory);
	}

	~MemoryRecord(void)
	{
		free(bufferForRecord_);
	}

	void record(void)
	{
		memcpy(bufferForRecord_, addressToMemory_, sizeofMemory_);
	}

	void dump(
		FILE* fp
		)
	{
		uint32_t bytePerLine = 8;

		for(uint32_t index = 0; index < sizeofMemory_; index++)
		{
			if((index % bytePerLine) == 0)
			{
				fprintf(fp, "0x%08X:", index);
			}

			fprintf(fp, " %02X", *(uint8_t*)&bufferForRecord_[index]);

			if((index % bytePerLine) == (bytePerLine - 1))
			{
				fprintf(fp, "\n");
			}
		}
	}

private:
	BYTE* addressToMemory_;
	size_t sizeofMemory_;
	BYTE* bufferForRecord_;

	MemoryRecord(void) { }
};

class MemoryHistory
{
public:
	typedef enum
	{
		eMemoryHistoryMode_1shot,
		eMemoryHistoryMode_circular,
	}eMemoryHistoryMode;

	MemoryHistory(
		BYTE* addressToMemory,
		size_t sizeofMemory,
		uint32_t numofHistory,
		int historyMode
		) :
		numofHistory_(numofHistory),
		historyMode_(historyMode),
		isRecording_(false),
		oldestIndex_(0)
	{
		memoryRecords_ = (MemoryRecord*)malloc(sizeof(MemoryRecord) * numofHistory);
		for(size_t index = 0; index < numofHistory_; index++)
		{
			new(&memoryRecords_[index]) MemoryRecord(addressToMemory, sizeofMemory);
		}

	}

	~MemoryHistory(void)
	{
		for(size_t index = 0; index < numofHistory_; index++)
		{
			memoryRecords_[index].~MemoryRecord();
		}
		free(memoryRecords_);
	}

	void startRecording(void)	{ isRecording_ = true; }
	void stopRecording(void)	{ isRecording_ = false; }
	void record(void)
	{
		if(! isRecording_)
		{
			return;
		}

		if(historyMode_ == eMemoryHistoryMode_circular)
		{
			memoryRecords_[oldestIndex_].record();
			oldestIndex_ += 1;

			if(oldestIndex_ == numofHistory_)
			{
				oldestIndex_ = 0;
			}
		}
		else // historyMode_ == eMemoryHistoryMode_1shot
		{
			if(oldestIndex_ != numofHistory_)
			{
				memoryRecords_[oldestIndex_].record();
				oldestIndex_ += 1;
			}
		}
	}

	bool dumpOldestToLatest(
		const char* filepath
		)
	{
		FILE* fp;

		fp = fopen(filepath, "w");
		if(fp == NULL)
		{
			return false;
		}

		if(historyMode_ == eMemoryHistoryMode_circular)
		{
			uint32_t index = oldestIndex_;
			fprintf(fp, "[%dth record]\n", index < oldestIndex_ ? numofHistory_ - oldestIndex_ + index : index - oldestIndex_);
			memoryRecords_[index].dump(fp);
			fprintf(fp, "\n\n");
			index += 1;
			while(index != oldestIndex_)
			{
				if(index == numofHistory_)
				{
					index = 0;
				}
				fprintf(fp, "[%dth record]\n", index < oldestIndex_ ? numofHistory_ - oldestIndex_ + index : index - oldestIndex_);
				memoryRecords_[index].dump(fp);
				fprintf(fp, "\n\n");

				index += 1;
			}
		}
		else // historyMode_ == eMemoryHistoryMode_1shot
		{
			for(uint32_t index = 0; index < numofHistory_; index++)
			{
				fprintf(fp, "[%dth record]\n", index);
				memoryRecords_[index].dump(fp);
				fprintf(fp, "\n\n");
			}
		}

		return true;
	}

private:
	MemoryHistory(void);

	uint32_t numofHistory_;
	int historyMode_;

	bool isRecording_;
	uint32_t oldestIndex_;
	MemoryRecord* memoryRecords_;
};

int main(void)
{
	size_t numofHistory = 5;
	BYTE memory[32];
	MemoryHistory memoryHistosy(
		memory,
		sizeof(memory),
		numofHistory,
		//MemoryHistory::eMemoryHistoryMode_1shot
		MemoryHistory::eMemoryHistoryMode_circular
		);

	printf("hello\n");

	memoryHistosy.startRecording();
	for(size_t index = 0; index < numofHistory; index++)
	{
		memory[index] = index;
		memoryHistosy.record();
	}
	memoryHistosy.stopRecording();
	memoryHistosy.dumpOldestToLatest("aaa");

	return 0;
}
