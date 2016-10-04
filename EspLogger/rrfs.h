#ifndef RRFS
#define RRFS

#include "main.h"
#include <ESP8266WebServer.h>
#include <FS.h>


class RRFs{


  public:
  RRFs(ESP8266WebServer* server);
  ~RRFs();
  //format bytes
  String formatBytes(size_t bytes);
  String getContentType(String filename);
  bool handleFileRead(String path);
  
  void handleFileUpload();
  void handleFileDelete();
  
  void handleFileCreate();
  
  void handleFileList() ;
    
  private:
  ESP8266WebServer* server;
 
  };


#endif 
