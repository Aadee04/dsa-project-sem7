#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_map>
#include <vector>

using namespace std;

unordered_map<char, string> huffmanCodes;

struct HuffmanNode {
    char data;
    unsigned freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Custom comparison for the priority queue
struct NodeComparator {
    bool operator()(const HuffmanNode* a, const HuffmanNode* b) const {
        return a->freq > b->freq;
    }
};

HuffmanNode* buildHuffmanTree(const unordered_map<char, unsigned>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, NodeComparator> minHeap;
    
    
    for (auto& pair : freqMap)
        minHeap.push(new HuffmanNode(pair.first, pair.second));
    

    while (minHeap.size() > 1) {
        
        HuffmanNode* left = minHeap.top();
        minHeap.pop();
        HuffmanNode* right = minHeap.top();
        minHeap.pop();

        HuffmanNode* internalNode = new HuffmanNode(0, left->freq + right->freq);
        internalNode->left = left;
        internalNode->right = right;

        minHeap.push(internalNode);

        
    }
    
    return minHeap.top();
}

void buildHuffmanCodes(HuffmanNode* root, const string& code, unordered_map<char, string>& huffmanCodes) {
    if (root == NULL) {
        return;
    }

    if (root->data) {
        huffmanCodes[root->data] = code;
        return;
    }

    buildHuffmanCodes(root->left, code + "0", huffmanCodes);
    buildHuffmanCodes(root->right, code + "1", huffmanCodes);
}

void huffmanEncode(const string& inputFile, const string& outputFile, unsigned long &size) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Failed to open input file." << endl;
        return;
    }

    string data((istreambuf_iterator<char>(inFile)), (istreambuf_iterator<char>()));
    unordered_map<char, unsigned> freqMap;

    for (char c : data)
        freqMap[c]++;
    
    size = data.length();
    
    HuffmanNode* root = buildHuffmanTree(freqMap);
    
    
    
    unordered_map<char, string> huffmanCodes;
    buildHuffmanCodes(root, "", huffmanCodes);
    ::huffmanCodes = huffmanCodes;;

    string encodedData;
    for (char c : data)
        encodedData += huffmanCodes[c];

    ofstream outFile(outputFile, ios::binary);
    if (!outFile.is_open()) {
        cerr << "Failed to open output file." << endl;
        inFile.close();
        return;
    }

    for (size_t i = 0; i < encodedData.length(); i += 8) {
        unsigned char byte = 0;
        string str8 = encodedData.substr(i, 8);
        for (unsigned b = 0; b < 8; b++) {
            if (b < str8.length()) {
                byte |= (str8[b] - '0') << b;
            }
        }
        outFile.put(byte);
    }
    
    cout << "Successfully compressed..." << endl;
    long long inSize = inFile.seekg(0, ios::end).tellg(), outSize = outFile.seekp(0, ios::end).tellp();
    cout << "Input size: " << inSize << endl;
    cout << "Output size: " << outSize << endl;
    cout << "A compressed rate of: " << ( (float)outSize/inSize ) * 100 << "%" << endl;

    inFile.close();
    outFile.close();
}

void huffmanDecode(const string& inputFile, const string& outputFile, unsigned long &size) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile.is_open()) {
        cerr << "Failed to open compressed file." << endl;
        return;
    }

    ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        cerr << "Failed to open output file." << endl;
        inFile.close();
        return;
    }

    unsigned long charCount = 0;
    string concat = "";
    string bitStr = "";
    string reverse = "";
    unsigned char buffer;
    unsigned long origLength = size;
    
    
    unordered_map<string, char> uMap;
    for (const auto& pair : huffmanCodes) {
        uMap[pair.second] = pair.first;
    }

    while (inFile.read(reinterpret_cast<char*>(&buffer), sizeof(buffer))) {
        bitset<8> setByte(buffer);
        bitStr = setByte.to_string();
        
        reverse = "";

        for (int k = 7; k >= 0; k--) {
            reverse += bitStr[k];
        }

        for (int j = 0; j < 8; j++) {
            concat += reverse[j];
            if (uMap[concat]) {
                outFile << uMap[concat];
                charCount++;
                concat = "";
                if (charCount == origLength) {
                    inFile.close();
                    outFile.close();
                    return;
                }
            }
        }
    }
    cout << "Successfully decompressed..." << endl;
    
    inFile.close();
    outFile.close();
}


int main() {
    unsigned long size;
    string inputFile; // = "/Users/aadee/Desktop/NMIMS FILES/SEM VII/DSA/Project/File_Compressor/File_Compressor/input.txt";
    cout << "Enter the path to the input file: ";
    getline(cin, inputFile);
    
    string compressedFile; // = "/Users/aadee/Desktop/NMIMS FILES/SEM VII/DSA/Project/File_Compressor/File_Compressor/compressed.bin";
    cout << "Enter the path to the compressed file: ";
    getline(cin, compressedFile);
    
    string decompressedFile; // = "/Users/aadee/Desktop/NMIMS FILES/SEM VII/DSA/Project/File_Compressor/File_Compressor/decompressed.txt";
    cout << "Enter the path to the decompressed file: ";
    getline(cin, decompressedFile);

    // Just the names
    string inputName = inputFile.substr(inputFile.rfind("/") + 1);
    string compressedName = compressedFile.substr(compressedFile.rfind("/") + 1);
    string decompressedName = decompressedFile.substr(decompressedFile.rfind("/") + 1);
    
    // Compress the file
    cout << "\nCompressing..." << endl;
    huffmanEncode(inputFile, compressedFile, size);
    cout << inputName << " has been compressed to " << compressedName << endl;
    cout << "Location:" << compressedFile << endl << endl;
    
    // Decompress the file
    cout << "Decompressing..." << endl;
    huffmanDecode(compressedFile, decompressedFile, size);
    cout << compressedName << " has been decompressed to " << decompressedName << endl;
    cout << "Location:" << decompressedFile << endl;
    
    return 0;
}
