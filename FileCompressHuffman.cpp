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
	//1.��ȡԴ�ļ���׺
	string headInfo;
headInfo += GetFileSuffix(filePath);
headInfo += '\n';
//2.��ȡƵ����Ϣ
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
	//1.ͳ��Դ�ļ�ÿ���ַ����ֵĴ���
	FILE* fd = fopen(filePath.c_str(), "rb"); //�����ö��������ƶ�д �����ʹ���ı��ļ� ����FF��-1��EOF��ֹͣ��д
	if (fd == nullptr) {
		perror("��ѹ���ļ�������");
		return;
	}
	uch ch[1024];
	int sz = 0;
	//fgetc()
	while ((sz = fread(&ch, 1, 1024, fd))) {
		for (int i = 0; i < sz; i++) {
			uch ch1 = ch[i];
    		_fileInfo[ch1]._appearCount++;  //�ѵ�2 �������� �±�Ϊ����
		}
		//_fileInfo[ch]._appearCount++;
	}
	//fclose(fd);
	//2.��ͳ�ƵĽ��������������
	ByteInfo invalid;
	HuffmanTree<ByteInfo> ht(_fileInfo, invalid);
	//3.���ɹ���������
	GenerateHuffmanCode(ht.getRoot());
	//cout << 1 << endl;
	// 
	// 
	//4. д������ѹ����������Ϣ
	string prefix=GetFilePrefix(filePath);
	string CompressFile = prefix + ".hz";
	FILE* fout = fopen(CompressFile.c_str(), "wb");
	WriteHeadInfo(filePath, fout);
	//5.�û�õı�������Դ�ļ����и�д

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
	cout << "ѹ���ɹ���" << CompressFile << endl;
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
		perror("��׺��ƥ��");
		return;
	}
	//1.��ȡ��ѹ����Ҫ����Ϣ
	FILE* fin = fopen(filePath.c_str(), "rb");
	if(fin == nullptr){
		perror("��ѹ���ļ�������");
		return;
    }
	//��ȡԴ�ļ�ǰ׺ --- manu1.hz -> manu1
	string unCompressFile = GetFilePrefix(filePath);
	unCompressFile += "new.";
	char buf[1024];
	//��ȡԴ�ļ���׺ �Ǵ�Դ�ļ������ж�ȡ��
	fgets(buf, sizeof(buf) , fin);
	unCompressFile += buf;
	unCompressFile.pop_back();
	//��ȡƵ����Ϣ����
	fgets(buf, sizeof(buf), fin);
	int count = atoi(buf);
	
	for (int i = 0; i < count; i++) {
		fgets(buf, sizeof(buf), fin);
		if (buf[0] == '\n') { //���⴦���� ���ѵ㣩
			fgets(buf, sizeof(buf), fin);
			_fileInfo['\n']._appearCount = std::atoi(buf + 1);
		}
		uch tmp = buf[0];
		_fileInfo[tmp]._appearCount = std::atoi(buf+2);
	}
	//2.��ԭ��������
	HuffmanTree<ByteInfo> ht(_fileInfo, ByteInfo());
	FILE* fout=fopen(unCompressFile.c_str(), "wb");
	int cnt = ht.getRoot()->_weight._appearCount;
	int fileSize = 0;
	//3.��ѹ��
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
	cout <<"��ѹ���ɹ�:"<< unCompressFile << endl;
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