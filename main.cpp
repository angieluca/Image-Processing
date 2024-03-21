#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct Header{
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;

    struct Pixel{
        //each BGR is one byte and then will be casted to int
        //values will be from 0-255
        unsigned char blue;
        unsigned char green;
        unsigned char red;
        //parameterized Pixel constructor
        Pixel(unsigned char blue, unsigned char green, unsigned char red){
            this->blue = blue;
            this->green = green;
            this->red = red;
        };
    };

    int num_pix;
    vector<Pixel> pix_vec;
    unsigned char temp_b;
    unsigned char temp_g;
    unsigned char temp_r;

    //default constructor
    Header(){}

    //parametrized Header constructor
    Header(ifstream& file){
        file.read((char*) &(this->idLength), sizeof(this->idLength));
        file.read((char*) &(this->colorMapType), sizeof(this->colorMapType));
        file.read((char*) &(this->dataTypeCode), sizeof(this->dataTypeCode));
        file.read((char*) &(this->colorMapOrigin), sizeof(this->colorMapOrigin));
        file.read((char*) &(this->colorMapLength), sizeof(this->colorMapLength));
        file.read((char*) &(this->colorMapDepth), sizeof(this->colorMapDepth));
        file.read((char*) &(this->xOrigin), sizeof(this->xOrigin));
        file.read((char*) &(this->yOrigin), sizeof(this->yOrigin));
        file.read((char*) &(this->width), sizeof(this->width));
        file.read((char*) &(this->height), sizeof(this->height));
        file.read((char*) &(this->bitsPerPixel), sizeof(this->bitsPerPixel));
        file.read((char*) &(this->imageDescriptor), sizeof(this->imageDescriptor));
        num_pix = width * height;

        //add pixels to vector of pixels
        for (int i = 0; i < num_pix; i++){
            file.read((char*) &temp_b, sizeof(temp_b));
            file.read((char*)&temp_g, sizeof(temp_g));
            file.read((char*)&temp_r, sizeof(temp_r));
            pix_vec.push_back(Pixel(temp_b, temp_g, temp_r)); //add pixel to the vector of pixels
        }
    };

    //print features function for debugging
    void print_ft(){
        cout << "idLength: " << int(this->idLength) << endl;
        cout << "colorMapType: " << int(this->colorMapType) << endl;
        cout << "dataTypeCode: " << int(this->dataTypeCode) << endl;
        cout << "colorMapOrigin: " << this->colorMapOrigin << endl;
        cout << "colorMapLength: " << this->colorMapLength << endl;
        cout << "colorMapDepth: " << int(this->colorMapDepth) << endl;
        cout << "xOrigin: " << this->xOrigin << endl;
        cout << "yOrigin: " << this->yOrigin << endl;
        cout << "width: " << this->width << endl;
        cout << "height: " << this->height << endl;
        cout << "bitsPerPixel: " << int(this->bitsPerPixel) << endl;
        cout << "imageDescriptor: " << int(this->imageDescriptor) << endl;
        cout << "number of pixels is: " << this->num_pix << endl;
    };

    void write_to(ofstream& file2){
        //write header data
        file2.write(&idLength, sizeof(idLength));
        file2.write(&colorMapType, sizeof(colorMapType));
        file2.write(&dataTypeCode, sizeof(dataTypeCode));
        file2.write((char*)&colorMapOrigin, sizeof(colorMapOrigin));
        file2.write((char*)&colorMapLength, sizeof(colorMapLength));
        file2.write(&colorMapDepth, sizeof(colorMapDepth));
        file2.write((char*)&xOrigin, sizeof(xOrigin));
        file2.write((char*)&yOrigin, sizeof(yOrigin));
        file2.write((char*)&width, sizeof(width));
        file2.write((char*)&height, sizeof(height));
        file2.write(&bitsPerPixel, sizeof(bitsPerPixel));
        file2.write(&imageDescriptor, sizeof(imageDescriptor));

        //write pixel data
        for (int i = 0; i < pix_vec.size(); i++){
            file2.write((char*)&pix_vec[i].blue, sizeof(pix_vec[i].blue));
            file2.write((char*)&pix_vec[i].green, sizeof(pix_vec[i].green));
            file2.write((char*)&pix_vec[i].red, sizeof(pix_vec[i].red));
        }
    };

};

//Algorithms
//P1 is top layer, P2 is bottom layer
//NP1 and NP2 are normalized values meaning P1/255 so between 0-1
//Multiply each pixel Method
unsigned char mult_pix(unsigned char P1, unsigned char P2){
    float NP1 = float(P1)/255;
    float NP2 = float(P2)/255;
    float newP = ((NP1 * NP2) * 255 ) + 0.5f;
    return (unsigned char)newP;
};
//Screen Method
unsigned char screen(unsigned char P1, unsigned char P2){
    float NP1 = float(P1)/255;
    float NP2 = float(P2)/255;
    float newP = ((1 - ((1 - NP1) * (1 - NP2))) * 255 ) + 0.5f;
    return (unsigned char)newP;
}
//Subtract Method
unsigned char subtract(unsigned char P1, unsigned char P2){
    unsigned char newP;
    if (P1 < P2){
        newP = (char)0;
    } else {
        P1 -= P2;
        newP = (char)P1;
    }
    return newP;
}
//Addition Method
unsigned char addition(unsigned char P1, unsigned char P2){
    int temp;
    if (int(P1) + int(P2) > 255){ //means overflow occurred
        temp = 255;
    } else {
        temp = P1 + P2;
    }
    return (char)temp;
}
//Overlay Method
unsigned char overlay(unsigned char P1, unsigned char P2){
    float NP1 = float(P1)/255;
    float NP2 = float(P2)/255;
    float final;
    if (NP2 <= 0.5){
        final = ((2 * NP1 * NP2) * 255) + 0.5f;
    } else {
        final = ((1 - (2 * (1 - NP1) * (1 - NP2))) * 255) + 0.5f;
    }
    return (unsigned char)final;
}

//Method for Task 1
void Task1(Header top, Header bottom, Header& temp){
    try {
        ofstream final("output/part1.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].blue = mult_pix(top.pix_vec[i].blue, bottom.pix_vec[i].blue);
            temp.pix_vec[i].green = mult_pix(top.pix_vec[i].green, bottom.pix_vec[i].green);
            temp.pix_vec[i].red = mult_pix(top.pix_vec[i].red, bottom.pix_vec[i].red);
        }
        temp.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task2(Header top, Header bottom, Header temp){
    try {
        ofstream final("output/part2.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].blue = subtract(top.pix_vec[i].blue, bottom.pix_vec[i].blue);
            temp.pix_vec[i].green = subtract(top.pix_vec[i].green, bottom.pix_vec[i].green);
            temp.pix_vec[i].red = subtract(top.pix_vec[i].red, bottom.pix_vec[i].red);
        }
        temp.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task3(Header top, Header bottom, Header temp, Header &final_img){
    Header top2 = top;
    try {
        ofstream final("output/part3.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            top2.pix_vec[i].blue = mult_pix(top.pix_vec[i].blue, bottom.pix_vec[i].blue);
            top2.pix_vec[i].green = mult_pix(top.pix_vec[i].green, bottom.pix_vec[i].green);
            top2.pix_vec[i].red = mult_pix(top.pix_vec[i].red, bottom.pix_vec[i].red);
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            final_img.pix_vec[i].blue = screen(top2.pix_vec[i].blue, temp.pix_vec[i].blue);
            final_img.pix_vec[i].green = screen(top2.pix_vec[i].green, temp.pix_vec[i].green);
            final_img.pix_vec[i].red = screen(top2.pix_vec[i].red, temp.pix_vec[i].red);
        }
        final_img.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task4(Header top, Header bottom, Header temp, Header& final_img){
    Header top2 = top;
    try {
        ofstream final("output/part4.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            top2.pix_vec[i].blue = mult_pix(top.pix_vec[i].blue, bottom.pix_vec[i].blue);
            top2.pix_vec[i].green = mult_pix(top.pix_vec[i].green, bottom.pix_vec[i].green);
            top2.pix_vec[i].red = mult_pix(top.pix_vec[i].red, bottom.pix_vec[i].red);
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            final_img.pix_vec[i].blue = subtract(top2.pix_vec[i].blue, temp.pix_vec[i].blue);
            final_img.pix_vec[i].green = subtract(top2.pix_vec[i].green, temp.pix_vec[i].green);
            final_img.pix_vec[i].red = subtract(top2.pix_vec[i].red, temp.pix_vec[i].red);
        }
        final_img.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task5(Header top, Header bottom, Header& temp){
    try {
        ofstream final("output/part5.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].blue = overlay(top.pix_vec[i].blue, bottom.pix_vec[i].blue);
            temp.pix_vec[i].green = overlay(top.pix_vec[i].green, bottom.pix_vec[i].green);
            temp.pix_vec[i].red = overlay(top.pix_vec[i].red, bottom.pix_vec[i].red);
        }
        temp.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task6(Header top, Header& temp){
    try {
        ofstream final("output/part6.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].green = addition(top.pix_vec[i].green,200);
            temp.pix_vec[i].blue = top.pix_vec[i].blue;
            temp.pix_vec[i].red = top.pix_vec[i].red;
        }
        temp.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task7(Header top, Header& temp){
    try {
        ofstream final("output/part7.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].blue = 0;
            temp.pix_vec[i].green = top.pix_vec[i].green;
            char P1 = top.pix_vec[i].red;
            if ((int)P1 * 4 >= 255){
                temp.pix_vec[i].red = (char)255;
            } else {
                int newP1 = (int)P1;
                newP1 *= 4;
                temp.pix_vec[i].red = (char)newP1;
            }
        }
        temp.write_to(final);
        final.close();
        cout << "Finished writing to new file";
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task8(Header top, Header& temp){
    try {
        ofstream final_r("output/part8_r.tga", ios_base::binary | ios::out | ios::trunc);
        ofstream final_g("output/part8_g.tga", ios_base::binary | ios::out | ios::trunc);
        ofstream final_b("output/part8_b.tga", ios_base::binary | ios::out | ios::trunc);

        //red channel
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].green = top.pix_vec[i].red;
            temp.pix_vec[i].blue = top.pix_vec[i].red;
            temp.pix_vec[i].red = top.pix_vec[i].red;
        }
        temp.write_to(final_r);
        final_r.close();

        //green channel
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].red = top.pix_vec[i].green;
            temp.pix_vec[i].blue = top.pix_vec[i].green;
            temp.pix_vec[i].green = top.pix_vec[i].green;
        }
        temp.write_to(final_g);
        final_g.close();

        //blue channel
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].red = top.pix_vec[i].blue;
            temp.pix_vec[i].green = top.pix_vec[i].blue;
            temp.pix_vec[i].blue = top.pix_vec[i].blue;
        }
        temp.write_to(final_b);
        final_b.close();

    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task9(Header b1, Header g1, Header r1, Header& temp){
    try {
        ofstream final("output/part9.tga", ios_base::binary | ios::out | ios::trunc);

        for (int i = 0; i < temp.pix_vec.size(); i++) {
            temp.pix_vec[i].green = g1.pix_vec[i].green;
            temp.pix_vec[i].blue = b1.pix_vec[i].blue;
            temp.pix_vec[i].red = r1.pix_vec[i].red;
        }
        temp.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

void Task10(Header top, Header& temp){
    try {
        ofstream final("output/part10.tga", ios_base::binary | ios::out | ios::trunc);
        if (!final.is_open()){
            throw runtime_error("New file not created :( ");
        }
        reverse(top.pix_vec.begin(), top.pix_vec.end());
        for (int i = 0; i < top.pix_vec.size(); i++) {
            temp.pix_vec[i].green = top.pix_vec[i].green;
            temp.pix_vec[i].blue = top.pix_vec[i].blue;
            temp.pix_vec[i].red = top.pix_vec[i].red;
        }
        temp.write_to(final);
        final.close();
    } catch (exception& e){
        cerr << e.what() << endl;
    }
}

int main() {
//    string input;
//    cout << "Input the file thing: " << endl;
//    getline(cin, input); //get file name from user
//    cout << input << endl;
//
//    ifstream in_file(input, ios_base::binary);
//    if (in_file.is_open()){
//        cout << "File opened successfully" << endl;
//
//        Header ex_file = Header(in_file);
//        in_file.close();
//
//        ex_file.print_ft();
//        cout << "Size of vector of pixels is: " << ex_file.pix_vec.size() << endl;
//        cout << "Should be " << ex_file.num_pix << endl;
//        cout << "You have completed reading the file" << endl;
//
//        try {
//            ofstream out_file("output/writing_ex.tga", ios_base::binary | ios::out | ios::trunc);
//            if (!out_file.is_open()){
//                throw runtime_error("New file not created :( ");
//            }
//            ex_file.write_to(out_file);
//            out_file.close();
//            cout << "Finished writing to new file";
//        } catch (exception& e){
//            cerr << e.what() << endl;
//        }
//
//    } else {
//        cout << "File not found :( " << endl;
//        return 1;
//    }

    //Create all the Header objects
    ifstream input11("input/layer1.tga", ios_base::binary);
    Header lay1 = Header(input11);
    ifstream input12("input/pattern1.tga", ios_base::binary);
    Header pat1 = Header(input12);
    ifstream input21("input/layer2.tga", ios_base::binary);
    Header lay2 = Header(input21);
    ifstream input22("input/car.tga", ios_base::binary);
    Header car = Header(input22);
    ifstream input32("input/pattern2.tga", ios_base::binary);
    Header pat2 = Header(input32);
    ifstream input33("input/text.tga", ios_base::binary);
    Header text = Header(input33);
    ifstream input42("input/circles.tga", ios_base::binary);
    Header circles = Header(input42);
    ifstream input91("input/layer_red.tga", ios_base::binary);
    Header lay_red = Header(input91);
    ifstream input92("input/layer_green.tga", ios_base::binary);
    Header lay_gr = Header(input92);
    ifstream input93("input/layer_blue.tga", ios_base::binary);
    Header lay_bl = Header(input93);
    ifstream input101("input/text2.tga", ios_base::binary);
    Header text2 = Header(input101);

    //Task 1: Multiply to combine layer1.tga (top) with pattern1.tga (bottom)
    Header temp = lay1;
    Header temp2 = lay1;
    Header temp3 = lay1;
    Header temp4 = lay1;
    Header temp5 = lay1;
    Header temp6 = lay1;
    Header temp7 = lay1;
    Header temp8 = lay1;
    Header temp9 = lay_red;
    Header temp10 = text2;
    Task1(lay1,pat1,temp);
    //Task 2: Subtract layer2 (bottom) from car (top)
    Task2(car,lay2,temp2);
    //Task3: Multiply lay1 and pat2 to become top and use screen with text (bottom)
    Task3(lay1, pat2, text, temp3);
    //Task4: Multiply lay2 with circles. Load pat2 (bottom) and prev (top) with subtract pat2 (bottom)
    Task4(lay2, circles, pat2, temp4);
    //Task5: Use overlay with lay1 (top) and pat1 (bot)
    Task5(lay1,pat1,temp5);
    //Task6: add 20 to green of car
    Task6(car,temp6);
    //Task7: Multiply car red by 4, blue by 0
    Task7(car,temp7);
    //Task8:write car red to part8_r, green to part8_g, blue to part8_b
    Task8(car,temp8);
    //Task9: Load lay_red, lay_g, lay_b and combine into 1
    Task9(lay_bl,lay_gr,lay_red,temp9);
    //Task10: Load text2 and rotate 180
    Task10(text2,temp10);

    return 0;
}
