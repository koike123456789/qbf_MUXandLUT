#include<iostream>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<fstream>
#include<cmath>
#include<bitset>
#include<vector>
#include<functional>
#include<algorithm>
#include<sstream>
#include<string>
//-------------------------------------------------------------------------------

int ch_ff=0;

//-------------------------------------------------------------------------------

int count_ff=0;
int count_input=0;
int size_mux=0;
int size_lut=0;

std::vector<std::string> latch_in;
std::vector<std::string> latch_out;
std::vector<std::string> vec_inputs;
std::string s_out;


void read_blif(std::string file_name);
void write_blif(std::string infile, std::string outfile);

int count_blank(const char* str);
int Size_mux();
int Size_lut();
std::vector<bool> binary(int n);

//--------------------------------------------------------------------------------
int main(int argc , char** argv){
    if(argc != 4){
        std::cout << "Error (./a.out infile_name outfile_name)" << std::endl;
        return 0;
    }
    std::string infile = argv[1];
    std::string outfile = argv[2];
    ch_ff = std::stoi(argv[3]);
    std::cout<<"infile:"<<infile<<std::endl;
    std::cout<<"outfile:"<<outfile<<std::endl;
    std::cout<<"choose_FF:"<<ch_ff<<std::endl;
    read_blif(infile);
    std::cout<<"lut_size:"<<Size_lut()<<std::endl;
    std::cout<<"mux_size:"<<Size_mux()<<std::endl;
    write_blif(infile,outfile);
    
    std::cout<<"main_finish"<<std::endl;
    std::cout<<"qbf parameters =  LUT:" << Size_lut() - 1 << " ctl:" << Size_mux() << std::endl;
    std::cout<<"num para = " << Size_lut() + Size_mux() - 1 << std::endl;
    return 0;
}


//空白数える------------------------------------------------------------------------
int count_blank(const char* str){
    int i, count = 0;
    for(i = 0;str[i]!='\0';i++){
        if(str[i] == ' '){
        count++;
        }
    }
    return count;
}


//size_mux-----------------------------------------------
int Size_mux(){
    return count_ff;
}


//size_lut-----------------------------------------------
int Size_lut(){
    return int(pow(2,ch_ff));
}

//二進数表記----------------------------------
std::vector<bool> binary(int n){
    std::vector<bool> v(size_mux,0);
    for(int i=0;n>0;i++){
        v[i] = n%2;
        n = n/2;
    }
    return v;
}

//blifファイルから必要なデータを読み取る--------------------------------------------------
void read_blif(std::string file_name){
    std::cout<<"start_read"<<std::endl;
    std::string str;

    std::ifstream ifs(file_name);
    if (ifs.fail()){
        std::cerr << "Failed to open file." << std::endl;
    }
    
    while (getline(ifs, str)) {
        if(str.find(".input")!=std::string::npos){
            std::stringstream ss(str);
            std::string str2;
            char delim = ' ';
            getline(ss,str2,delim);
            while(getline(ss,str2,delim)){
                vec_inputs.push_back(str2);
                count_input++;
            }
            //std::cout << str << std::endl;
            // count_input=count_blank(str.c_str());
        }else if(str.find(".latch")!=std::string::npos){//-------------------------------------------
        std::istringstream iss(str);
        std::string a,in,out,b;
        iss >> a>>in>>out>>b;
        //std::cout << in<<out<< std::endl;
        
        latch_in.push_back(in);
        latch_out.push_back(out);
 
        count_ff=count_ff+1;
        }else if(str.find(".output")!=std::string::npos){
        std::istringstream iss(str);
        std::string a,output;
        iss >> a>>output;
        //std::cout << output<< std::endl;
        s_out=output;
        
        }else{
        //std::cout << str << std::endl;
        }
    }

    //read_blif.txtに書き込む
    std::cout<<"count_ff:"<<count_ff << std::endl;
    std::cout<<"count_input:"<<count_input<<std::endl;
    std::cout << "read_finish" << std::endl;

}

//blif書き換え------------------------------------------------------------------------------
void write_blif(std::string infile, std::string outfile){
    std::cout<<"start_write"<<std::endl;
    std::string str;

    size_mux=Size_mux();
    size_lut=Size_lut();

    std::ifstream ifs(infile);
    if (ifs.fail()){
        std::cerr << "Failed to open file." << std::endl;
    }
    
    std::ofstream ofs_write_blif(outfile);//file name -------------------------------------
    if (!ofs_write_blif){
        std::cerr << "Failed to open file." << std::endl;
    }

    ofs_write_blif<<".model " + infile << std::endl;

  //inputs outputs----------------------------------------------------
    ofs_write_blif<<".inputs";
    int i=0;
    //lut
    for(i=0;i<size_lut-1;i++){
        ofs_write_blif<<" "<<"lut"<<i+1;
    }
    //mux
    for(i=0;i<size_mux;i++){
        ofs_write_blif<<" "<<"ctl"<<i;
    }
    //input
    for(i=0;i<count_input;i++){
        ofs_write_blif<<" "<<vec_inputs[i];
    }
    //ff
    for(i=0;i<count_ff;i++){
        ofs_write_blif<<" "<<latch_out[i];
    }
    ofs_write_blif<<std::endl;;
    //out
    ofs_write_blif<<".outputs out"<< std::endl;

    //subckt---------------------------------------------
    ofs_write_blif<<".subckt MUX ";
    for(int i=0;i<size_mux;i++){
        ofs_write_blif<<"ctl"<<i<<"=ctl"<<i<<" ";
    }
    for(int i=0;i<count_ff;i++){
        ofs_write_blif<<"FF"<<i<<"="<<  latch_in[i]<<           " ";//---------------------------------
    }
    for(int i=0;i<ch_ff;i++){
        ofs_write_blif<<"out"<<i<<"=lut_iny_"<<i<<" ";
    }
    ofs_write_blif<<std::endl;
    ofs_write_blif<<".subckt LUT ";
    for(int i=0;i<ch_ff;i++){
        ofs_write_blif<<"in_FF"<<i<<"="<<"lut_iny_"<<i<<" ";
    }
    
    ofs_write_blif<<"lut0=one"<<" ";
    for(int i=1;i<size_lut;i++){
        ofs_write_blif<<"lut"<<i<<"="<<"lut"<<i<<" ";
    }
    ofs_write_blif<<"out=y";
    ofs_write_blif<<std::endl;

    ofs_write_blif<<".subckt MUX ";
    for(int i=0;i<size_mux;i++){
        ofs_write_blif<<"ctl"<<i<<"=ctl"<<i<<" ";
    }
    for(int i=0;i<count_ff;i++){
        ofs_write_blif<<"FF"<<i<<"="<< latch_out[i]<<            " ";//---------------------------------
    }
    for(int i=0;i<ch_ff;i++){
        ofs_write_blif<<"out"<<i<<"=lut_inx_"<<i<<" ";
    }
    ofs_write_blif<<std::endl;
    
    ofs_write_blif<<".subckt LUT ";
    for(int i=0;i<ch_ff;i++){
        ofs_write_blif<<"in_FF"<<i<<"="<<"lut_inx_"<<i<<" ";
    }
    
    ofs_write_blif<<"lut0=one"<<" ";
    for(int i=1;i<size_lut;i++){
        ofs_write_blif<<"lut"<<i<<"="<<"lut"<<i<<" ";
    }
    ofs_write_blif<<"out=x";
    ofs_write_blif<<std::endl;

    ofs_write_blif<<".names one"<< std::endl;
    ofs_write_blif<<"1"<< std::endl;

    // LUT outputs are all 1 or not(all 1 -> chech_lut = 1)
    ofs_write_blif<<".names";
    for(int i=1;i<size_lut;i++){
        ofs_write_blif<<" lut"<<i;
    }
    ofs_write_blif<<" check_lut"<<std::endl;
    for(int i=1;i<size_lut;i++){
        ofs_write_blif<<"1";
    }
    ofs_write_blif<<" 1" << std::endl;

    // outputs of circuit = (~x or y) and ~check_lut
    ofs_write_blif<<".names x y check_lut out"<< std::endl;
    ofs_write_blif<<"10- 0"<< std::endl;
    ofs_write_blif<<"--1 0"<< std::endl;


    //元のファイルから書き写す-------------------------
    ofs_write_blif<< std::endl;
    while (getline(ifs, str)) {
        if(str.find(".inputs")!=std::string::npos){
        }else if(str.find(".latch")!=std::string::npos){
        ofs_write_blif<<"#"<<str<<std::endl;
        }else if(str.find(".names")!=std::string::npos){
        ofs_write_blif<<str<<std::endl;
        }else if(str.find(".outputs")!=std::string::npos){
        }else if(str.find(".model")!=std::string::npos){
        }else{
        ofs_write_blif<<str<<std::endl;
        }
    }

    //lut部分-------------------------------------
    ofs_write_blif<< std::endl;
    ofs_write_blif<<".model LUT"<< std::endl;
    ofs_write_blif<<".inputs";
    for(int i=0;i<ch_ff;i++){
        ofs_write_blif<<" "<<"in_FF"<<i;
    }
    for(int i=0;i<size_lut;i++){
        ofs_write_blif<<" "<<"lut"<<i;
    }
    ofs_write_blif<<std::endl;
    
    ofs_write_blif<<".outputs out"<< std::endl;
    
    ofs_write_blif<<".names";
    for(i=0;i<ch_ff;i++){
        ofs_write_blif<<" "<<"in_FF"<<i;
    }
    for(i=0;i<size_lut;i++){
        ofs_write_blif<<" "<<"lut"<<i;
    }
    ofs_write_blif<<" "<<"out";
    ofs_write_blif<<std::endl;
    
    for(i=0;i<pow(2,ch_ff);i++){
        std::vector<std::string> v(size_lut,"-");
        for(int k=ch_ff-1;k>=0;k--){
        ofs_write_blif<<binary(i)[k];
        }
        
        v[i]="1";

        for(int k=0;k<size_lut;k++){
        ofs_write_blif<<v[k];
        }
        
        ofs_write_blif<<" 1";
        ofs_write_blif<<std::endl;
    }
    

    
    ofs_write_blif<<".end"<< std::endl;


    //mux部分------------------------------------
    ofs_write_blif<< std::endl;
    ofs_write_blif<<".model MUX"<< std::endl;
    //inputs
    ofs_write_blif<<".inputs";
    for(i=0;i<size_mux;i++){
        ofs_write_blif<<" "<<"ctl"<<i;
    }
    for(i=0;i<count_ff;i++){
        ofs_write_blif<<" "<<"FF"<<i;
    }
    ofs_write_blif<<std::endl;

    
    //outputs
    ofs_write_blif<<".outputs";
    for(i=0;i<ch_ff;i++){
        ofs_write_blif<<" "<<"out"<<i;
    }
    ofs_write_blif<<std::endl;

    //names
    int j=0;
    for(j=0;j<ch_ff;j++){
        ofs_write_blif<<".names";
        for(i=0;i<size_mux;i++){
        ofs_write_blif<<" "<<"ctl"<<i;
        }
        for(i=0;i<count_ff;i++){
        ofs_write_blif<<" "<<"FF"<<i;
        }
        ofs_write_blif<<" "<<"out"<<j<<std::endl;
        
        for(i=0;i<pow(2,size_mux);i++){
        std::vector<bool> v;
        for(int k=size_mux-1;k>=0;k--){
        ofs_write_blif<<binary(i)[k];
        }
                
        v=binary(i);

        int count_v=std::count(v.begin(),v.end(),1);
        int count_1=0;
        bool f=0;
        for(int k=0;k<count_ff;k++){
        if(count_v==0){
        if(k==0){
            ofs_write_blif<<"1";
        }else{
            ofs_write_blif<<"-";
        }
        }
        
        else if(count_v>=ch_ff){
        if(v[k]==0){
            ofs_write_blif<<"-";
        }else if(v[k]==1){
            if((count_1==j)&&(f==0)){
            ofs_write_blif<<"1";
            f=1;
            }else{
            ofs_write_blif<<"-";
            }
            count_1=count_1+1;
        }
        }else{
        if(v[k]==0){
            ofs_write_blif<<"-";
        }else if(v[k]==1){
            if( ((count_1==j) || (count_1==count_v-1)) &&(f==0)){
            ofs_write_blif<<"1";
            f=1;
            }else{
            ofs_write_blif<<"-";
            }
            count_1=count_1+1;
        }
            }
        
        }
        
        ofs_write_blif<<" 1";
        ofs_write_blif<<std::endl;
        }
    }
    
    //end
    ofs_write_blif<<".end"<< std::endl;
    std::cout<<"write_finish"<<std::endl;
    }
