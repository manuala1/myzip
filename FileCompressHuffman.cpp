#define  _CRT_SECURE_NO_WARNINGS 1
#include "FileCompressHuffman.h"
#include "HuffmanTree.h"

#include "Common.h"


FileCompressHuffman::FileCompressHuffman() {
	_fileInfo.resize(256);
	for (int i = 0; i < 256; i++) {
		_fileInfo[i]._ch = i;
	}
}

void FileCompressHuffman::WriteHeadInfo(const string& filePath, FILE* fout) {
	//1.获取源文件后缀
	string headInfo;
headInfo += GetFileSuffix(filePath);
headInfo += '\n';
//2.获取频次信息
size_t appearLineCount = 0;
string chInfo;
for (auto& e : _fileInfo) {
	if (e._appearCount) {
		appearLineCount++;
		chInfo += e._ch;
		chInfo += ':';
		chInfo += std::to_string(e._appearCount);
		chInfo += '\n';
	}
}
headInfo += std::to_string(appearLineCount);
headInfo += '\n';
fwrite(headInfo.c_str(), 1, headInfo.size(), fout);
fwrite(chInfo.c_str(), 1, chInfo.size(), fout);


}
string FileCompressHuffman::GetFileSuffix(const string& filePath) {
	size_t pos = filePath.find_last_of('.') + 1;
	return filePath.substr(pos);
}

void FileCompressHuffman::CompressFile(const string& filePath) {
	//1.统计源文件每个字符出现的次数
	FILE* fd = fopen(filePath.c_str(), "rb"); //必须用二进制形势读写 ，如果使用文本文件 遇到FF即-1即EOF就停止读写
	if (fd == nullptr) {
		perror("带压缩文件不存在");
		return;
	}
	uch ch[1024];
	int sz = 0;
	//fgetc()
	while ((sz = fread(&ch, 1, 1024, fd))) {
		for (int i = 0; i < sz; i++) {
			uch ch1 = ch[i];
    		_fileInfo[ch1]._appearCount++;  //难点2 汉字问题 下标为负数
		}
		//_fileInfo[ch]._appearCount++;
	}
	//fclose(fd);
	//2.用统计的结果建立哈夫曼树
	ByteInfo invalid;
	HuffmanTree<ByteInfo> ht(_fileInfo, invalid);
	//3.生成哈弗曼编码
	GenerateHuffmanCode(ht.getRoot());
	//cout << 1 << endl;
	// 
	// 
	//4. 写用来解压缩的数据信息
	string prefix=GetFilePrefix(filePath);
	string CompressFile = prefix + ".hz";
	FILE* fout = fopen(CompressFile.c_str(), "wb");
	WriteHeadInfo(filePath, fout);
	//5.用获得的编码来对源文件进行改写

	fseek(fd, 0, SEEK_SET);
	uch bits = 0;
	int bitCount = 0;
	while ((sz = fread(&ch, 1, 1024, fd))) {
		for (int i = 0; i < sz; i++) {
			string& strCode = _fileInfo[ch[i]]._chCode;
			for (int j = 0; j < strCode.size(); j++) {
				bits <<= 1;
				if (strCode[j] == '1') {
					bits |= 1;
				}
				if (++bitCount == 8) {
					fputc(bits, fout);
					bits = 0;
					bitCount = 0;
				}
			}

		}
		//_fileInfo[ch]._appearCount++;
	}
	if (bitCount > 0) {
		bits <<= (8 - bitCount);
		fputc(bits, fout);
	}
	cout << "压缩成功：" << CompressFile << endl;
	fclose(fd);
	fclose(fout);
}
void FileCompressHuffman::GenerateHuffmanCode(HuffmanTreeNode<ByteInfo>* root, string s) {
	if (root == nullptr)
		return;
	if (root->_left == nullptr && root->_right == nullptr) {
		_fileInfo[root->_weight._ch]._chCode = s;
	}
	GenerateHuffmanCode(root->_left, s + "0");
	GenerateHuffmanCode(root->_right, s + "1");
}
void FileCompressHuffman::UnCompressFile(const string& filePath) {

	if (GetFileSuffix(filePath) != "hz") {
		perror("后缀不匹配");
		return;
	}
	//1.读取解压缩需要的信息
	FILE* fin = fopen(filePath.c_str(), "rb");
	if(fin == nullptr){
		perror("待压缩文件不存在");
		return;
    }
	//获取源文件前缀 --- manu1.hz -> manu1
	string unCompressFile = GetFilePrefix(filePath);
	unCompressFile += "new.";
	char buf[1024];
	//读取源文件后缀 是从源文件内容中读取的
	fgets(buf, sizeof(buf) , fin);
	unCompressFile += buf;
	unCompressFile.pop_back();
	//读取频次信息个数
	fgets(buf, sizeof(buf), fin);
	int count = atoi(buf);
	
	for (int i = 0; i < count; i++) {
		fgets(buf, sizeof(buf), fin);
		if (buf[0] == '\n') { //特殊处理换行 （难点）
			fgets(buf, sizeof(buf), fin);
			_fileInfo['\n']._appearCount = std::atoi(buf + 1);
		}
		uch tmp = buf[0];
		_fileInfo[tmp]._appearCount = std::atoi(buf+2);
	}
	//2.还原哈夫曼树
	HuffmanTree<ByteInfo> ht(_fileInfo, ByteInfo());
	FILE* fout=fopen(unCompressFile.c_str(), "wb");
	int cnt = ht.getRoot()->_weight._appearCount;
	int fileSize = 0;
	//3.解压缩
	char rdbuff[1024];
	int sz = 0;
	HuffmanTreeNode<ByteInfo>* root = ht.getRoot();
	HuffmanTreeNode<ByteInfo>* cur = root;
	while (sz = fread(rdbuff,1,1024,fin)) {
		for (int i = 0; i < sz; i++) {
			char ch = rdbuff[i];
			for (int j = 0; j < 8; j++) {
				if (cur->_left == nullptr && cur->_right == nullptr) {
					fputc(cur->_weight._ch,fout);
					if (++fileSize == cnt) {
						break;
					}
					cur = root;
				}
				if ((ch & 0x80) == 0x80) {
					cur = cur->_right;	
				}
				else {
					cur = cur->_left;
				}
				ch <<= 1;
			}
		}
	}
	fclose(fin);
	fclose(fout);
	cout <<"解压缩成功:"<< unCompressFile << endl;
}

string FileCompressHuffman::GetFilePrefix(const string& filePath) {
	size_t pos = filePath.find_last_of('.');
	return filePath.substr(0, pos);
}

void testCompress() {
	FileCompressHuffman fchm;
	fchm.CompressFile("manu.txt");
}

void testUnCompress() {
	FileCompressHuffman fchm;
	//fchm.CompressFile("manu.txt");
	fchm.UnCompressFile("manu1.hz");
}