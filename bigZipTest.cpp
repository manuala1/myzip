#define  _CRT_SECURE_NO_WARNINGS 1
#include "HuffmanTree.h"
#include "FileCompressHuffman.h"
void menu()
{
	cout << "*******************************" << endl;
	cout << "*** 0. �˳�                  ***" << endl;
	cout << "*** 1. huffmanѹ��           ***" << endl;
	cout << "*** 2. huffman��ѹ��           ***" << endl;
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
		cout << "������:";
		cin >> input;
		switch (input)
		{
			case 0:
				isQuit = 1;
				break;
			case 1:
				cout << "������ѹ���ļ�����: ";
				cin >> filename;
				fchm.CompressFile(filename);
				break;
			case 2:
				cout << "�������ѹ���ļ�����: ";
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
	//Ϊʲôѹ���ʲ���
	
	return 0;
}