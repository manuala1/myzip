#pragma once
#include <vector>
#include <queue>
#include <iostream>
template<class W>
struct HuffmanTreeNode {
	HuffmanTreeNode<W>* _left;
	HuffmanTreeNode<W>* _right;
	W _weight;
	HuffmanTreeNode(const W& w = W()):_left(nullptr),_right(nullptr),_weight(w) {

	}
};

template<class W>
class HuffmanTree {
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree():_root(nullptr) {

	}
	HuffmanTree(const std::vector<W>& vw,const W& invalid) {
		//1.�����е�Ȩֵ����ֻ�и��ڵ�Ķ�����ɭ��
		//ɭ���ж�����Ӧ��ʹ�öѣ����ȼ����У�������
		//С��
		std::priority_queue < Node*, std::vector<Node*>, Compare>pq;
		for (auto& e : vw) {  
			if(e!=invalid)
				pq.push(new Node(e));
		}
  		while (pq.size() > 1) {
			Node* left = pq.top();
			pq.pop();
			Node* right = pq.top();
			pq.pop();
			Node* parent = new Node(left->_weight + right->_weight);
			parent->_left = left;
			parent->_right = right;
			pq.push(parent);
		}
		_root = pq.top();
      	}
	~HuffmanTree() {
		destroy(_root);
		_root = nullptr;
	}
	void destroy(Node* root) {
		if (root == nullptr)
			return;
		destroy(root->_left);
		destroy(root->_right);
		delete root;
	}
	Node* getRoot() {
		return _root;
	}
private:
	struct Compare
	{
		bool operator()(Node* n1, Node* n2) {
			return n1->_weight > n2->_weight;
		}
	};
	Node* _root;
};

//void testHuffmanTree() {
//	std::vector<int> v = { 1,3,5,7 };
//	HuffmanTree<int> ht(v);
//	std::cout << "test" << std::endl;
//}