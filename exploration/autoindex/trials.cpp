# include <iostream>
# include <dirent.h>

#define ROOTDIR "/mnt/nfs/homes/jescully/Documents/webserv/exploration/autoindex"

void directory_contents ( char * directory_path )
{  
  DIR *dh;
  struct dirent * contents; 
  
  dh = opendir ( directory_path );
  
  if ( !dh )
  {
    std::cout << "The given directory is not found";
    return;
  }
  std::cout << "<Html>" << std::endl;
  while ( ( contents = readdir ( dh ) ) != NULL )
  {
    //   <a href="message.html">A Message from Warren E. Buffett</a>
    std::string name = "<a href=\"" + std::string(contents->d_name) + "\">";
    std::cout << name << std::endl;
  }
  std::cout << "</Html>" << std::endl;
  closedir ( dh );
}

int main ()
{
  directory_contents ( (char*) ROOTDIR);
  return 0;
}