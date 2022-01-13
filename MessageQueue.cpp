#include "stdafx.h"
#include "MessageQueue.h"

MessageQueue::MessageQueue(int maxSize)
{
	maxDataSize = maxSize;
}

void MessageQueue::insert(const char msg[])
{
	mutex.lock();
	std::cout << msg << std::endl;
	std::string* data = new std::string(msg);
	Node* node = new Node();
	node->data = data;
	dataSize++;

	if (dataSize == 1)
	{
		node->nextNode = nullptr;
		node->prevNode = nullptr;
		headNode = node;
		tailNode = node;
	}
	else
	{
		if (dataSize == maxDataSize+1)
		{
			freeNode(tailNode);
			node->nextNode = headNode;
			headNode->prevNode = node;
			headNode = node;
		}
		else
		{
			node->nextNode = headNode;
			headNode->prevNode = node;
			headNode = node;
		}
	}
	mutex.unlock();
}

void MessageQueue::insert(std::string msg)
{
	mutex.lock();
	std::cout << msg << std::endl;
	std::string* data = new std::string(msg);
	Node* node = new Node();
	node->data = data;
	dataSize++;

	if (dataSize == 1)
	{
		node->nextNode = nullptr;
		node->prevNode = nullptr;
		headNode = node;
		tailNode = node;
	}
	else
	{
		if (dataSize == maxDataSize + 1)
		{
			freeNode(tailNode);
			node->nextNode = headNode;
			headNode->prevNode = node;
			headNode = node;
		}
		else
		{
			node->nextNode = headNode;
			headNode->prevNode = node;
			headNode = node;
		}
	}
	mutex.unlock();
}

int MessageQueue::size()
{
	return dataSize;
}

void MessageQueue::clear()
{
	mutex.lock();
	while (dataSize != 0)
	{
		freeNode(tailNode);
	}
	mutex.unlock();
}

std::string& MessageQueue::at(int index)
{
	Node* node = headNode;
	for (int i = 0; i < index; i++)
	{
		node = node->nextNode;
	}
	return *node->data;
}

void MessageQueue::freeNode(Node* node)
{
	delete node->data;
	if (node->prevNode == nullptr)
	{
		headNode = node->nextNode;
		node->nextNode->prevNode = nullptr;
	}
	else if (node->nextNode == nullptr)
	{
		tailNode = node->prevNode;
		node->prevNode->nextNode = nullptr;
	}
	else
	{
		node->prevNode->nextNode = node->nextNode;
		node->nextNode->prevNode = node->prevNode;
	}
	delete node;
	dataSize--;
}

MessageQueueIter::MessageQueueIter(MessageQueue* queue)
{
	this->queue = queue;
}

std::string MessageQueueIter::next()
{
	std::string* ret = nullptr;
	if (index == queue->size())
	{
		throw "Stop Iteration";
	}
	else
	{
		ret = &(queue->at(index));
		index++;
	}
	return *ret;
}
