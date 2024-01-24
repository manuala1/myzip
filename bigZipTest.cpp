#define  _CRT_SECURE_NO_WARNINGS 1
#include "HuffmanTree.h"
#include "FileCompressHuffman.h"
void menu()
{
	cout << "*******************************" << endl;
	cout << "*** 0. 退出                  ***" << endl;
	cout << "*** 1. huffman压缩           ***" << endl;
	cout << "*** 2. huffman解压缩           ***" << endl;
	cout << "*******************************" << endl;
}

int main() {
	//testHuffmanTree();
	/*testCompress();
	testUnCompress();*/
	int input = 0;
	bool isQuit = false;
	FileCompressHuffman fchm;
	string filename;
	while (true) {
		menu();
		cout << "请输入:";
		cin >> input;
		switch (input)
		{
			case 0:
				isQuit = 1;
				break;
			case 1:
				cout << "请输入压缩文件名称: ";
				cin >> filename;
				fchm.CompressFile(filename);
				break;
			case 2:
				cout << "请输入解压缩文件名称: ";
				cin >> filename;
				fchm.UnCompressFile(filename);
				break;
			default:
				break;
		}
		if (isQuit) {
			break;
		}
	}


	/*fchm.CompressFile("manu.txt");
	fchm.UnCompressFile("manu1.hz");*/
	//为什么压缩率不行
	
	return 0;
}