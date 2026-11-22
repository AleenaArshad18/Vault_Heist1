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
	bool deleteValue(int value);
	bool findMinimum(int* minimumValue);
	bool findMaximum(int* maximumValue);
	bool findSuccessor(int value, int* successorValue);
	bool findPredecessor(int value, int* predecessorValue);
	Node* getRoot();

	void rangeQuery(Node* node, int low, int high, int output[], int& count);
	void rangeQuery(int low, int high, int output[], int& count);
	void clear();
};
