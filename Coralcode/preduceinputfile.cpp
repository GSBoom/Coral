#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
//Dataset processing
void remove_first_line(const std::string& file_path) {
    std::ifstream input_file(file_path);
    if (!input_file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

   
    std::string temp_file_path = file_path + ".tmp";
    std::ofstream temp_file(temp_file_path);
    if (!temp_file.is_open()) {
        std::cerr << "Failed to create temporary file!" << std::endl;
        input_file.close();
        return;
    }

    std::string line;
    std::getline(input_file, line);
    std::cout<<line<<std::endl;
    std::getline(input_file, line);

    std::cout<<std::endl;

    
    while (std::getline(input_file, line)) {
        temp_file << line << std::endl;
    }

    input_file.close();
    temp_file.close();

 
    if (std::remove(file_path.c_str()) != 0) {
        std::cerr << "Failed to delete original file!" << std::endl;
        return;
    }

    if (std::rename(temp_file_path.c_str(), file_path.c_str()) != 0) {
        std::cerr << "Failed to rename temporary file!" << std::endl;
        return;
    }

    std::cout << "First line removed successfully!" << std::endl;
}

int main() {
    std::string file_path = ".\\DataSet\\Friendster\\out.friendster"; 
    remove_first_line(file_path);
    return 0;
}