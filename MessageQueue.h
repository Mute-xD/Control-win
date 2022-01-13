#pragma once
#include "stdafx.h"


struct Node
{
	std::string* data;
	Node* nextNode;
	Node* prevNode;
};

class MessageQueue
{
public:
	MessageQueue(int maxSize);
	void insert(const char msg[]);
	void insert(std::string msg);
	int size();
	void clear();
	std::string& at(int index);
private:
	int dataSize = 0;
	int maxDataSize = 0;
	Node* headNode = nullptr;
	Node* tailNode = nullptr;
	void freeNode(Node* node);
	std::mutex mutex;
};

class MessageQueueIter
{
public:
	MessageQueueIter(MessageQueue* queue);
	std::string next();
private:
	MessageQueue* queue = nullptr;
	int index = 0;
};