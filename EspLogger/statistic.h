#ifndef STATISTIC
#define STATISTIC

#include <typeinfo>
#include <math.h> 

#include <ESP8266WiFi.h>

// TEMPLATEs werden nur in header datei decaliert #include "statistic.h"
template <typename T, int size>
class Statistic{
  private:
    T data[size]={0};

  public:
    Statistic<T,size>(){
      //std::cout << "StatClass:";
      //std::cout << typeid(T).name() << " "<<size<<'\n';
      resetData();
    }
    T &getData(int index){
      return data[index];
      }
    void setData(const T &object, int index){
        data[index]=object;
      }
    void sumData(const T &object, int index){
        data[index]+=object;
      }
    void resetData(int index = -1){ //if -1 = reset all
      if(index==-1){
        for (int i = 0; i < size; i++) {
          data[i] = 0;
        }
      }else{
        data[index] = 0;
      }
    }
    T mean() {
      T dSum = data[0];
      for (int i = 1; i < size; ++i) {
        dSum += data[i];
      }
      return dSum/size;
    }
    float meanf() {
      T dSum = data[0];
      for (int i = 1; i < size; ++i) {
        dSum += data[i];
      }
      return dSum/(float)size;
    }
    T variance()
    {
      T meanval = mean();
      T temp = 0;
      for (int i = 0; i < size; ++i) {
        temp += (meanval-data[i])*(meanval-data[i]);
      }
      return temp/size;
    }
     float variancef()
    {
      T meanval = meanf();
      T temp = 0;
      for (int i = 0; i < size; ++i) {
        temp += (meanval-data[i])*(meanval-data[i]);
      }
      return temp/(float)size;
    }
    T stdDev()
    {
      return sqrt(variance());
    }
    float stdDevf()
    {
      return sqrt(variancef());
    }
    //todo
    String toSVG(int width,int height, byte r =110, byte g =100, byte b =110, byte line =3){
      String str="";
      float stepSize = width/(float)size +0.5;
      for(int i = 1;i<size;i++){
        str +=  "<line x1='"+String(data[i-1])+"' y1='"+String((i-1)*(stepSize))+"' x2='"+String(data[i])+"' y2='"+String((i-1)*(stepSize))+"' "
        + "style='stroke:rgb("+String(r)+","+String(g)+","+String(b)+");stroke-width:"+String(line)+"' stroke-linecap='round' />";
      }
      Serial.print("\nStatisik SVG");
      Serial.println(str);
      
      return str;
    }
    //todo
    String toCsv(){
      String str="idx;value\n";
      for(int i = 0;i<size;i++){
        str +=  String(i)+";"+data[i]+"\n";
      }
      Serial.print("\nStatisik SVG");
      Serial.println(str);
      
      return str;
    }
};

/*EXAMPLE
 * Statistic<double,10> hourly;
 hourly.setData(36,0);
  hourly.setData(37,1);
  hourly.setData(38,2);
  hourly.setData(39,3);
  hourly.setData(40,4);
  hourly.setData(41,5);
  hourly.setData(42,6);
  hourly.setData(43,7);
  hourly.setData(44,8);
  hourly.setData(45,9);
    
  std::cout<<hourly.getMean()<<"\n";
  std::cout<<hourly.getVariance()<<"\n";
  std::cout<<hourly.getStdDev()<<"\n";
  Statistic<bool,24> booler;
  Statistic<float,24> flaoter; 
  */


#endif
