
#include <iostream>
#include "finder_engine.h"




int main()
{
    finder_engine engine;
    const std::string my_path = engine.get_path_by_name("atikmdag_dce.log");
    std::cout << "==============================\n" << my_path << std::endl;


}

