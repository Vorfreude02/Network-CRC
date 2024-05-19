#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

string calculateCRC(string dataword, string generator) {
    int dLen = dataword.length(); //dataword 길이
    int gLen = generator.length(); //generator 길이
    string appendedData = dataword + string(gLen - 1, '0');

    for (int i = 0; i <= dLen - 1; i++) {
        if (appendedData[i] == '1') {
            for (int j = 0; j < gLen; j++) {
                appendedData[i + j] = (appendedData[i + j] == generator[j] ? '0' : '1');
            }
        }
    }

    string remainder = appendedData.substr(appendedData.length() - (gLen - 1));
    return dataword + remainder;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cout << "usage: " << argv[0] << " input_file output_file generator dataword_size" << endl;
        return 1;
    } // 조건(1) 인자 4개 받고 아닐 경우 메시지 출력

    ifstream input_file(argv[1], ios::binary);
    if (!input_file) {
        cout << "input file open error." << endl;
        return 1;
    } // 조건(2) input_file 열기 argv[1]

    ofstream output_file(argv[2], ios::binary);
    if (!output_file) {
        cout << "output file open error." << endl;
        return 1;
    } // 조건(3) output_file 열기 argv[2]

    string generator = argv[3];
    int dataword_size = atoi(argv[4]);
    if (dataword_size != 4 && dataword_size != 8) {
        cout << "dataword size must be 4 or 8." << endl;
        return 1;
    } //dataword_size 4 또는 8만 가능하도록

    vector<char> buffer((istreambuf_iterator<char>(input_file)), (istreambuf_iterator<char>()));
    input_file.close();

    string codewordBinaryString = "";

    for (char &byte : buffer) {
        bitset<8> byteBits(byte);
        string byteString = byteBits.to_string();

        for (int i = 0; i < 8; i += dataword_size) {
            string dataword = byteString.substr(i, dataword_size);
            string codeword = calculateCRC(dataword, generator);
            codewordBinaryString += codeword;
        }
    }

    //필요한 패딩 계산하기
    int padSize = (8 - (codewordBinaryString.length() % 8)) % 8;
    string padding = string(padSize, '0');
    codewordBinaryString = padding + codewordBinaryString;

    output_file.put(static_cast<char>(padSize)); 
    
    for (size_t i = 0; i < codewordBinaryString.size(); i += 8) {
        bitset<8> byte(codewordBinaryString.substr(i, 8));
        output_file.put(static_cast<char>(byte.to_ulong()));
    }

    output_file.close();

    return 0;
}
