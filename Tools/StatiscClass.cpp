

// Example program
#include <iostream>
#include <string>
#include <typeinfo>
#include <cmath> 

using namespace std;

typedef unsigned char byte;

template <typename T, int size>
class Statistic{
private:
    T data[size]={0};

public:
    Statistic<T,size>(){
        //std::cout << "StatClass:";
        //std::cout << typeid(T).name() << " "<<size<<'\n';
    }
    T &getData(int index){
        return data[index];
        }
    void setData(const T &object, int index){
            data[index]=object;
        }
    T getMean() {
        T dSum = data[0];
        for (int i = 1; i < size; ++i) {
            dSum += data[i];
        }
        return dSum/size;
    }
    T getVariance()
    {
        T mean = getMean();
        T temp = 0;
        for (int i = 0; i < size; ++i) {
            temp += (mean-data[i])*(mean-data[i]);
        }
        return temp/size;
    }
    T getStdDev()
    {
        return sqrt(getVariance());
    }
    T getMax(){
        T max = data[0];
        for (int i = 1; i < size; ++i) {
           if(max < data[i]){
               max=data[i];
           }
        }
        return max;
    }
    T getMin(){
        T min = data[0];
        for (int i = 1; i < size; ++i) {
           if(min > data[i]){
               min=data[i];
           }
        }
        return min;
    }
    T getSpan(){
			return getMax()-getMin();
	}
    //todo
    string toSVG(int width,int height, byte lineWidth =3,char** indexTexts=0, int indexTextSize=0,byte r =110, byte g =100, byte b =110){
       
        int lastWidthTick=0;
        int padding=2;
        T max=getMax();
        T min=getMin();
        T span=max-min;
        int legendY=20;
        int legendX=35;
        int canvasHeight = height-legendY-padding;
        int canvasWidth = width-legendX-padding;
        int stepSize = canvasWidth/(double)size +0.5;
        int legendXStepSize = canvasHeight/(double)span +0.5; //rounded
        int fontsize=9; //pt
        string str="";
        str+="<svg xmlns='http://www.w3.org/2000/svg' version='1.1' width='"+to_string(width)+"' height='"+to_string(height)+"'>\n";
        str+="<style>.data:hover{stroke-width: "+to_string(lineWidth*2)+";}\n.data{stroke-width: "+to_string(lineWidth)+"; stroke-linecap:round;}\n.legend{stroke-width:1; stroke-dasharray:2,2;}</style>";
        str+="\n<rect  width='"+to_string(width-legendX)+"' height='"+to_string(height-legendY)+"' fill='#516291' stroke='#07153B' /><g stroke='#07153B'>";
        //style='stroke:rgb("+to_string(r)+","+to_string(g)+","+to_string(b)+");stroke-width:"+to_string(line)+"'
        // the x Legend
        for(int i=0;i<legendXStepSize;i++){
			str +="<text x='"+to_string(canvasWidth+4)+"' y='"+to_string(canvasHeight-canvasHeight*(i/(double)legendXStepSize)+fontsize)+"' style='font-size:"+to_string(fontsize)+"pt;' >";
			str +=to_string((span*(i/(double)legendXStepSize) + min));
			str +="</text>\n";
			if(i>0){
				str +="<line class='legend' x1='0' x2='"+to_string(canvasWidth+4)+"'";
				str+=" y1='"+to_string(canvasHeight-canvasHeight*(i/(double)legendXStepSize))+"' y2='"+to_string(canvasHeight-canvasHeight*(i/(double)legendXStepSize))+"' />\n";
			}
		}
		int x=0;
        //The values and y legend
        for(int i = 1;i<size;i++){
			x=(i)*(stepSize);
            str +=  "<line  class='data' y1='"+to_string(canvasHeight-(canvasHeight*(data[i-1]-min)/span))+"' x1='"+to_string((i-1)*(stepSize))+"' ";
            str+=" y2='"+to_string(canvasHeight-(canvasHeight*(data[i]-min)/span))+"' x2='"+to_string(x)+"' ";
            str += " title='"+to_string(data[i-1])+" nach "+to_string(data[i])+ "' />\n";
            //the ticks
            str +="<line class='legend' y1='0' y2='"+to_string(canvasHeight+4)+"'";
			str+=" x1='"+to_string(x)+"' x2='"+to_string(x)+"' />";
			//the texts
            if(lastWidthTick - x < 0 ){
					str +="<text x='"+to_string(x-fontsize)+"' y='"+to_string(height-1)+"' style='font-size:"+to_string(fontsize)+"pt;' >";
					if(i<indexTextSize){
						str += indexTexts[i];
					}else{
						str+=to_string(i);
					}
					str+="</text>\n";
					lastWidthTick = x+20;
			}  
        }
        str+="</g>";
		str+="</svg>\n";
        return str;
    }


    string toCsv(char** indexTexts=0, int indexTextSize=0){ //the offset allows to display a diffents index (e.g. offeset =-2 will lead to i=0 -> 0=-2 

        string str="";
        for(int i = 0;i<size;i++){
            if(i<indexTextSize){
                str += indexTexts[i];
            }else{
                str+=to_string(i);
            }
            str+=+"\t"+to_string(data[i])+"\r\n";
        }

        return str;
     }

 };


int main(){

Statistic<double,24> hourly;
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
hourly.setData(10,10);
hourly.setData(11,11);
hourly.setData(12,12);
hourly.setData(20,13);
hourly.setData(24,14);
hourly.setData(24,15);
hourly.setData(41.25,16);
hourly.setData(41.25,17);
hourly.setData(41.25,18);

hourly.setData(31.875,19);
hourly.setData(32,20);
hourly.setData(32,21);
hourly.setData(38,22);
hourly.setData(38,23);

//std::cout<<hourly.getMean()<<"\n";
//std::cout<<hourly.getVariance()<<"\n";
//std::cout<<hourly.getStdDev()<<"\n";
Statistic<bool,24> booler;
Statistic<float,24> flaoter;


    char* texts[4]={"23","00","01","02"};
    //cout<<hourly.toCsv(texts,4);
    cout<<"\n\n";
    cout<<hourly.toSVG(400,200,3,texts,4);
 }

