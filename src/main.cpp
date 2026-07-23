#include <cassert>
#include <string>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <map>
#include <vector>
#include <list>
#include <array>
#include <algorithm>

#include "shortjson.h"
#include "rcdef.h"

using namespace shortjson;

std::string substitute(std::string str, std::string one, std::string two)
{
  for(size_t loc = std::string::npos; (loc = str.find("$1")) != std::string::npos;)
    str = str.replace(loc, 2, one);

  for(size_t loc = std::string::npos; (loc = str.find("$2")) != std::string::npos;)
    str = str.replace(loc, 2, two);

  return str;
}


int main(int argc, char** argv)
{
  node_t struct_data = Parse(rules_json);

  //"definition_types"
  std::cout << struct_data.identifier << std::endl;



  return 0;
}
