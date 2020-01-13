#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <cmath>

using namespace std;
typedef unsigned char uchar;
//Write floating point number as 4 byte into file
void outbitstream(float Converted_Float, ofstream &fout){
    uchar* outputarray = new uchar [4];
    *((float *)outputarray) = Converted_Float;
    for (int i = 3; i >= 0 ; i --){
        fout << outputarray[i];
    }
    return;
}

//Convert Binary Representation to Decimal
void Binary_to_Decimal(int *BinaryHolder, int &FirstNibbleInteger, int &SecondNibbleInteger){
    int answer = 0;
    int power = 0;
    for (int i = 3; i >= 1; i--){
        int temp = pow (2.0, power);
        power++;
        answer += (BinaryHolder[i] * temp);
    }
    if (BinaryHolder[0] == 1){
        FirstNibbleInteger = answer * -1;
    }
    else if (BinaryHolder[0] == 0){
        FirstNibbleInteger = answer;
    }
    answer = 0;
    power = 0;
    for (int i = 7; i >= 5; i--){
        int temp = pow(2.0, power);
        power++;
        answer += (BinaryHolder[i] * temp);
    }
    if (BinaryHolder[4] == 1){
        SecondNibbleInteger = answer * -1;
    }
    else if (BinaryHolder[4] == 0){
        SecondNibbleInteger = answer;
    }
    return;

}
//Convert one byte into a integer for each nibble in the byte
void Nibble_to_Integer(uchar character, int &FirstNibbleInteger, int &SecondNibbleInteger){
    int index = 0;
    int n = 7;
    int *BinaryHolder = new int[8];
    for (int i = 0; i < 8; i++){
        int next = ((character & (0x01 << n)) >> n);
        BinaryHolder[i] = next;
        n--;
    }
    Binary_to_Decimal(BinaryHolder, FirstNibbleInteger, SecondNibbleInteger);
    delete[] BinaryHolder;
    return;

}
void BRRdecoding (ifstream &fin, ofstream &fout){
    uchar* BytesStorage = new uchar [9];
    int16_t* Converted_Integer = new int16_t [16];
    float* Converted_Float = new float [16];
    int Header_Range, FirstNibbleInteger, SecondNibbleInteger = 0;
    //Read in 9 bytes from the file
    for (int i = 0; i < 9; i++){
        BytesStorage[i] = fin.get();
    }
    //Convert Nibble to Integer for header and chunk
    Nibble_to_Integer(BytesStorage[0], Header_Range, SecondNibbleInteger);
    SecondNibbleInteger = 0;
    int index = 0;
    for (int i = 1; i < 9; i++){
        Nibble_to_Integer(BytesStorage[i], FirstNibbleInteger, SecondNibbleInteger);
        Converted_Integer[index++] = FirstNibbleInteger;
        Converted_Integer[index++] = SecondNibbleInteger;
    }
    
    for (int i = 0; i < 16; i++){
        Converted_Integer[i] = Converted_Integer[i] * (pow(2.0,(Header_Range)));
        Converted_Float[i] = static_cast < float> (Converted_Integer[i]) / pow(2.0,15.0);
        outbitstream (Converted_Float[i], fout);
    }
    delete [] Converted_Integer;
    delete [] Converted_Float;
    delete [] BytesStorage;
    return;
}


int main(int argc, char *argv[])
{

    /////////////////////////////////// Input and Output //////////////////////////////////////////
    //Input File Name & Output File Name
    string InputFileName = "\0";
    string OutputFileName = "\0";
    int length = 0;

    //Check for Number of Argument Passed to the Command Line Switch
    if (argc != 3){
        cout << "Invalid Argument" << endl;
        return -1;
    }

    // Move the 1st and 2nd Parameter passed by Command Line Switch to FileName
    InputFileName = argv[1];
    OutputFileName = argv[2];

    ifstream fin(argv[1], ifstream::binary);
    ofstream fout(argv[2], ofstream::binary);

    // Check whether the file is succesfully open
    if (fin.fail()){
        cout << "Error opening file. Please try again." << endl;
    }
    else{
        fin.seekg(0, fin.end);
        //Initialize length
        length = fin.tellg();
        //Point to the first element in the file
        fin.seekg(0, fin.beg);
    }

    cout <<"The file is being decoded..."<<endl;
    //BRR Decoding Stage
    int index = 0;
    
    while (index <= length){
        cout << "";
        //Check for corner case: At the end of the file
        index = index + 9;
        if (index > length){
            break;
        }
        BRRdecoding(fin, fout);
    }
    cout << "Decoding Completed!" << endl;
    return 0;
}