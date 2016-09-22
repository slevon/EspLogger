#include "rrfs.h"


RRFs::RRFs(ESP8266WebServer* webserver){


  Serial.println("Setting up RRFs");
  server=webserver;

  if(SPIFFS.begin()){
    Serial.println("Filesystem started");
  }else{
    Serial.println("Filesystem failed");  
  }


  //Setup pages:
   //SERVER INIT
  //list directory
  server->on("/fs/list", HTTP_GET, std::bind(&RRFs::handleFileList,this));
  //load editor
  server->on("/fs/edit", HTTP_GET, [&](){
    if(!handleFileRead("/edit.htm")) server->send(404, "text/plain", "FileNotFound");
  });
  //create file
  server->on("/fs/edit", HTTP_PUT, std::bind(&RRFs::handleFileCreate,this));
  //delete file
  server->on("/fs/edit", HTTP_DELETE, std::bind(&RRFs::handleFileDelete,this));
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server->on("/fs/edit", HTTP_POST, [&](){ server->send(200, "text/plain", ""); },std::bind(&RRFs::handleFileUpload,this));


  //get heap status, analog input value and all GPIO statuses in one json call
  server->on("/fs/all", HTTP_GET, [&](){
    String json = "{";
    json += "\"heap\":"+String(ESP.getFreeHeap());
    json += ", \"analog\":"+String(analogRead(A0));
    json += ", \"gpio\":"+String((uint32_t)(((GPI | GPO) & 0xFFFF) | ((GP16I & 0x01) << 16)));
    json += "}";
    this->server->send(200, "text/json", json);
    json = String();
  });


  
  //called when the url is not defined here
  //use it to load content from SPIFFS
  server->onNotFound([&](){
    if(!handleFileRead(server->uri()))
      this->server->send(404, "text/plain", "FileNotFound");
  });

  
  }

RRFs::~RRFs(){
  
  
  }


  
//format bytes
String RRFs::formatBytes(size_t bytes){
  if (bytes < 1024){
    return String(bytes)+"B";
  } else if(bytes < (1024 * 1024)){
    return String(bytes/1024.0)+"KB";
  } else if(bytes < (1024 * 1024 * 1024)){
    return String(bytes/1024.0/1024.0)+"MB";
  } else {
    return String(bytes/1024.0/1024.0/1024.0)+"GB";
  }
}

String RRFs::getContentType(String filename){
  if(server->hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool RRFs::handleFileRead(String path){
  Serial.print("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server->streamFile(file, contentType);
    file.close();
    Serial.println("...ok");
    return true;
  }
  return false;
}

void RRFs::handleFileUpload(){
  File fsUploadFile;
  if(server->uri() != "/edit") return;
  HTTPUpload& upload = server->upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.print("handleFileUpload Name: "); Serial.println(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if(upload.status == UPLOAD_FILE_WRITE){
    //Serial.print("handleFileUpload Data: "); Serial.println(upload.currentSize);
    if(fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
      fsUploadFile.close();
    Serial.print("handleFileUpload Size: "); Serial.println(upload.totalSize);
  }
}

void RRFs::handleFileDelete(){
  if(server->args() == 0) return server->send(500, "text/plain", "BAD ARGS");
  String path = server->arg(0);
  Serial.println("handleFileDelete: " + path);
  if(path == "/")
    return server->send(500, "text/plain", "BAD PATH");
  if(!SPIFFS.exists(path))
    return server->send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server->send(200, "text/plain", "");
  path = String();
}

void RRFs::handleFileCreate(){
  if(server->args() == 0)
    return server->send(500, "text/plain", "BAD ARGS");
  String path = server->arg(0);
  Serial.println("handleFileCreate: " + path);
  if(path == "/")
    return server->send(500, "text/plain", "BAD PATH");
  if(SPIFFS.exists(path))
    return server->send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if(file)
    file.close();
  else
    return server->send(500, "text/plain", "CREATE FAILED");
  server->send(200, "text/plain", "");
  path = String();
}

void RRFs::handleFileList() {
  if(!server->hasArg("dir")) {server->send(500, "text/plain", "BAD ARGS"); return;}
  
  String path = server->arg("dir");
  Serial.println("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while(dir.next()){
    if (output != "[") output += ',';
   output += "{";
   //output+="\"type\":\"";
   // output += (dir.isDir())?"dir":"file";
    output += "\"name\":\"";
    output += String(dir.fileName()).substring(1);
    output += "\",\"size\":\"";
    output += String(dir.fileSize()).substring(1);
    output += "\"}";
  }
  
  output += "]";
  server->send(200, "text/json", output);
}
