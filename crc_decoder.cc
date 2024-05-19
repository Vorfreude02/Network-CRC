#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
using namespace std;

string calculateCRC(string codeword, string generator) {
    int cLen = codeword.length(); //codeword 길이
    int gLen = generator.length(); //generator 길이
    string appendedData = codeword; 

    for (int i = 0; i <= cLen - gLen; ++i) {
        if (appendedData[i] == '1') {
            for (int j = 0; j < gLen; ++j) {
                appendedData[i + j] = (appendedData[i + j] == generator[j] ? '0' : '1');
            }
        }
    }

    return appendedData.substr(cLen - gLen + 1);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        cout << "usage: " << argv[0] << " input_file output_file result_file generator dataword_size" << endl;
        return 1;
    } // 조건(1) 인자 5개 받고 아닐 경우 메시지 출력

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

    ofstream result_file(argv[3]);
    if (!result_file) {
        cout << "result file open error." << endl;
        return 1;
    } // 조건(4) result_file 열기 argv[3]
 
    string generator = argv[4];
    int dataword_size = atoi(argv[5]);
    if (dataword_size!= 4 && dataword_size!= 8) {
        cout << "dataword size must be 4 or 8." << endl;
        return 1;
    } // 조건(5) dataword_size 4 또는 8만 가능하도록 argv[4] argv[5]

    vector<char> buffer((istreambuf_iterator<char>(input_file)), (istreambuf_iterator<char>()));
    input_file.close();

    if (buffer.empty()) {
        result_file << "0 0" << endl;
        return 0;
    }

    int padSize = static_cast<unsigned char>(buffer.front()); //첫 번째 바이트에서 읽기
    buffer.erase(buffer.begin()); //패딩 바이트 제거하기

    string binaryString = "";
    for (char &byte : buffer) {
        bitset<8> byteBits(byte);
        binaryString += byteBits.to_string();
    }

    binaryString = binaryString.substr(padSize);//패딩 비트 제거하기

    int totalFrames = 0, errorFrames = 0;
    for (size_t i = 0; i < binaryString.length(); i += dataword_size + generator.length() - 1) {
        string codeword = binaryString.substr(i, dataword_size + generator.length() - 1);

        if (codeword.length() < dataword_size + generator.length() - 1)
            break;

        string remainder = calculateCRC(codeword, generator);
        if (remainder.find('1') != string::npos) errorFrames++; //나머지o->오류

        //dataword 출력하기
        string dataword=codeword.substr(0, dataword_size);
        bitset<8> byte(std::stoi(dataword, 0, 2));
        output_file.put(static_cast<char>(byte.to_ulong()));

        totalFrames++;
    }

    result_file << totalFrames << " " << errorFrames << endl;

    output_file.close();
    result_file.close();

    return 0;
}
