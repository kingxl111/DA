#include <bits/stdc++.h>

using namespace std;

class SuffixArray {
public:
    vector<int> BuildSuffixArray(const vector<uint8_t>& input) {
        int n = input.size();
        vector<int> suffixArray(n), classes(n), cnt(max(256, n), 0);

        // Initial sorting by first character
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

        // Doubling step
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
        HuffmanNode* left = pq.top(); pq.pop();
        HuffmanNode* right = pq.top(); pq.pop();

        HuffmanNode* merged = new HuffmanNode(0, left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    return pq.top();
}

void buildHuffmanTable(HuffmanNode* root, string code, unordered_map<uint8_t, string>& huffmanTable) {
    if (!root) return;

    if (!root->left && !root->right) {
        huffmanTable[root->byte] = code;
    }

    buildHuffmanTable(root->left, code + "0", huffmanTable);
    buildHuffmanTable(root->right, code + "1", huffmanTable);
}

void deleteHuffmanTree(HuffmanNode* root) {
    if (!root) return;
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
    vector<uint8_t> augmentedInput(input);
    augmentedInput.push_back(0); 

    SuffixArray sa;
    vector<int> suffixArray = sa.BuildSuffixArray(augmentedInput);

    vector<uint8_t> bwtResult(n + 1);
    int primaryIndex = -1;
    for (int i = 0; i < n + 1; ++i) {
        if (suffixArray[i] == 0) {
            primaryIndex = i;
            bwtResult[i] = augmentedInput.back();
        } else {
            bwtResult[i] = augmentedInput[suffixArray[i] - 1];
        }
    }

    return {bwtResult, primaryIndex};
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


int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(0);

    if (argc < 2) {
        cerr << "Usage: <compress|decompress> input_file output_file" << endl;
        return 1;
    }

    string cmd = argv[1];

    if (cmd == "compress") {
        if (argc < 4) {
            cerr << "Usage: compress input_file output_file" << endl;
            return 1;
        }

        string inputFilename = argv[2];
        string outputFilename = argv[3];

        ifstream inputFile(inputFilename, ios::binary);
        if (!inputFile) {
            cerr << "Error opening input file." << endl;
            return 1;
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
        cout << "ENCODED DATA: " << encodedData;
        ofstream outputFile(outputFilename, ios::binary);
        if (!outputFile) {
            cerr << "Error opening output file." << endl;
            return 1;
        }

        outputFile.write(reinterpret_cast<const char*>(&primaryIndex), sizeof(primaryIndex));
        printf("PRIMARY INDEX: %d\n", primaryIndex);
        size_t encodedBitCount = encodedData.size();
        printf("ENCODED BIT COUNT: %ld\n", encodedBitCount);
        outputFile.write(reinterpret_cast<const char*>(&encodedBitCount), sizeof(encodedBitCount));
        writeEncodedData(outputFile, encodedData);

        for (int freq : frequencies) {
            outputFile.write(reinterpret_cast<const char*>(&freq), sizeof(freq));
        }

        deleteHuffmanTree(root);
        outputFile.close();
    }

    else if (cmd == "decompress") {
        if (argc < 4) {
            cerr << "Usage: decompress input_file output_file" << endl;
            return 1;
        }

        string inputFilename = argv[2];
        string outputFilename = argv[3];

        ifstream inputFile(inputFilename, ios::binary);
        if (!inputFile) {
            cerr << "Error opening input file." << endl;
            return 1;
        }
        int primaryIndex;
        inputFile.read(reinterpret_cast<char*>(&primaryIndex), sizeof(primaryIndex));
        printf("PRIMARY INDEX: %d\n", primaryIndex);
        size_t encodedBitCount;
        inputFile.read(reinterpret_cast<char*>(&encodedBitCount), sizeof(encodedBitCount));
        printf("ENCODED BIT COUNT: %ld\n", encodedBitCount);

        string encodedData;
        readEncodedData(inputFile, encodedData, encodedBitCount);
        cout << "ENCODED DATA: " << encodedData;
        vector<int> frequencies(256, 0);
        for (int& freq : frequencies) {
            inputFile.read(reinterpret_cast<char*>(&freq), sizeof(freq));
            if (!inputFile) {
                cerr << "Error reading frequency table." << endl;
                return 1;
            }
        }

        HuffmanNode* root = buildHuffmanTree(frequencies);

        vector<uint8_t> decodedData;
        HuffmanDecode(encodedData, root, decodedData);

        vector<uint8_t> rleDecoded = ReverseRLE(decodedData);
        vector<uint8_t> mtfDecoded = ReverseMTF(rleDecoded);
        vector<uint8_t> originalData = ReverseBWT(mtfDecoded, primaryIndex);

        ofstream outputFile(outputFilename, ios::binary);
        if (!outputFile) {
            cerr << "Error opening output file." << endl;
            return 1;
        }

        outputFile.write(reinterpret_cast<char*>(originalData.data()), originalData.size());
        outputFile.close();

        deleteHuffmanTree(root);
    }
    
    else {
        cerr << "Invalid command." << endl;
        return 1;
    }

    return 0;
}
