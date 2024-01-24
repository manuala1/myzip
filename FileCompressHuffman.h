#pragma once
#include <string>
#include <vector>
#include "HuffmanTree.h"
#include "Common.h"
using std::string;
struct ByteInfo{
	uch _ch;
	long long _appearCount;
	string _chCode;
	ByteInfo(size_t appearCount=0) :_appearCount(appearCount) {

	}
	ByteInfo operator+(const ByteInfo& bi) {
		return ByteInfo(_appearCount + bi._appearCount);
	}
	bool operator>(const ByteInfo& bi) {
		return _appearCount > bi._appearCount;
	}
	bool operator==(const ByteInfo& bi) const{
		return _appearCount == bi._appearCount;
	}
	bool operator!=(const ByteInfo& bi) const{
		return _appearCount != bi._appearCount;
	}
};

class FileCompressHuffman {

public:
	void CompressFile(const string& filePath);
	void UnCompressFile(const string& filePath);
	FileCompressHuffman();
private:
	void GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root,string s="");
	void WriteHeadInfo(const string& filePath,FILE* fout);
	
private:
	string GetFileSuffix(const string& filePath);
	string GetFilePrefix(const string& filePath);
	std::vector<ByteInfo> _fileInfo;
};
void testCompress(); 
void testUnCompress();