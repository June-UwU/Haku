#include "haku.hpp"
#include <iostream>


int main(int argc, char** argv) {
  std::cout << "Hellow ^_^\n";
  engine *p_engine = new engine(argc, argv);
  p_engine->run_engine();
  delete p_engine;
  
  return 0;
}  
