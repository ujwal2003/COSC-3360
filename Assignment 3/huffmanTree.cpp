#ifndef HUFFMANTREE_CPP
#define HUFFMANTREE_CPP

#include <iostream>
#include <string>
#include <queue>
#include <utility>
#include "huffmanTree.h"

using namespace std;

//free memory when destructor is called
HuffmanNode::~HuffmanNode() {
    delete left;
    delete right;
}

//create new huffman tree node
HuffmanNode* createTreeNode(char nodeCh, int nodeFreq, HuffmanNode* leftChild, HuffmanNode* rightChild) {
    HuffmanNode* hNode = new HuffmanNode();

    hNode->character = nodeCh;
    hNode->frequency = nodeFreq;
    hNode->left = leftChild;
    hNode->right = rightChild;

    return hNode;
}

//generate huffman tree, this function returns the root
HuffmanNode* createHuffmanTree(priority_queue<HuffmanNode*, vector<HuffmanNode*>, huffmanComparator> pq, int currAge) {
    int nodeAge = currAge;

    while(pq.size() != 1) {
        HuffmanNode* node_left = pq.top(); //left node (least value)
        //cout << "\t LEFT: " << node_left->character << ' ' << node_left->frequency << endl; 
        pq.pop();

        HuffmanNode* node_right = pq.top(); //right node (next least value)
        //cout << "\t RIGHT: " << node_right->character << ' ' << node_right->frequency << endl; 
        pq.pop();

        int freqSum = node_left->frequency + node_right->frequency;
        HuffmanNode* temp = createTreeNode('\0', freqSum, node_left, node_right);
        nodeAge+=1;
        temp->age = nodeAge;
        pq.push(temp);
    }

    return pq.top();
}

//traverse through the huffman tree and print the huffman codes (inorder)
void printHuffmanTree(HuffmanNode* root, string code) {
    if(root == nullptr)
        return;

    else if(root->left == nullptr && root->right == nullptr) {
        cout << "Symbol: " << root->character << ", Frequency: " << root->frequency << ", Code: " << code << endl;
    }

    printHuffmanTree(root->left, code+"0");
    printHuffmanTree(root->right, code+"1");
}

//traverse through the huffman tree and print out the character from the respective code
pair<char, int> getCharacterFromHuffmanCode(HuffmanNode* root, string code, int codeIndex) {
    if(root == nullptr)
        return make_pair('\0', -1);

    if(root->left == nullptr && root->right == nullptr) {
        return make_pair(root->character, root->frequency);
    }

    codeIndex += 1;
    if(code[codeIndex] == '0') {
        return getCharacterFromHuffmanCode(root->left, code, codeIndex);
    } else {
        return getCharacterFromHuffmanCode(root->right, code, codeIndex);   
    }
}

#endif