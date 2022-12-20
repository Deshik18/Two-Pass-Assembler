#include<bits/stdc++.h>
#include<fstream>
using namespace std;

string fileName;

vector<int> errorline;
vector<string> error;
vector<string> source_code[100000];
vector<string> labels;
vector<int> labPC;
vector<string> counter;
vector<string> machinecode;
vector<string> dataline[100000];
vector<string> SETlabels;
vector<string> SETvalue;
vector<string> usedlabels;
vector<string> unusedlabels;

struct instrwithoutoperand{
    string mnemonic_0;
    int op_0;
};
struct instrwithoperand{
    string mnemonic_1;
    int op_1;
};
struct jumptype{
    string mnemonic_2;
    int op_2;
};

struct instrwithoutoperand zero[8];
struct instrwithoperand one[9];
struct jumptype two[4];


void initialize();
void firstPass(string store,int linenum,int* PC);
bool labelvalidity(string label);
int searchLabel(string label);
int search0(string mnemonic);
int search1(string mnemonic);
void secondPass();
int operandtype(string operand);
void producemachinecode(int instrtype,string mnemonic,int prog);
void producemachinecode1(string mnemonic,int prog,string operand);
string Hex(int num);
void identifyunused();
void logfile();
void listfile();
void objfile();

int main(int argc,char** argv){
    initialize();
    if(argc != 2){
        cout << "Incorrect file type!!   try .asm files ";
    }
    fstream fileptr;
	fileptr.open(argv[1],ios::in);
	fileName = "";
	
	for(int i=0;i<strlen(argv[1]);i++){
		if(argv[1][i] == '.')
			break;
		fileName += argv[1][i];
	}
	
	if(fileptr.is_open())
	{
		int line_num = 1;
		int program_counter = 0;
		string curr_line;

		// Read Line by Line and extract useful data
		while(getline(fileptr,curr_line))
		{
			string s;
			for(int i=0;i<curr_line.length();i++)
			{
				if(curr_line[i] == ';')
					break;
				s += curr_line[i];
			}

			// Used to Identify Labels and Common assembly errors
			firstPass(s,line_num,&program_counter);
			line_num++;
		}
	    secondPass();
	    
	}
	else{
	    error.push_back("Unable to open given file, please check format of the file");
	    errorline.push_back(0);
	}
    identifyunused();
	logfile();
	listfile();
    if(error.empty()){
        objfile();
    }
	return 0;
}

void initialize(){
    zero[0].mnemonic_0="add";
    zero[0].op_0=6;
    zero[1].mnemonic_0="sub";
    zero[1].op_0=7;
    zero[2].mnemonic_0="shl";
    zero[2].op_0=8;
    zero[3].mnemonic_0="shr";
    zero[3].op_0=9;
    zero[4].mnemonic_0="a2sp";
    zero[4].op_0=11;
    zero[5].mnemonic_0="sp2a";
    zero[5].op_0=12;
    zero[6].mnemonic_0="return";
    zero[6].op_0=14;
    zero[7].mnemonic_0="HALT";
    zero[7].op_0=18;
    
    one[0].mnemonic_1="ldc";
    one[0].op_1=0;
    one[1].mnemonic_1="adc";
    one[1].op_1=1;
    one[2].mnemonic_1="ldl";
    one[2].op_1=2;
    one[3].mnemonic_1="stl";
    one[3].op_1=3;
    one[4].mnemonic_1="ldnl";
    one[4].op_1=4;
    one[5].mnemonic_1="stnl";
    one[5].op_1=5;
    one[6].mnemonic_1="adj";
    one[6].op_1=10;
    
    one[7].mnemonic_1="SET";
    one[7].op_1=19;
    one[8].mnemonic_1="data";
    one[8].op_1=20;
    
    two[0].mnemonic_2="call";
    two[0].op_2=13;
    two[1].mnemonic_2="brz";
    two[1].op_2=15;
    two[2].mnemonic_2="brlz";
    two[2].op_2=16;
    two[3].mnemonic_2="br";
    two[3].op_2=17;
}

void firstPass(string store,int linenum,int* PC){
    if(store!=""){
        //cout << "Calling line number " << linenum << "  Current PC " << (*PC) <<endl;
        string s;
        for(int i=0;i<store.length();i++){
            s+=store[i];
            if(store[i]==':')
                s += ' ';
        }
        stringstream brk(s);
        string words[3]={"","",""};
        int lengthw[3]={0,0,0};
        int i=0;
        while(brk >> words[i]){
            lengthw[i]=words[i].length();
            i++;
        }
               // checking label
        if(words[0][lengthw[0]-1]==':'){
            string labelName = words[0].substr(0,lengthw[0]-1);
            if(!labelvalidity(labelName)){
                error.push_back("Invalid Label! at line num ");
                errorline.push_back(linenum);
            }
            else{
                if(searchLabel(labelName)==1){
                    error.push_back("Duplicate Label !! Label already exists at line num ");
                    errorline.push_back(linenum);
                }
                else{
                    labels.push_back(labelName);
                    labPC.push_back(*PC);
                    if(words[1]=="SET"){
                        string rat1=Hex(stoi(words[2]));
                        int mask=6-rat1.length();
                        string temp="";
                        for(int i=0;i<mask;i++){
                            char c='0';
                            temp=temp+c;
                        }
                        temp=temp+rat1;
                        SETlabels.push_back(labelName);
                        SETvalue.push_back(temp);
                        usedlabels.push_back(labelName);
                    }    
                }
            }
            if(words[1]!=""){
                if(search0(words[1])!=0){
                    if(words[2]!=""){
                        error.push_back("Unexpected Operand, No Operand is allowed at line num ");
                        errorline.push_back(linenum);
                    }
                    else{
                        source_code[(*PC)].push_back(words[1]);
                        (*PC)++;
                    }
                }
                else if(search1(words[1])!=0){
                    if(words[2]==""){
                        error.push_back("No Operand found at line num ");
                        errorline.push_back(linenum);
                    }
                    else{
                        source_code[(*PC)].push_back(words[1]);
                        source_code[(*PC)].push_back(words[2]);
                        (*PC)++;
                    }
                }
                else{
                    error.push_back("No such as instruction!! at line numn");
                    errorline.push_back(linenum);
                }
            }
        }
        else{
            //cout << "searching" <<words[0] << " " << search0(words[0]) << " " << search1(words[0]) <<endl;
            if(search0(words[0])!=0){
                if(words[1]!=""){
                    error.push_back("Unexpected Operand, No Operand is allowed at line num ");
                    errorline.push_back(linenum);
                }
                else{
                    source_code[(*PC)].push_back(words[0]);
                    (*PC)++;
                }
            }
            else if(search1(words[0])!=0){
                if(words[2]!=""){
                    error.push_back("Unexpected Operand, No Operand is allowed at line num ");
                    errorline.push_back(linenum);
                }
                else if(words[1]==""){
                    error.push_back("No Operand found at line num ");
                    errorline.push_back(linenum);
                }
                else{
                    source_code[(*PC)].push_back(words[0]);
                    source_code[(*PC)].push_back(words[1]);
                    (*PC)++;
                }
            }
            else{
                error.push_back("No such instruction! at line num ");
                errorline.push_back(linenum);
            }
        }
    }

}

bool labelvalidity(string label){
	if(label[0] >= '0' and label[0] <= '9')
		return false;	

	for(int i=0;i<label.length();i++){
		if( !((label[i] >= 'a' && label[i] <= 'z') || (label[i] >= 'A' && label[i] <= 'Z') || (label[i] >= '0' && label[i] <= '9') || (label[i] == '_')) )
			return false;
	}
	return true;
}

int searchLabel(string label){
    for(int i=0;i<labels.size();i++){
        if(label == labels[i]){
            return 1;
        }
    }
    return 0;
}

int search0(string mnemonic){
    for(int i=0;i<8;i++){
        if(mnemonic == zero[i].mnemonic_0){
            return 1;
        }
    }
    return 0;
}

int search1(string mnemonic){
    for(int i=0;i<9;i++){
        if(mnemonic == one[i].mnemonic_1){
            return 1;
        }
    }
    for(int i=0;i<4;i++){
        if(mnemonic == two[i].mnemonic_2){
            return 2;
        }
    }
    return 0;
}

void secondPass(){
    for(int i=0;i<100000;i++){
        if(source_code[i].empty())
            break;
        if(source_code[i].size()==1){
            string mnemonic = source_code[i][0];
            producemachinecode(0,mnemonic,i);
        }
        else{
            string mnemonic = source_code[i][0];
            string operand = source_code[i][1];
            producemachinecode1(mnemonic,i,operand);
        }
    }
}

void producemachinecode(int instrtype,string mnemonic,int prog){
    for(int i=0;i<labPC.size();i++){
        if(labPC[i]==prog){
            string l=labels[i]+':';
            dataline[prog].push_back(l);
        }
    }
    if(instrtype == 0){
        int opcode;
        for(int i=0;i<8;i++){
            if(mnemonic==zero[i].mnemonic_0){
                opcode=zero[i].op_0;
                break;
            }
        }
        string temp=Hex(opcode);
        string mc;
        if(opcode<16){
            mc="0000000";
            mc=mc+temp;
        }
        else{
            mc="000000";
            mc=mc+temp;
        }
        machinecode.push_back(mc);
        dataline[prog].push_back(mnemonic);
        temp=Hex(prog);
        string pr="";
        for(int i=0;i<(8-temp.length());i++){
            string a="0";
            pr=pr+a;
        }
        pr=pr+temp;
        counter.push_back(pr);
    }
}

void producemachinecode1(string mnemonic,int prog,string operand){
    for(int i=0;i<labPC.size();i++){
        if(labPC[i]==prog){
            string l=labels[i]+':';
            dataline[prog].push_back(l);
        }
    }
    string mc;
    if(search1(mnemonic)==1){
        if(mnemonic == "SET"){
            if(operand[0]==0 && (operand[1]=='x' || operand[1]=='X')){
                mc="";
                int mask=8-(operand.length()-2);
                for(int i=0;i<mask;i++){
                    char c='0';
                    mc+=c;
                }
                string temp=operand.substr(2,(operand.length()-2));
                mc = mc +temp;
            }
            else{
                string temp=Hex(stoi(operand));
                mc="";
                int mask=(8-temp.length());
                for(int i=0;i<mask;i++){
                    char c='0';
                    mc+=c;
                }
                mc = mc+temp;
            }
        }
        else if(mnemonic == "data"){
            if(operand[0]==0 && (operand[1]=='x' || operand[1]=='X')){
                mc="";
                int mask=8-(operand.length()-2);
                for(int i=0;i<mask;i++){
                    char c='0';
                    mc+=c;
                }
                for(int i=2;i<operand.length();i++){
                    mc = mc+operand[i];
                }
            }
            else{
                string temp=Hex(stoi(operand));
                mc="";
                int mask=(8-temp.length());
                for(int i=0;i<mask;i++){
                    char c='0';
                    mc+=c;
                }
                mc = mc+temp;
            }
        }
        else{
            int form = operandtype(operand);
            string temp;
            string opcode;
            int opc;
            char c='0';
            if(form == 6){
                error.push_back("Invalid Number Format in PC ");
                errorline.push_back(prog);
                mc = "00000000";
            }
            else if(form == 1){
                error.push_back("Given label doesn't exist in PC ");
                errorline.push_back(prog);
                mc = "00000000";
            }
            else if(form == 0){
                int k=0;
                string sett="";
                for(int i=0;i<SETlabels.size();i++){
                    if(operand==SETlabels[i]){
                        k=1;
                        sett+=SETvalue[i];
                    }
                }
                if(k==1){
                    temp=sett;
                }
                else{
                    temp="";
                    int pc;
                    for(int i=0;i<labels.size();i++){
                        if(operand==labels[i]){
                            usedlabels.push_back(labels[i]);
                            pc=labPC[i];
                        }
                    }
                    string rat=Hex(pc);
                    int mask=6-rat.length();
                    for(int i=0;i<mask;i++){
                        char c='0';
                        temp=temp+c;
                    }
                    temp=temp+rat;
                    usedlabels.push_back(operand);
                }
            }
            else if(form == 2){
                string oper=operand.substr(1,operand.length()-1);
                string rat=Hex(stoi(oper));
                int mask=6-rat.length();
                temp="";
                for(int i=0;i<mask;i++){
                    temp=temp+c;
                }
                temp=temp+rat;
            }
            else if(form == 3){
                temp="";
                string rat=Hex(stoi(operand));
                for(int i=2;i<rat.length();i++){
                    temp=temp+rat[i];
                }
            }
            else if(form == 4){
                string rat=Hex(stoi(operand));
                int mask=6-rat.length();
                temp="";
                for(int i=0;i<mask;i++){
                    temp=temp+c;
                }
                temp=temp+rat;
            }
            else{
                string oper = operand.substr(2,operand.length()-2);
                temp = "";
                int mask=6-oper.length();
                for(int i=0;i<mask;i++){
                    temp=temp+c;
                }
                temp+=oper;
            }
            for(int i=0;i<9;i++){
                if(mnemonic==one[i].mnemonic_1){
                    opc=one[i].op_1;
                }
            }
            string rat1=Hex(opc);
            opcode="";
            if(rat1.length()==1){
                opcode=opcode+c;
                opcode+=rat1;
            }
            else{
                opcode+=rat1;
            }
            mc=temp+opcode;
        }
    }
    if(search1(mnemonic)==2){
        if(searchLabel(operand)==0){
            error.push_back("Label doesn't exist at PC ");
            errorline.push_back(prog);
        }
        else{
            int nextpc;
            for(int i=0;i<labels.size();i++){
                if(operand==labels[i]){
                    usedlabels.push_back(labels[i]);
                    nextpc=labPC[i];
                    break;
                }
            }
            int offset=(nextpc-(prog+1));
            string rat=Hex(offset);
            string temp="";
            if(rat.length()==8){
                for(int i=2;i<8;i++){
                    temp=temp+rat[i];
                }
            }
            else{
                int mask=6-(rat.length());
                for(int i=0;i<mask;i++){
                    char c='0';
                    temp=temp=temp+c;
                }
                temp=temp+rat;
            }
            int opc;
            for(int i=0;i<4;i++){
                if(mnemonic==two[i].mnemonic_2){
                    opc=two[i].op_2;
                }
            }
            string rat1=Hex(opc);
            string opcode="";
            if(rat1.length()==1){
                char c='0';
                opcode=opcode+c;
                opcode+=rat1;
            }
            else{
                opcode+=rat1;
            }
            mc=temp+opcode;
        }
    }
    machinecode.push_back(mc);
    dataline[prog].push_back(mnemonic);
    dataline[prog].push_back(operand);
    string tempo=Hex(prog);
    string pr="";
    for(int i=0;i<(8-tempo.length());i++){
        string a="0";
        pr=pr+a;
    }
    pr=pr+tempo;
    counter.push_back(pr);
}

string Hex(int num){
	map<int, char> m;

	char digit = '0';
	char c = 'A';

	for (int i = 0; i <= 15; i++) {
		if (i < 10) {
			m[i] = digit++;
		}
		else {
			m[i] = c++;
		}
	}

	string res = "";

	if (!num) {
		return "0";
	}
	
	if (num > 0) {
		while (num) {
			res = m[num % 16] + res;
			num /= 16;
		}
	}
	
	else {
		unsigned int n = num;

		while (n) {
			res = m[n % 16] + res;
			n /= 16;
		}
	}

	return res;
}
 
int operandtype(string operand){
    //cout << "operand that is checked is " << operand << endl;
    if(labelvalidity(operand) && searchLabel(operand)){
        return 0;
    }
    if(labelvalidity(operand) && !(searchLabel(operand))){
        return 1;
    }
    if(operand[0]=='+'){
        for(int i=1;i<operand.length();i++){
        if(!(operand[i] >= '0' and operand[i] <= '9'))
            return 6;
        }
        return 2;
    }
    if(operand[0]=='-'){
        for(int i=1;i<operand.length();i++){
        if(!(operand[i] >= '0' and operand[i] <= '9'))
            return 6;
        }
        return 3;
    }
    if(operand[0]=='0' && (operand[1]=='x' || operand[1]=='X')){
        for(int i=2;i<operand.length();i++){
		    if( !((operand[i] >= 'a' and operand[i] <= 'f') or (operand[i] >= 'A' and operand[i] <= 'F') or (operand[i] >= '0' and operand[i] <= '9')) )
			return 6;
	    }
        return 5;
    }
    for(int i=0;i<operand.length();i++){
        if(!(operand[i] >= '0' and operand[i] <= '9'))
            return 6;
    }
    return 4;
}

void identifyunused(){
    for(int i=0;i<labels.size();i++){
        int k=0;
        for(int j=0;j<usedlabels.size();j++){
            if(labels[i]==usedlabels[j]){
                k=1;
            }
        }
        if(k==0){
            unusedlabels.push_back(labels[i]);
        }
    }
}

void logfile(){
    string s = fileName;
	s += ".log";
	fstream log_fptr;
	log_fptr.open(s,ios::out);
	if(log_fptr.is_open()){
	    log_fptr << "LabelName \t=>\t Value\n";
	    for(int i=0;i<labels.size();i++){
	        log_fptr << labels[i] << "\t=>\t" << labPC[i] << "\n";
	    }
	    log_fptr << "\nUnused Labels \n";
	    for(int i=0;i<unusedlabels.size();i++){
	        log_fptr << unusedlabels[i] <<"\n";
	    }
	    log_fptr << "\nErrors \n";
	    for(int i=0;i<error.size();i++){
	        log_fptr << error[i] << errorline[i] << "\n";
	    }
	}
	
	
	log_fptr.close();
}

void listfile(){
    fstream list_fptr;
	string s = fileName;
	s += ".lst";
	list_fptr.open(s,ios::out);

	if(list_fptr.is_open()){
	    for(int i=0;i<counter.size();i++){
	        list_fptr << counter[i] << "\t" << machinecode[i] << "\t";
	        for(int j=0;j<dataline[i].size();j++){
	            list_fptr << dataline[i][j] << "\t";
	        }
	        list_fptr << "\n";
	    }
	}
	list_fptr.close();
}

void objfile(){
    string s = fileName;
	s += ".o";
	ofstream output;
    output.open(s,ios::binary);
    for(auto ito=machinecode.begin();ito!=machinecode.end();ito++){
         string str = *ito;
         for(auto i=str.begin();i!=str.end();i++){
            char x=*i;
            output.write((char *)&x , sizeof(x));
         }
    }
}