//
// Created by Jim Li on 2020/8/6.
//IMX476_5M_Unpack10_20200605T170455.2592X1940.unpack10_rggb.vcmpos_0

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>


using namespace std;

typedef unsigned short byte2;
typedef unsigned char byte;

vector<vector<byte2>> file;
int height=1940, width=2592;
byte colour[1938][2590][3];
vector<vector<byte>> raw;

int mean2(int a,int b){return (a+b)/2;}
int mean4(int a, int b, int c, int d){return (a+b+c+d)/4;}

void init(){
    vector<byte2> a(width);
    file.assign(height, a);
    vector<byte> m(width);
    raw.assign(height, m);
}

void io(string f_name){
    ifstream fin(f_name, ios::binary|ios::ate);
    fin.seekg(0);
    for(int i = 0 ; i < file.size() ; i++)
        fin.read((char*)&file[i][0],file[i].size()*2);
}

void raw10_to_raw8(){
    for(int i =0 ; i < file.size(); i++){
        for(int j = 0 ; j < file[0].size() ; j ++){
            raw[i][j]=file[i][j]>>2;
        }
    }
}

void write_header(ofstream& fout, int f_size, int offbit, int colNum, byte2 bit){
    fout<<"BM";
    fout.write((char*)&f_size,sizeof(f_size));
    int tmp=0;
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&offbit, sizeof(offbit));
    tmp=40;
    fout.write((char*)&tmp, sizeof(tmp));
    width-=2;
    height-=2;
    fout.write((char*)&width, sizeof(width-1));
    fout.write((char*)&height, sizeof(height-1));
    byte2 plane=1;
    fout.write((char*)&plane, sizeof(plane));
    fout.write((char*)&bit, sizeof(bit));
    tmp=0;
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&colNum, sizeof(colNum));
    fout.write((char*)&colNum, sizeof(colNum));
}

void write_file_no_ref(string f_name){
    ofstream fout(f_name, ios::binary|ios::ate);
    write_header(fout, 2590*1938*3+54, 54, 0, 24);
    for(int i = 0 ; i < 1938 ; i ++){
        for(int  j=0 ; j< 2590 ; j++){
            for(int k = 0 ; k < 3 ; k++){
                fout<<colour[i][j][k];
            }
        }
    }
}

vector<byte> convert(){
    vector<byte> pixels;
    ofstream fout("abc.bmp", ios::binary|ios::ate);
    write_header(fout, 2590*1938*3+54, 54, 0, 24);
    byte r,g,b,a=0;
    for(int i = 1 ; i < 1939;i++){
        for(int j = 1 ; j < 2591; j++){
            if(i%2==1){
                if(j%2==1){
                    //blue center pixel
                    b=colour[i-1][j-1][0]=(raw[i][j]);
                    g=colour[i-1][j-1][1]=(raw[i-1][j]+raw[i+1][j]+raw[i][j+1]+raw[i][j-1]>>2);
                    r=colour[i-1][j-1][2]=(raw[i-1][j-1]+raw[i-1][j+1]+raw[i+1][j-1]+raw[i+1][j+1]>>2);
                }else{
                    //green center pixel
                    b=colour[i-1][j-1][0]=(raw[i][j+1]+raw[i][j-1]>>1);
                    g=colour[i-1][j-1][1]=(raw[i][j]);
                    r=colour[i-1][j-1][2]=(raw[i+1][j]+ raw[i-1][j]>>1);
                }
            }else{
                if(j%2==0){
                    //red center pixel
                    b=colour[i-1][j-1][0]=(raw[i-1][j-1]+raw[i-1][j+1]+raw[i+1][j-1]+raw[i+1][j+1]>>2);
                    g=colour[i-1][j-1][1]=(raw[i-1][j]+raw[i+1][j]+raw[i][j+1]+raw[i][j-1]>>2);
                    r=colour[i-1][j-1][2]=(raw[i][j]);
                }else{
                    //green center pixel
                    b=colour[i-1][j-1][0]=(raw[i+1][j]+ raw[i-1][j]>>1);
                    g=colour[i-1][j-1][1]=(raw[i][j]);
                    r=colour[i-1][j-1][2]=(raw[i][j+1]+raw[i][j-1]>>1);
                }
            }
            fout<<b<<g<<r;
        }
        fout<<(unsigned char)0<<(unsigned char)0;
    }
    return pixels;
}

void cut(){
    ofstream fout("c.raw", ios::binary|ios::ate);
    for(int i = 0 ; i < 6 ; i ++){
        for(int j = 0 ; j < 6 ; j ++){
            fout<<raw[i][j];
        }
    }
}

void out(){
    ofstream fout("abc.bmp", ios::binary|ios::ate);
    write_header(fout, 2592*1940*3+54,54,0,24);
    for(int i = 0; i < raw.size(); i++){
        for(int j = 0 ; j < raw[0].size();j++){
            fout<<raw[i][j]<<raw[i][j]<<raw[i][j];
        }
    }
}

void read_raw8(string f_name){
    ifstream fin(f_name, ios::binary|ios::ate);
    fin.seekg(0);
    for(int i = 0 ; i < 6 ; i ++){
            fin.read((char*)&raw[i][0], raw[i].size());
    }
}

int main(){
    string f_name;
    cin>>f_name;
    init();
    io(f_name);
    raw10_to_raw8();
    convert();
    return 0;
}