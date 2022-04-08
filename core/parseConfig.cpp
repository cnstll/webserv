#include <map>
#include <iterator>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>

void displayMap(const std::map<std::string, std::string> &map){
  std::map<std::string, std::string>::const_iterator it = map.begin();
  while(it != map.end()){
    std::cout << "Field: " << it->first << " - Value: " << it->second << std::endl;
    ++it;
  }
}
std::string configToString(char *filepath){

  std::ifstream configFile;
  std::string line;
  std::string config;
  std::multimap<std::string, std::map<std::string, std::string> > parsedConfigHolder; 
  configFile.open(filepath, std::ifstream::in);
  if (!configFile.is_open())
  {
    std::cerr << "Configuration file could not be found\n";
    exit(EXIT_FAILURE);
    }
    while (getline(configFile, line))
    {
      config += line;
      config += "\n";
  }
  std::cout << "HERE COMES THE CONFIG\n";
  std::cout << config;
  configFile.close();
  return config;
}

int main (int argc, char *argv[]){
  // test if config file exist, otherwise exit
  if (argc != 2){
    std::cerr << "Wrong Number of arguments\n";
    exit(EXIT_FAILURE);
  }
  std::string config = configToString(argv[1]);
  std::string parsedConfig;
  std::string serverMinimalConfig[] = {
    "listen",
    "server_name",
    "root",
    ""
  };
  std::map<std::string, std::string> serverConf;
  int i = 0;
	//fill map with all possible allowed fields in server
	while (serverMinimalConfig[i] != ""){
		serverConf[serverMinimalConfig[i]] = std::string();
		i++;
	}
  displayMap(serverConf);
  // parse server bloc into map
  // parse http bloc into map
}