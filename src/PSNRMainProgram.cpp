#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;
typedef unsigned char uchar;

// Read in the new floating point from the file
void FloatingPointReader (ifstream &fin, float &FloatnumberValue){
    unsigned char float_number_pointer[4];
    // Read in 32 bits from file
    for (int j = 0; j < 4; j++){
        float_number_pointer[j] = fin.get();
    }
    // Combine 4 bytes into a 32bit float
    *((uchar *)(&FloatnumberValue) + 3) = float_number_pointer[0];
    *((uchar *)(&FloatnumberValue) + 2) = float_number_pointer[1];
    *((uchar *)(&FloatnumberValue) + 1) = float_number_pointer[2];
    *((uchar *)(&FloatnumberValue) + 0) = float_number_pointer[3];
    return;
}

void PSNR (ifstream &fin1, ifstream &fin2, int size, float &PSNR_Result){
    int index = 0;
    float OriginalValue, PredictedValue,result = 0.0;
    // Calculate the summation of (OriginalValue - PredictedValue)^2 for the whole file
    while (index < size){
        FloatingPointReader (fin1, OriginalValue);
        FloatingPointReader (fin2, PredictedValue);
        result += pow((OriginalValue - PredictedValue),2);
        index++;
    }
    result = result / size;
    //Calculate MAX of the Signal which is 2^32-1 for 32 bit floating number 
    int64_t MAXSIZE32bit = (pow (2,32)) - 1;
    //Calculate the MAX/MSE^2
    double SQRT_MSE = sqrt (result);
    double MAX_MSESquared = MAXSIZE32bit / SQRT_MSE;
    //Calculate 20 * log10(MAX/MSE^2)
    MAX_MSESquared = log10(MAX_MSESquared);
    PSNR_Result = 20 * MAX_MSESquared;
    return;
}

int main(int argc, char *argv[]){
    /////////////////////////////////// Input and Output //////////////////////////////////////////
    //Input File Name & Output File Name
    string InputFile1Name = "\0";
    string InputFile2Name = "\0";
    int max_of_audio1, max_of_audio2, length1file, length2file, size = 0;

    //Check for Number of Argument Passed to the Command Line Switch
    if (argc != 3){
       cout << "Invalid Argument" << endl;
       return -1;
    }

    InputFile1Name = argv[1];
    InputFile2Name = argv[2];

    ifstream fin1file (argv[1], ifstream::binary);
    ifstream fin2file (argv[2], ifstream::binary);

    //Calculating size of file1
    if (fin1file.fail()){
        cout << "Error opening file. Please try again." << endl;
    }
    else{
        fin1file.seekg(0, fin1file.end);
        //Initialize length
        length1file = fin1file.tellg();
        //Point to the first element in the file
        fin1file.seekg(0, fin1file.beg);
    }

    //Calculating size of file2
    if (fin2file.fail()){
        cout << "Error opening file. Please try again." << endl;
    }
    else{
        fin2file.seekg(0, fin2file.end);
        //Initialize length
        length2file = fin2file.tellg();
        //Point to the first element in the file
        fin2file.seekg(0, fin2file.beg);
    }

    //If 2 file have diff, return an exception
    if (length1file != length2file){
        cout << "Length of both files are not equal! Please try again!" << endl;
        return -1;
    }

    // Set the the amount of floating point in the file to the size of the file
    size = length1file / 4;
    float PSNR_Result;
    // Calculate the PSNR
    cout << "Calculating PSNR..." << endl;
    PSNR (fin1file,fin2file,size,PSNR_Result);
    cout << "PSNR of " << argv[1] <<" and " << argv[2] << ": " << PSNR_Result << endl; 
    fin1file.close();
    fin2file.close();
    return 0;
}