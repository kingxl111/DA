#include <bits/stdc++.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

class SuffixArray {
public:
    vector<int> BuildSuffixArray(const vector<uint8_t>& input) {
        int n = input.size();
        vector<int> suffixArray(n), classes(n), cnt(max(256, n), 0);

        for (int i = 0; i < n; ++i) {
            cnt[input[i]]++;
        }
        for (int i = 1; i < 256; ++i) {
            cnt[i] += cnt[i - 1];
        }
        for (int i = n - 1; i >= 0; --i) {
            suffixArray[--cnt[input[i]]] = i;
        }
        classes[suffixArray[0]] = 0;
        int numClasses = 1;
        for (int i = 1; i < n; ++i) {
            if (input[suffixArray[i]] != input[suffixArray[i - 1]]) {
                numClasses++;
            }
            classes[suffixArray[i]] = numClasses - 1;
        }

        vector<int> newSuffixArray(n), newClasses(n);
        for (int len = 1; len < n; len *= 2) {
            for (int i = 0; i < n; ++i) {
                newSuffixArray[i] = suffixArray[i] - len;
                if (newSuffixArray[i] < 0) {
                    newSuffixArray[i] += n;
                }
            }

            fill(cnt.begin(), cnt.begin() + numClasses, 0);
            for (int i = 0; i < n; ++i) {
                cnt[classes[newSuffixArray[i]]]++;
            }
            for (int i = 1; i < numClasses; ++i) {
                cnt[i] += cnt[i - 1];
            }
            for (int i = n - 1; i >= 0; --i) {
                suffixArray[--cnt[classes[newSuffixArray[i]]]] = newSuffixArray[i];
            }

            newClasses[suffixArray[0]] = 0;
            numClasses = 1;
            for (int i = 1; i < n; ++i) {
                pair<int, int> cur = {classes[suffixArray[i]],
                                      classes[(suffixArray[i] + len) % n]};
                pair<int, int> prev = {classes[suffixArray[i - 1]],
                                       classes[(suffixArray[i - 1] + len) % n]};
                if (cur != prev) {
                    numClasses++;
                }
                newClasses[suffixArray[i]] = numClasses - 1;
            }
            classes.swap(newClasses);
        }

        return suffixArray;
    }
};

struct HuffmanNode {
    uint8_t byte;
    int freq;
    HuffmanNode *left, *right;

    HuffmanNode(uint8_t b, int f) : byte(b), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return l->freq > r->freq;
    }
};

HuffmanNode* buildHuffmanTree(const vector<int>& frequencies) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    for (int i = 0; i < frequencies.size(); ++i) {
        if (frequencies[i] > 0) {
            pq.push(new HuffmanNode(i, frequencies[i]));
        }
    }

    while (pq.size() > 1) {

        HuffmanNode* left = pq.top(); 
        pq.pop();
        HuffmanNode* right = pq.top(); 
        pq.pop();

        HuffmanNode* merged = new HuffmanNode(0, left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    return pq.top();
}

void buildHuffmanTable(HuffmanNode* root, string code, unordered_map<uint8_t, string>& huffmanTable) {

    if (!root) {
        return;
    }

    if (!root->left && !root->right) {
        huffmanTable[root->byte] = code;
    }

    buildHuffmanTable(root->left, code + "0", huffmanTable);
    buildHuffmanTable(root->right, code + "1", huffmanTable);
}

void deleteHuffmanTree(HuffmanNode* root) {
    
    if (!root) { 
        return;
    }
    deleteHuffmanTree(root->left);
    deleteHuffmanTree(root->right);
    delete root;
}

void HuffmanEncode(const vector<uint8_t>& input, string& encoded, unordered_map<uint8_t, string>& huffmanTable) {
    for (uint8_t byte : input) {
        encoded += huffmanTable[byte];
    }
}

void HuffmanDecode(const string& encoded, HuffmanNode* root, vector<uint8_t>& decoded) {
    HuffmanNode* current = root;
    for (char bit : encoded) {
        if (bit == '0') current = current->left;
        else current = current->right;

        if (!current->left && !current->right) {
            decoded.push_back(current->byte);
            current = root;
        }
    }
}

pair<vector<uint8_t>, int> BWT(const vector<uint8_t>& input) {
    int n = input.size();
    vector<uint8_t> modifiedInput(input);
    modifiedInput.push_back(0); 

    SuffixArray sufAr;
    vector<int> suffixArray = sufAr.BuildSuffixArray(modifiedInput);

    vector<uint8_t> bwtResult(n + 1);
    int primaryIndex = -1;
    for (int i = 0; i < n + 1; ++i) {
        if (suffixArray[i] == 0) {
            primaryIndex = i;
            bwtResult[i] = modifiedInput.back();
        } else {
            bwtResult[i] = modifiedInput[suffixArray[i] - 1];
        }
    }

    return {bwtResult, primaryIndex};
}

vector<uint8_t> MTF(const vector<uint8_t>& input) {
    vector<uint8_t> alphabet(256);
    iota(alphabet.begin(), alphabet.end(), 0);

    vector<uint8_t> result;
    for (uint8_t byte : input) {
        auto it = find(alphabet.begin(), alphabet.end(), byte);
        int pos = distance(alphabet.begin(), it);
        result.push_back(pos);
        alphabet.erase(it);
        alphabet.insert(alphabet.begin(), byte);
    }

    return result;
}

vector<uint8_t> RLE(const vector<uint8_t>& input) {
    vector<uint8_t> result;
    for (size_t i = 0; i < input.size(); ++i) {
        uint8_t count = 1;
        while (i + 1 < input.size() && input[i] == input[i + 1] && count < 255) {
            ++count;
            ++i;
        }
        result.push_back(input[i]);
        result.push_back(count);
    }

    return result;
}


vector<uint8_t> ReverseBWT(const vector<uint8_t>& bwt, int primaryIndex) {
    int n = bwt.size();
    vector<int> count(256, 0), rank(n);

    for (uint8_t c : bwt) {
        count[c]++;
    }
    for (int i = 1; i < 256; ++i) {
        count[i] += count[i - 1];
    }

    for (int i = n - 1; i >= 0; --i) {
        rank[i] = --count[bwt[i]];
    }

    vector<uint8_t> result(n - 1); 
    int idx = primaryIndex;
    for (int i = n - 2; i >= 0; --i) {
        idx = rank[idx];
        result[i] = bwt[idx];
    }

    return result;
}

vector<uint8_t> ReverseMTF(const vector<uint8_t>& mtf) {
    vector<uint8_t> alphabet(256);
    iota(alphabet.begin(), alphabet.end(), 0);

    vector<uint8_t> result;
    for (uint8_t pos : mtf) {
        uint8_t byte = alphabet[pos];
        result.push_back(byte);
        alphabet.erase(alphabet.begin() + pos);
        alphabet.insert(alphabet.begin(), byte);
    }

    return result;
}

vector<uint8_t> ReverseRLE(const vector<uint8_t>& rle) {
    vector<uint8_t> result;
    for (size_t i = 0; i < rle.size(); i += 2) {
        result.insert(result.end(), rle[i + 1], rle[i]);
    }

    return result;
}

void writeEncodedData(ofstream& outputFile, const string& encodedData) {
    uint8_t buffer = 0;
    int bitCount = 0;

    for (char bit : encodedData) {
        buffer <<= 1;
        if (bit == '1') {
            buffer |= 1;
        }
        bitCount++;

        if (bitCount == 8) {
            outputFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }
    if (bitCount > 0) { 
        buffer <<= (8 - bitCount);
        outputFile.write(reinterpret_cast<const char*>(&buffer), sizeof(buffer));
    }
}

void readEncodedData(ifstream& inputFile, string& encodedData, size_t bitCount) {
    uint8_t buffer;
    while (inputFile.read(reinterpret_cast<char*>(&buffer), sizeof(buffer))) {
        for (int i = 7; i >= 0; --i) {
            encodedData += ((buffer >> i) & 1) ? '1' : '0';
            if (encodedData.size() == bitCount) {
                return;
            }
        }
    }
}

void printUsageInfo() {
    cerr << "Usage: my_gzip [-cdlrt19] [file...]\n";
    cerr << "Options:\n"
         << "  -c   Write output to stdout, keep original files unchanged.\n"
         << "  -d   Decompress files.\n"
         << "  -l   List compressed file contents.\n"
         << "  -k   Keep\n"
         << "  -r   Process directories recursively.\n"
         << "  -t   Test compressed file integrity.\n"
         << "  -1   Fastest (min compression).\n"
         << "  -9   Best compression.\n";
}

void collectFiles(const string& path, vector<string>& files, bool recursive) {
    if (recursive) {
        for (const auto& entry : fs::recursive_directory_iterator(path)) {
            if (fs::is_regular_file(entry.path())) {
                files.push_back(entry.path().string());
            }
        }
    } else {
        if (fs::is_regular_file(path)) {
            files.push_back(path);
        } else if (!fs::is_directory(path)) {
            files.push_back(path);
        } else {
            cerr << "Error: " << path << " is not a file." << endl;
        }
    }
}

// TODO: add light compress and light de without Huffman
void LiteCompress(const string& inputFilename, const string& outputFilename, bool toStdout) {}

void LiteDecompress(const string& inputFilename, const string& outputFilename, bool toStdout) {}

void Compress(const string& inputFilename, const string& outputFilename, bool toStdout) {
    ifstream inputFile(inputFilename, ios::binary);
    if (!inputFile) {
        cerr << "Error opening input file." << endl;
    }
    vector<uint8_t> inputData((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    auto [bwtData, primaryIndex] = BWT(inputData);
    vector<uint8_t> mtfData = MTF(bwtData);
    vector<uint8_t> rleData = RLE(mtfData);

    vector<int> frequencies(256, 0);
    for (uint8_t byte : rleData) {
        frequencies[byte]++;
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    unordered_map<uint8_t, string> huffmanTable;
    buildHuffmanTable(root, "", huffmanTable);

    string encodedData;
    HuffmanEncode(rleData, encodedData, huffmanTable);
    // cout << "ENCODED DATA: " << encodedData;
    ofstream outputFile(outputFilename, ios::binary | ios::app);
    if(!toStdout) {
        if (!outputFile) {
            cerr << "Error opening output file." << endl;
        }
    }

    if (toStdout) {
        size_t encodedBitCount = encodedData.size(); 
        cout 
        << reinterpret_cast<const char*>(&primaryIndex) 
        << reinterpret_cast<const char*>(&encodedBitCount);
        uint8_t buffer = 0;
        int bitCount = 0;
        for (char bit : encodedData) {
            buffer <<= 1;
            if (bit == '1') {
                buffer |= 1;
            }
            bitCount++;

            if (bitCount == 8) {
                cout << reinterpret_cast<const char*>(&buffer);
                buffer = 0;
                bitCount = 0;
            }
        }
        if (bitCount > 0) { 
            buffer <<= (8 - bitCount);
            cout << reinterpret_cast<const char*>(&buffer);
        }
        deleteHuffmanTree(root);
        return;
    }

    outputFile.write(reinterpret_cast<const char*>(&primaryIndex), sizeof(primaryIndex));
    // printf("PRIMARY INDEX: %d\n", primaryIndex);
    size_t encodedBitCount = encodedData.size();
    // printf("ENCODED BIT COUNT: %ld\n", encodedBitCount);
    outputFile.write(reinterpret_cast<const char*>(&encodedBitCount), sizeof(encodedBitCount));
    writeEncodedData(outputFile, encodedData);

    for (int freq : frequencies) {
        outputFile.write(reinterpret_cast<const char*>(&freq), sizeof(freq));
    }

    deleteHuffmanTree(root);
    outputFile.close();
}

void Decompress(const string& inputFilename, const string& outputFilename, bool toStdout) {

    ifstream inputFile(inputFilename, ios::binary);
    if (!inputFile) {
        cerr << "Error opening input file." << endl;
        return;
    }
    ofstream outputFile(outputFilename, ios::binary | ios::app);
    if (!outputFile) {
        cerr << "Error opening output file." << endl;
        return;
    }

    while(!inputFile.eof()) {
        int primaryIndex;
        inputFile.read(reinterpret_cast<char*>(&primaryIndex), sizeof(primaryIndex));
        if (inputFile.eof()) {
            return;
        }
        if (!inputFile) {
            cerr << "Error reading primary index." << endl;
            return;
        }

        size_t encodedBitCount;
        inputFile.read(reinterpret_cast<char*>(&encodedBitCount), sizeof(encodedBitCount));
        if (!inputFile) {
            cerr << "Error reading encoded bit count." << endl;
            return;
        }

        string encodedData;
        readEncodedData(inputFile, encodedData, encodedBitCount);

        vector<int> frequencies(256, 0);
        for (int& freq : frequencies) {
            inputFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
            if (!inputFile) {
                cerr << "Error reading frequency table." << endl;
                return;
            }
        }

        HuffmanNode* root = buildHuffmanTree(frequencies);

        vector<uint8_t> decodedData;
        HuffmanDecode(encodedData, root, decodedData);

        vector<uint8_t> rleDecoded = ReverseRLE(decodedData);
        vector<uint8_t> mtfDecoded = ReverseMTF(rleDecoded);
        vector<uint8_t> originalData = ReverseBWT(mtfDecoded, primaryIndex);

        outputFile.write(reinterpret_cast<char*>(originalData.data()), originalData.size());
        deleteHuffmanTree(root);
    }

    outputFile.close();
    inputFile.close();
}


int main(int argc, char* argv[]) {

    if (argc < 3) {
        printUsageInfo();
        return -1;
    }

    bool compress = true;
    bool decompress = false;
    bool toStdout = false;
    bool list = false; 
    bool recursive = false;
    bool test = false;
    bool minCompress = false;
    bool maxCompress = true;
    bool keep = false;

    string inputFile = "-", outputFile;
    vector<string> inputFiles;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg[0] == '-') {
            for (char c : arg.substr(1)) {
                switch (c) {
                    case 'c': 
                        toStdout = true; 
                        break;
                    case 'd': 
                        compress = false; 
                        decompress = true;
                        break;
                    case 'l':
                        list = true;
                        break;
                    case 'r':
                        recursive = true;
                        break;
                    case 't':
                        test = true;
                        break;
                    case '1':
                        maxCompress = false;
                        minCompress = true;
                        break;
                    case '9':
                        maxCompress = true;
                        minCompress = false;
                        break;
                    case 'k':
                        keep = true;
                        break;
                    default:
                        cerr << "Wrong option: -" << c << endl; 
                        return 1;
                }
            }
        } else {
            if (inputFile == "-") {
                inputFile = arg;
                collectFiles(inputFile, inputFiles, recursive);
            } else if (i + 1 < argc) {
                collectFiles(arg, inputFiles, recursive);
            } else {
                if (toStdout) {
                    collectFiles(arg, inputFiles, recursive);
                } else {
                    if (!fs::is_directory(arg)) {
                        outputFile = arg;
                    } else {
                        cerr << "Error: " << arg << " is not a file." << endl;
                        return -1;
                    }
                }
            }
        }
    }

    cout << "compress: " << compress << endl
         << "decompress: " << decompress << endl
         << "toStdout: " << toStdout << endl
         << "list: " << list << endl
         << "recursive: " << recursive << endl
         << "test: " << test << endl
         << "minCompress: " << minCompress << endl
         << "maxCompress: " << maxCompress << endl
         << "outputFile: " << outputFile << endl
         << endl;
    
    for (size_t i = 0; i < inputFiles.size(); i++) {
        cout << "inputFile{" << i + 1 << "}: " << inputFiles[i] << endl; 
    }

    if (compress) {
        for (size_t i = 0; i < inputFiles.size(); i++) {
            if (maxCompress) {
                Compress(inputFiles[i], outputFile, toStdout);
            } else {
                LiteCompress(inputFiles[i], outputFile, toStdout);
            }
        }
    }
    else if (decompress) {
        if (maxCompress) {
            for (size_t i = 0; i < inputFiles.size(); i++) {
                Decompress(inputFiles[i], outputFile, toStdout);
            }
        } else {
            for (size_t i = 0; i < inputFiles.size(); i++) {
                LiteDecompress(inputFiles[i], outputFile, toStdout);
            }
        }
    } else {
        cerr << "Compress or decompress required!" << endl;
        return -1;
    }

}

    /*
    if (outputFile.empty()) {
        outputFile = compress ? inputFile + ".gz" : inputFile.substr(0, inputFile.rfind('.'));
    }

    if (compress) {
        Compress(inputFile, outputFile, toStdout);
    }
    */

