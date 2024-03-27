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

//Algorithms: P1 is top layer, P2 is bottom layer
//NP1 and NP2 are normalized values meaning P1/255 so between 0-1
//Multiply each pixel Method
unsigned char mult_pix(unsigned char P1, unsigned char P2){
    float NP1 = (float)P1/(float)255;
    float NP2 = (float)P2/(float)255;
    float newP = ((NP1 * NP2) * 255) + 0.5f;
    return (unsigned char)newP;
}
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
char addition(char P1, char P2){
    int temp;
    if (int(P1) + int(P2) > 255){ //means overflow occurred
        temp = 255;
    } else if (int(P1) + int(P2) < 0){ //we accept negative amount, but least pixel val is 0
        temp = 0;
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
            unsigned char P1 = top.pix_vec[i].red;
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

//check if input file exists
bool in_name_check(string &name){
    //check if ends with ".tga"
    if (name.length() > 4){
        string last4 = name.substr(name.length() - 4,4);
        if (last4 == ".tga") {
            //check if starts with input/, if not, then add it
            //if (name.substr(0, 6) != "input/") name = "input/" + name;
            //check if file exists in input folder
            ifstream file(name, ios_base::binary);
            if (!file.is_open()) {
                cout << "Invalid argument, file does not exist.";
                file.close();
                return false;
            }
            cout << name << " does exist!!!" << endl;
            file.close();
            return true;
        } else {
            cout << "Invalid file name.";
            return false;
        }
    } else {
        cout << "Invalid file name."; //too short and .tga doesn't exist
        return false;
    }
}
//check if output file exists
bool out_name_check(string &name){
    //check if ends with ".tga"
    if (name.length() > 4){
        string last4 = name.substr(name.length() - 4,4);
        if (last4 != ".tga")
        {
            //check if starts with output/, if not, then add it
            //if (name.substr(0, 7) != "output/") name = "output/" + name;
            //check if file exists is opened correctly
            ofstream file(name, ios_base::binary | ios::out | ios::trunc);
            if (!file.is_open()){
                cout << name << " file not created :( ";
                return false;
            } else {
                cout << name << " does exist!!!" << endl;
                file.close();
                return true;
            }
        } else {
            cout << "Invalid file name.";
            return false;
        }
    } else {
        cout << "Invalid file name."; //too short and .tga doesn't exist
        return false;
    }
}

int main(int argc, const char* argv[]) {

    string help_msg = "Project 2: Image Processing, Spring 2024\n\nUsage:\n\t./project2.out [output] [firstImage] [method] [...]";
    string method_names = "multiply subtract overlay screen combine flip onlyred onlygreen onlyblue addred addgreen addblue scalered scalegreen scaleblue";
    string out_name = "";
    string track_name = "";
    string bottom_name = "";
    string top_name = "";
    string bl_name = "";
    string gr_name = "";
    string rd_name = "";
    int position = 0;

    //no arguments or only --help --> print help message
    if (argc == 1 ){
        cout << help_msg;
        position = 1;
    } else if (argc == 2 && (string)argv[1] == "--help"){
        cout << help_msg;
        position = 2;
    } else if (argc == 2){
        //2nd argument not provided
        cout << "Invalid file name.";
        position = 2;
    } else if (argc > 2) {
        //check if 1st argument is valid output file name
        out_name = (string)argv[1];
        if (!out_name_check(out_name)) {
            position = argc;
        }
        //check if 2nd argument is valid file name
        track_name = (string)argv[2];
        if (!in_name_check(track_name)) {
            position = argc;
        }
    }

    //3rd argument (manipulation method) not provided
    if (argc == 3){
        cout << "Invalid method name.";
        position = 3;
    }

    //tracking img is top layer
    Header tracking_img;
    ifstream tracking_file(track_name, ios_base::binary);
    tracking_img = Header(tracking_file);
    //bottom layer for manipulations
    Header bottom_img;
    //top layer for manipulation (not always used)
    Header top_img;
    Header rd_img;
    Header gr_img;
    Header bl_img;

    //while loop that goes through index position starting at 3rd argument
    position = 3;
    string method = "";
    string flexible = ""; //flexible file name for checking validity
    while (position < argc){

        cout << "\nPosition is at: " << position << endl;
        //3rd argument should be method name
        method = (string)argv[position];
        if (method_names.find(method) == -1){
            cout << "Invalid method name.";
            position = argc;
            break;

        } else if (method == "multiply") {
            //multiply method, requires one more additional argument
            cout << "They said multiply!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                //check if file name valid
                position += 1;
                flexible = (string) argv[position];
                if (!in_name_check(flexible)) {
                    position = argc;
                    break;
                } else {
                    //test passed and can perform multiply
                    //get bottom layer of multiply (NOT tracking)
                    bottom_name = (string) argv[position];
                    ifstream bottom(bottom_name, ios_base::binary);
                    bottom_img = Header(bottom);
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].blue = mult_pix(tracking_img.pix_vec[i].blue,
                                                                bottom_img.pix_vec[i].blue);
                        tracking_img.pix_vec[i].green = mult_pix(tracking_img.pix_vec[i].green,
                                                                 bottom_img.pix_vec[i].green);
                        tracking_img.pix_vec[i].red = mult_pix(tracking_img.pix_vec[i].red, bottom_img.pix_vec[i].red);
                    }
                    cout << "finished multiplying";
                    position += 1;
                }
            }

        } else if (method == "subtract") {
            //subtract method, requires one more additional argument
            cout << "They said subtract!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                //check if file name valid
                position += 1;
                flexible = (string) argv[position];
                if (!in_name_check(flexible)) {
                    position = argc;
                    break;
                } else {
                    //test passed and can perform subtract
                    //get bottom layer of subtract (NOT tracking)
                    bottom_name = (string) argv[position];
                    ifstream bottom(bottom_name, ios_base::binary);
                    bottom_img = Header(bottom);
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].blue = subtract(tracking_img.pix_vec[i].blue,
                                                                bottom_img.pix_vec[i].blue);
                        tracking_img.pix_vec[i].green = subtract(tracking_img.pix_vec[i].green,
                                                                 bottom_img.pix_vec[i].green);
                        tracking_img.pix_vec[i].red = subtract(tracking_img.pix_vec[i].red, bottom_img.pix_vec[i].red);
                    }
                    cout << "finished subtracting";
                    position += 1;
                }
            }

        } else if (method == "overlay") {
            //overlay method, requires one more additional argument
            cout << "They said overlay!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                //check if file name valid
                position += 1;
                flexible = (string) argv[position];
                if (!in_name_check(flexible)) {
                    position = argc;
                    break;
                } else {
                    //test passed and can perform overlay
                    //get bottom layer of overlay (NOT tracking)
                    bottom_name = (string) argv[position];
                    ifstream bottom(bottom_name, ios_base::binary);
                    bottom_img = Header(bottom);
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].blue = overlay(tracking_img.pix_vec[i].blue,
                                                               bottom_img.pix_vec[i].blue);
                        tracking_img.pix_vec[i].green = overlay(tracking_img.pix_vec[i].green,
                                                                bottom_img.pix_vec[i].green);
                        tracking_img.pix_vec[i].red = overlay(tracking_img.pix_vec[i].red,
                                                              bottom_img.pix_vec[i].red);
                    }
                    cout << "finished overlaying";
                    position += 1;
                }
            }

        } else if (method == "screen") {
            //overlay method, requires one more additional argument
            cout << "They said screen!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                //check if file name valid
                position += 1;
                flexible = (string) argv[position];
                if (!in_name_check(flexible)) {
                    position = argc;
                    break;
                } else {
                    //test passed and can perform screen
                    //get top layer of screen (THIS TIME IT ISN"T TRACKING)
                    top_name = (string) argv[position];
                    ifstream topper(top_name, ios_base::binary);
                    top_img = Header(topper);
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].blue = screen(top_img.pix_vec[i].blue,
                                                               tracking_img.pix_vec[i].blue);
                        tracking_img.pix_vec[i].green = screen(top_img.pix_vec[i].green,
                                                                tracking_img.pix_vec[i].green);
                        tracking_img.pix_vec[i].red = screen(top_img.pix_vec[i].red,
                                                              tracking_img.pix_vec[i].red);
                    }
                    cout << "finished screen";
                    position += 1;
                }
            }

        } else if (method == "combine"){
            //combine method, requires 2 more arguments (green, then blue)
            //tracking is red channel
            cout << "They said combine!!" << endl;
            if (position + 2 >= argc) { //doesn't provide 2 additional arguments
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                //check if first (green) file name valid
                position += 1;
                flexible = (string) argv[position];
                if (!in_name_check(flexible)) {
                    position = argc;
                    break;
                }
                //check if second (blue) file name valid
                position += 1;
                flexible = (string) argv[position];
                if (!in_name_check(flexible)) {
                    position = argc;
                    break;
                } else {
                    //both files passed and can perform combine
                    //tracking is red, first is green, second is blue
                    track_name = (string) argv[position - 3];
                    ifstream tracking(track_name, ios_base::binary);
                    tracking_img = Header(tracking);
                    gr_name = (string) argv[position - 1]; //first additional arg is green
                    ifstream green(gr_name, ios_base::binary);
                    gr_img = Header(green);
                    bl_name = (string) argv[position]; //second additional arg is blue
                    ifstream blue(bl_name, ios_base::binary);
                    bl_img = Header(blue);
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].blue = bl_img.pix_vec[i].blue;
                        tracking_img.pix_vec[i].green = gr_img.pix_vec[i].green;
                        //don't change tracking_img.pix_vec[i].red                                                              tracking_img.pix_vec[i].red);
                    }
                    cout << "finished combine";
                    position += 1;
                }
            }

        } else if (method == "flip"){
            //takes no additional arguments
            Header temp = tracking_img;
            reverse(temp.pix_vec.begin(), temp.pix_vec.end());
            for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                tracking_img.pix_vec[i].green = temp.pix_vec[i].green;
                tracking_img.pix_vec[i].blue = temp.pix_vec[i].blue;
                tracking_img.pix_vec[i].red = temp.pix_vec[i].red;
            }
//            tracking_img.write_to(out_file);
//            out_file.close();
            cout << "Done flipping! ";
            position++;

        } else if (method == "onlyred"){
            //takes no additional arguments
            //tracking_img becomes only red channel
            for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                tracking_img.pix_vec[i].green = tracking_img.pix_vec[i].red;
                tracking_img.pix_vec[i].blue = tracking_img.pix_vec[i].red;
            }
            cout << "Done making it only red channel! ";
            position++;

        } else if (method == "onlygreen"){
            //takes no additional arguments
            //tracking_img becomes only green channel
            for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                tracking_img.pix_vec[i].blue = tracking_img.pix_vec[i].green;
                tracking_img.pix_vec[i].red = tracking_img.pix_vec[i].green;
            }
            cout << "Done making it only green channel! ";
            position++;

        } else if (method == "onlyblue"){
            //takes no additional arguments
            //tracking_img becomes only blue channel
            for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                tracking_img.pix_vec[i].green = tracking_img.pix_vec[i].blue;
                tracking_img.pix_vec[i].red = tracking_img.pix_vec[i].blue;
            }
            cout << "Done making it only blue channel! ";
            position++;

        } else if (method == "addred") {
            //addred method, requires one more additional argument (amount to add to red channel)
            cout << "They said addred!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                position += 1;
                string flex = argv[position];
                int flexy;
                try {
                    flexy = stoi(flex);
                } catch (invalid_argument& e){
                    cout << "Invalid argument, expected number.";
                    position = argc;
                    break;
                }
                for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                    if ((int)tracking_img.pix_vec[i].red + flexy > 255){
                        tracking_img.pix_vec[i].red = 255;
                    } else if ((int)tracking_img.pix_vec[i].red + flexy < 0){
                        tracking_img.pix_vec[i].red = 0;
                    } else {
                        tracking_img.pix_vec[i].red = char((int)tracking_img.pix_vec[i].red + flexy);
                    }
                }
                cout << "finished addred";
                position += 1;
            }

        } else if (method == "addgreen") {
            //addgreen method, requires one more additional argument (amount to add to green channel)
            cout << "They said addgreen!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                position += 1;
                string flex = argv[position];
                int flexy;
                try {
                    flexy = stoi(flex);
                } catch (invalid_argument& e){
                    cout << "Invalid argument, expected number.";
                    position = argc;
                    break;
                }
                for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                    if ((int)tracking_img.pix_vec[i].green + flexy > 255){
                        tracking_img.pix_vec[i].green = 255;
                    } else if ((int)tracking_img.pix_vec[i].green + flexy < 0){
                        tracking_img.pix_vec[i].green = 0;
                    } else {
                        tracking_img.pix_vec[i].green = char((int)tracking_img.pix_vec[i].green + flexy);
                    }
                }
                cout << "finished addgreen";
                position += 1;
            }

        } else if (method == "addblue") {
            //addblue method, requires one more additional argument (amount to add to red channel)
            cout << "They said addblue!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                position += 1;
                string flex = argv[position];
                int flexy;
                try {
                    flexy = stoi(flex);
                } catch (invalid_argument& e){
                    cout << "Invalid argument, expected number.";
                    position = argc;
                    break;
                }
                for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                    if ((int)tracking_img.pix_vec[i].blue + flexy > 255){
                        tracking_img.pix_vec[i].blue = 255;
                    } else if ((int)tracking_img.pix_vec[i].blue + flexy < 0){
                        tracking_img.pix_vec[i].blue = 0;
                    } else {
                        tracking_img.pix_vec[i].blue = char((int)tracking_img.pix_vec[i].blue + flexy);
                    }
                }
                cout << "finished addblue";
                position += 1;
            }

        } else if (method == "scalered"){
            //scalered method, requires one more additional argument
            cout << "They said scalered!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                position += 1;
                string flex = argv[position];
                int flexy;
                try {
                    cout << flex << endl;
                    flexy = stoi(flex);
                    cout << flexy << "is of type: " << typeid(flexy).name() << endl;
                } catch (invalid_argument &e) {
                    cout << "Invalid argument, expected number.";
                    position = argc;
                    break;
                }
                if (flexy < 0) {
                    cout << "Invalid argument, number must be positive.";
                    position = argc;
                    break;
                } else if (flexy == 0){
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].red = 0;
                    }
                } else {
                    cout << "before: " << tracking_img.pix_vec[1].red << endl;
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        int tempred = 2 * int(mult_pix(tracking_img.pix_vec[i].red, ((char)flexy * 255)));
                        if (tempred > 255){
                            tracking_img.pix_vec[i].red = 255;
                        } else {
                            tracking_img.pix_vec[i].red = tempred;
                        }
                    }
                    cout << "after: " << (int)tracking_img.pix_vec[1].red << endl;
                }
                cout << "finished scalered";
                position += 1;
            }

        } else if (method == "scalegreen"){
            //scalegreen method, requires one more additional argument
            cout << "They said scalegreen!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                position += 1;
                string flex = argv[position];
                int flexy;
                try {
                    cout << flex << endl;
                    flexy = stoi(flex);
                    cout << flexy << endl;
                } catch (invalid_argument &e) {
                    cout << "Invalid argument, expected number.";
                    position = argc;
                    break;
                }
                if (flexy < 0) {
                    cout << "Invalid argument, number must be positive.";
                    position = argc;
                    break;
                } else if (flexy == 0){
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].green = 0;
                    }
                } else {
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        int tempgreen = 2 * int(mult_pix(tracking_img.pix_vec[i].green, ((char)flexy * 255)));
                        if (tempgreen > 255){
                            tracking_img.pix_vec[i].green = 255;
                        } else {
                            tracking_img.pix_vec[i].green = tempgreen;
                        }
                    }
                }
                cout << "finished scalegreen";
                position += 1;
            }
        } else if (method == "scaleblue"){
            //scaleblue method, requires one more additional argument
            cout << "They said scaleblue!!" << endl;
            if (position + 1 >= argc) { //doesn't provide additional argument
                cout << "Missing argument.";
                position = argc;
                break;
            } else {
                position += 1;
                string flex = argv[position];
                int flexy;
                try {
                    cout << flex << endl;
                    flexy = stoi(flex);
                    cout << flexy << endl;
                } catch (invalid_argument &e) {
                    cout << "Invalid argument, expected number.";
                    position = argc;
                    break;
                }
                if (flexy < 0) {
                    cout << "Invalid argument, number must be positive.";
                    position = argc;
                    break;
                } else if (flexy == 0){
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        tracking_img.pix_vec[i].blue = 0;
                    }
                } else {
                    for (int i = 0; i < tracking_img.pix_vec.size(); i++) {
                        //int tempblue = 3 * int(mult_pix(tracking_img.pix_vec[i].blue, ((char)flexy * 255)));
                        float NP1 = (float)tracking_img.pix_vec[i].blue/255;
                        float newP = ((NP1 * flexy) * 255) + 0.5f;
                        if (newP > 255){
                            tracking_img.pix_vec[i].blue = 255;
                        } else {
                            tracking_img.pix_vec[i].blue = (char)newP;
                        }
                    }
                }
                cout << "finished scaleblue";
                position += 1;
            }
        }
    }
    ofstream out_file(out_name, ios_base::binary | ios::out | ios::trunc);
    tracking_img.write_to(out_file);
    out_file.close();
    return 0;
}