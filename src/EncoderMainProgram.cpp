#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;
typedef unsigned char uchar;

//Read in a floating number and convert it to 16bit integer
int16_t &read_in_32bits(ifstream &fin, int16_t &converted_integer_16)
{
    unsigned char float_number_pointer[4];
    float output;
    // Read in 32 bits from file 
    for (int j = 0; j < 4; j++){
        float_number_pointer[j] = fin.get();
    }

    // Combine 4 bytes into a 32bit float
    *((uchar *)(&output) + 3) = float_number_pointer[0];
    *((uchar *)(&output) + 2) = float_number_pointer[1];
    *((uchar *)(&output) + 1) = float_number_pointer[2];
    *((uchar *)(&output) + 0) = float_number_pointer[3];

    // Convert 32bit float to 16 bit int
    converted_integer_16 = static_cast <int16_t> (output * pow(2.0,15.0));
    return converted_integer_16;
}

void read_In_64bytes (ifstream &fin, int16_t* integer_array){
    int16_t Bit16Integer = 0;
    int16_t converted_integer_16 = 0;

    //Read In 64 bytes a Chunk and put the 16bit integer value into the integer array
    for (int i = 0; i < 16; i++){
        converted_integer_16 = read_in_32bits(fin, Bit16Integer);
        integer_array[i] = converted_integer_16;
    }
    return;
}

//Calculate max of the chunk
int& maxof64bytes (int16_t* integer_array, int& max){
    max = integer_array[0];
    for (int i = 1; i < 16; i++){
        if (integer_array[i] > max){
            max = integer_array[i];
        }
    }
    return max;
}

//Calculate min of the chunk
int &minof64bytes(int16_t *integer_array, int &min){
    min = integer_array[0];
    for (int i = 1; i < 16; i++){
        if (integer_array[i] < min){
            min = integer_array[i];
        }
    }
    return min;
}

//Print the binary representation into the file
void outBitStream(int * binaryholder, ofstream &fout, char &outBuffer){
    int index = 0;
    outBuffer = 0;
    for (int i = 7; i >= 0; i--){
        if (binaryholder[index] == 0){
            index++;
            char divider = 1;
            outBuffer = (0x00 << i) | outBuffer;
        }
        else if(binaryholder[index] == 1){
            index++;
            char divider = 1;
            outBuffer = (0x01 << i) | outBuffer;
        }
        else{
            cout<<"problem"<<endl;
        }
    }
    fout << outBuffer;
    return;
}

//Write the header into the file
void writeheadertofile(int range, ofstream &fout)
{
    int outNumbits = 0;
    char outBuffer;
    int* binaryrepresentation = new int [8];
    //Write the binary representation of the range into the array
    while (range != 0){
        binaryrepresentation[3 - outNumbits] = range % 2;
        outNumbits++;
        range /= 2;
    }
    while ((3 - outNumbits) >= 0){
        binaryrepresentation[3 - outNumbits] = 0;
        outNumbits++;
    }
    outNumbits = 0;
    while ((7 - outNumbits) >= 4){
        binaryrepresentation[7 - outNumbits] = 0;
        outNumbits++;
    }
    //Print the binary representation of the range into the file
    outBitStream(binaryrepresentation, fout, outBuffer);
    delete[] binaryrepresentation;
}


void RangeConversion (int range, int16_t* integertempholder, int* binaryholder,int outNumbits, int value, int index, char outBuffer, ofstream &fout){
    index = 0;
    outNumbits = 0;
    for (int i = 0; i < 8; i++){
        outNumbits = 0;
        integertempholder[index] = integertempholder[index] / range;
        value = abs(integertempholder[index]);
        if (integertempholder[index] >= 0){
            binaryholder[0] = 0;
        }
        else{
            binaryholder[0] = 1;
        }
        while (value != 0){
            binaryholder[3 - outNumbits] = value % 2;
            value /= 2;
            outNumbits++;
        }
        while ((3 - outNumbits) >= 1){
            binaryholder[3 - outNumbits] = 0;
            outNumbits++;
        }
        index++;
        integertempholder[index] = integertempholder[index] / range;
        value = abs(integertempholder[index]);
        if (integertempholder[index] >= 0){
            binaryholder[4] = 0;
        }
        else{
            binaryholder[4] = 1;
        }
        outNumbits = 0;
        while (value != 0){
            binaryholder[7 - outNumbits] = value % 2;
            value /= 2;
            outNumbits++;
        }
        while ((7 - outNumbits) >= 5){
            binaryholder[7 - outNumbits] = 0;
            outNumbits++;
        }
        index++;
        outBitStream(binaryholder, fout, outBuffer);
    }
    return;
}

// BRR Encoding Stage
void BRRencoding (ifstream &fin, ofstream &fout){
    //Store the 16 samples (16bits integers) of the chuck
    int16_t *integertempholder = new int16_t[16];
    // Holds the binary representation of 2 Nibble
    int *binaryholder = new int[8];
    // Helper Variables
    int outNumbits, value, index = 0;
    char outBuffer;

    // Read in 64 bytes a chunk
    read_In_64bytes(fin, integertempholder);

    // Find min and max of the chunk
    int max = maxof64bytes(integertempholder, max);
    int min = minof64bytes(integertempholder, min);
    //Range Table (Range 0 - 12)
    if ((max < 7) && (min > -8)){
        writeheadertofile(0, fout);
        RangeConversion (1, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 14) && (min > -16)){
        writeheadertofile(1, fout);
        RangeConversion(2, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 28) && (min > -32)){
        writeheadertofile(2, fout);
        RangeConversion(4, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 56) && (min > -64)){
        writeheadertofile(3, fout);
        RangeConversion(8, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 112) && (min > -128)){
        writeheadertofile(4, fout);
        RangeConversion(16, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 224) && (min > -256)){
        writeheadertofile(5, fout);
        RangeConversion(32, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 448) && (min > -512)){
        writeheadertofile(6, fout);
        RangeConversion(64, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 896) && (min > -1024)){
        writeheadertofile(7, fout);
        RangeConversion(128, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 1792) && (min > -2048)){
        writeheadertofile(8, fout);
        RangeConversion(256, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 3584) && (min > -4096)){
        writeheadertofile(9, fout);
        RangeConversion(512, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 7168) && (min > -8192)){
        writeheadertofile(10, fout);
        RangeConversion(1024, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 14336) && (min > -16384)){
        writeheadertofile(11, fout);
        RangeConversion(2048, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    else if ((max < 28672) && (min > -32768)){
        writeheadertofile(12, fout);
        RangeConversion(4096, integertempholder, binaryholder, outNumbits, value, index, outBuffer, fout);
    }
    delete[] integertempholder;
    delete[] binaryholder;

    return;
}


int main(int argc, char* argv[]){
    //assert(CHAR_BIT * sizeof(float) == 32);
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

    ifstream fin (argv[1], ifstream::binary);
    ofstream fout (argv[2], ofstream::binary);

    // Check whether the file is succesfully open
    if (fin.fail()){
        cout << "Error opening file. Please try again." << endl;
        return -1;
    }

    // Get the length of the file
    else{
        fin.seekg(0, fin.end);
        //Initialize length
        length = fin.tellg();
        //Point to the first element in the file
        fin.seekg(0, fin.beg);
    }

    cout << "The file is being encoded..." << endl;
    //BRR Encoding Stage
    int index = 0;
    while (index <= length){
        index = index + 64;
        // Check for corner case: At the end of the file
        if (index > length){
            break;
        }
        BRRencoding(fin, fout);
    }
    cout << "Encoding Completed!" << endl;
    fin.close();
    fout.close();
    return 0;
}