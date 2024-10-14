#include <LiOB/math/library.hpp>
#include <iostream>

int main(){
    LiOB::LiOB_config config;

    LiOB::LiOB_address address;
    // address.room_uid = LiOB::LiOB_random_room_uid();
    address.wall = 1;
    address.shelf = 2;
    address.volume = 3;
    address.page = 42;

    LiOB::Library library(config);

    std::string phrase = "lorem ipsum";
    std::cout << "Searching for phrase <" << phrase << ">: ";
    
    auto faddress = library.addressGen(
        LiOB::fill_with_spaces(phrase, library.max_simbols().convert_to<int>()), 
        library.get_page_uid(address)
    );

    std::cout << "Found at address: " << faddress.to_string() << std::endl;

    std::string content = library.contentGen(faddress);

    std::cout << "Content: " << content << std::endl;

    return 0;
}