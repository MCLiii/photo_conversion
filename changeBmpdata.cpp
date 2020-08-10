//
//  main.cpp
//  bmp
//
//  Created by Jim Li on 8/5/20.
//  Copyright © 2020 Jim Li. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

using namespace std;

typedef unsigned char byte;
typedef unsigned short byte_2;

int width, height,offbits;
vector<byte> data;
vector<byte> color;
vector<byte> pix_ref;
int colour [256][256][256];
ifstream fin;

//store data into array
void io(string f_name){
    fin.open(f_name, ios::binary|ios::ate);
    data.resize(fin.tellg());
    fin.seekg(0);
    fin.read((char*)&data[0], data.size());
    fin.close();
}

//read little endian from array
int lil_endien_reader(int start, int end){
    int num=0;
    for(int i = 0 ; i <= end-start ;i ++) {
        num += data[start + i] << (i * 8);
    }
    return num;
}

void read_header(){
    offbits=lil_endien_reader(10, 13);
    height=lil_endien_reader(22,25);
    width=lil_endien_reader(18,21);
}

void assign_index(){
    memset(colour, -1 , sizeof(colour));
    vector<byte> tmp;
    int r,g,b,a=0,cnt=0;
    for(int i = 0 ; i < pix_ref.size()/3 ; i++){
        b=pix_ref[i*3];
        g=pix_ref[i*3+1];
        r=pix_ref[i*3+2];
        if(colour[r][g][b]==-1){
            color.push_back(b);
            color.push_back(g);
            color.push_back(r);
            color.push_back(a);
            colour[r][g][b]=cnt;
            cnt++;
        }
        tmp.push_back(colour[r][g][b]);
    }
    pix_ref.assign(&tmp[0], &tmp[tmp.size()]);
}

void read_file(){
    color.assign(&data[54],&data[offbits]);
    pix_ref.assign(&data[offbits],&data[data.size()]);
    if(color.size()==0){
        assign_index();
    }
}

void write_header(ofstream& fout, int f_size, int offbit, int colNum, byte_2 bit){
    fout<<"BM";
    fout.write((char*)&f_size,sizeof(f_size));
    int tmp=0;
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&offbit, sizeof(offbit));
    tmp=40;
    fout.write((char*)&tmp, sizeof(tmp));
    fout.write((char*)&width, sizeof(width));
    fout.write((char*)&height, sizeof(height));
    byte_2 plane=1;
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

//only for 8 bit and below
void write_file_with_ref(vector<byte> color, vector<byte> reference, string f_name){
    ofstream fout(f_name, ios::binary|ios::ate);
    write_header(fout, color.size()+reference.size()+54, color.size()+54, color.size()/4, 8);
    for(byte a : color)
        fout<<a;
    for(byte a : reference)
        fout<<a;
    fout.close();
}

void write_file_no_ref(vector<byte> color, vector<byte> reference, string f_name){
    ofstream fout(f_name, ios::binary|ios::ate);
    write_header(fout, reference.size()*3+54, 54, 0, 24);
    for(int i = 0 ; i < reference.size() ; i ++){
        for(int j = 0 ; j < 3 ; j ++){
            fout<<color[reference[i]*4+j];
        }
    }
}

vector<byte> single_color(vector<byte> color, char a){
    int k;
    if(a=='r')
        k=2;
    else if(a=='g')
        k=1;
    else if(a=='b')
        k=0;
    else
        return vector<byte>{};
    vector<byte> m(color.size(),0);
    for(int i = 0 ; i < m.size()/4 ; i++){
        m[i*4+k]=color[i*4+k];
    }
    return m;
}

vector<byte> grayscale(vector<byte>color){
    vector<byte> m(color.size(),0);
    for(int i = 0 ; i < m.size(); i ++){
        byte average=(0.3 * color[i*4]) + (0.59 * color[i*4+1]) + (0.11 * color[i*4+2]);
        m[i*4]=average;
        m[i*4+1]=average;
        m[i*4+2]=average;
    }
    return m;
}

int main() {
    string f_name;
    cin>>f_name;
    io(f_name);
    read_header();
    read_file();
    write_file_no_ref(single_color(color, 'r'),pix_ref, "red.bmp");
    write_file_no_ref(single_color(color, 'g'),pix_ref, "green.bmp");
    write_file_no_ref(single_color(color, 'b'),pix_ref, "blue.bmp");
    write_file_no_ref(grayscale(color),pix_ref, "gray.bmp");
    write_file_with_ref(grayscale(color),pix_ref, "gray_referenced.bmp");
    return 0;
}