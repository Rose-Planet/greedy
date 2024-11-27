#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <chrono> // 用于性能计时
#include <bitset>

using namespace std;
using namespace chrono;

// 哈夫曼树节点结构
struct HuffmanNode {
    char ch;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

// 比较器，用于优先队列
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// 读取文件并统计字符频率
unordered_map<char, int> countFrequency(const string& filePath) {
    unordered_map<char, int> freqMap;
    ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        cerr << "无法打开文件: " << filePath << endl;
        exit(1);
    }
    char ch;
    while (inputFile.get(ch)) {
        freqMap[ch]++;
    }
    inputFile.close();
    return freqMap;
}

// 构建哈夫曼树
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& freqMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();
        HuffmanNode* newNode = new HuffmanNode('\0', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        pq.push(newNode);
    }

    return pq.top();
}

// 生成哈夫曼编码表
void generateHuffmanCodes(HuffmanNode* root, const string& prefix, unordered_map<char, string>& huffmanCodes) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = prefix;
    }
    generateHuffmanCodes(root->left, prefix + "0", huffmanCodes);
    generateHuffmanCodes(root->right, prefix + "1", huffmanCodes);
}

// 保存哈夫曼编码文件
void saveEncodedFile(const string& encodedText, const string& fileName) {
    ofstream encodedFile(fileName);
    if (encodedFile.is_open()) {
        encodedFile << encodedText;
        encodedFile.close();
        cout << "编码文件已保存到 " << fileName << " 文件中。" << endl;
    } else {
        cerr << "无法创建 " << fileName << " 文件！" << endl;
    }
}

// 保存解码后的文件
void saveDecodedFile(const string& decodedText, const string& fileName) {
    ofstream decodedFile(fileName);
    if (decodedFile.is_open()) {
        decodedFile << decodedText;
        decodedFile.close();
        cout << "解码文件已保存到 " << fileName << " 文件中。" << endl;
    } else {
        cerr << "无法创建 " << fileName << " 文件！" << endl;
    }
}

// 定长编码表
unordered_map<char, string> generateFixedLengthCodes(const unordered_map<char, int>& freqMap) {
    unordered_map<char, string> fixedLengthCodes;
    int index = 0;
    for (const auto& pair : freqMap) {
        fixedLengthCodes[pair.first] = bitset<8>(index++).to_string();
    }
    return fixedLengthCodes;
}

// 计算编码后的文件大小
int calculateFileSize(const string& text, const unordered_map<char, string>& encodingDict) {
    int totalSize = 0;
    for (char ch : text) {
        totalSize += encodingDict.at(ch).size();
    }
    return totalSize;
}

// 使用哈夫曼编码对文本编码
string encodeText(const string& text, const unordered_map<char, string>& encodingDict) {
    string encodedText;
    for (char ch : text) {
        encodedText += encodingDict.at(ch);
    }
    return encodedText;
}

// 解码哈夫曼编码
string decodeHuffman(const string& encodedText, HuffmanNode* root) {
    string decodedText;
    HuffmanNode* current = root;
    for (char bit : encodedText) {
        current = (bit == '0') ? current->left : current->right;
        if (!current->left && !current->right) {
            decodedText += current->ch;
            current = root;
        }
    }
    return decodedText;
}

// 主函数
int main() {
    // 文件列表
    vector<string> fileList = {"huffman.txt", "A Tale of Two Cities - Charles Dickens.txt"};

    // CSV 文件初始化
    ofstream csvFile("huffman_results.csv", ios::binary);
    if (csvFile.is_open()) {
        // 写入 UTF-8 BOM 标记
        unsigned char bom[] = {0xEF, 0xBB, 0xBF};
        csvFile.write(reinterpret_cast<char*>(bom), sizeof(bom));

        // 写入标题
        csvFile << "文件名,编码方式,原文件大小 (bits),编码文件大小 (bits),压缩比,编码时间 (微秒),解码时间 (微秒)\n";

        for (const string& filePath : fileList) {
            unordered_map<char, int> freqMap = countFrequency(filePath);

            // 读取文件内容
            ifstream inputFile(filePath);
            string text((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
            inputFile.close();

            int originalFileSize = text.size() * 8;

            // 定长编码
            unordered_map<char, string> fixedLengthCodes = generateFixedLengthCodes(freqMap);
            auto startTime = high_resolution_clock::now();
            string fixedLengthEncodedText = encodeText(text, fixedLengthCodes);
            auto endTime = high_resolution_clock::now();
            auto fixedLengthEncodeTime = duration_cast<microseconds>(endTime - startTime).count(); // 改为微秒
            int fixedLengthFileSize = calculateFileSize(text, fixedLengthCodes);

            startTime = high_resolution_clock::now();
            string fixedLengthDecodedText = fixedLengthEncodedText; // 定长无需解码树
            endTime = high_resolution_clock::now();
            auto fixedLengthDecodeTime = duration_cast<microseconds>(endTime - startTime).count(); // 改为微秒

            // 哈夫曼编码
            startTime = high_resolution_clock::now();
            HuffmanNode* huffmanTree = buildHuffmanTree(freqMap);
            unordered_map<char, string> huffmanCodes;
            generateHuffmanCodes(huffmanTree, "", huffmanCodes);
            string huffmanEncodedText = encodeText(text, huffmanCodes);
            endTime = high_resolution_clock::now();
            auto huffmanEncodeTime = duration_cast<microseconds>(endTime - startTime).count(); // 改为微秒
            int huffmanFileSize = calculateFileSize(text, huffmanCodes);

            startTime = high_resolution_clock::now();
            string huffmanDecodedText = decodeHuffman(huffmanEncodedText, huffmanTree);
            endTime = high_resolution_clock::now();
            auto huffmanDecodeTime = duration_cast<microseconds>(endTime - startTime).count(); // 改为微秒

            // 保存哈夫曼编码文件和解码文件
            if (filePath == "A Tale of Two Cities - Charles Dickens.txt") {
                saveEncodedFile(huffmanEncodedText, "A Tale of Two Cities_code.txt");
                saveDecodedFile(huffmanDecodedText, "A Tale of Two Cities_huffman.txt");
            }

            // 写入 CSV 文件
            csvFile << filePath << ",定长编码," << originalFileSize << "," << fixedLengthFileSize << ","
                    << (double)originalFileSize / fixedLengthFileSize << "," << fixedLengthEncodeTime << ","
                    << fixedLengthDecodeTime << "\n";
            csvFile << filePath << ",哈夫曼编码," << originalFileSize << "," << huffmanFileSize << ","
                    << (double)originalFileSize / huffmanFileSize << "," << huffmanEncodeTime << ","
                    << huffmanDecodeTime << "\n";
        }

        csvFile.close();
        cout << "结果已保存到 huffman_results.csv 文件中。" << endl;
    } else {
        cerr << "无法创建 CSV 文件！" << endl;
    }

    return 0;
}