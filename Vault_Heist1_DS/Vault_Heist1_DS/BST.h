#pragma once

struct Node {
	int data;
	Node* right;
	Node* left;

	Node(int value) {
		data = value;
		right = nullptr;
		left = nullptr;
	}
};

class BST {
	Node* root;

public:
	BST();
	void insert(int value);
	bool search( int target, int path[], int* pathlength);
	Node* getRoot();
};
