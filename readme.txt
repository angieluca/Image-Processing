Your output files should be written to this folder. Read files from input, process them, and write them here. Compare your output files against those in the examples folder.
This was to practice reading and writing to tga files:
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
