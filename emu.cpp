#include <bits/stdc++.h>

using namespace std;

int totaltrace=0;
string fileName;
vector<string> machinecode;
string memory[100000];
vector<string> vectorA;
vector<string> vectorB;
vector<string> vectorC;
vector<string> vectorD;

string Hex(int num);
void description(string opcode);
string hexConvertbinary(string s);
string findNum(char c);
string masking(string hex,int num);
int binaryTOdecimal(string s);
void emulate(int A,int B,int SP,int PC);


int main(int argc,char** argv){
    if(argc != 2){
        cout << "Incorrect file type!!   try .o files ";
    }

    fstream fileptr;
	fileptr.open(argv[1],ios::in);

	fileName = "";
	for(int i=0;i<strlen(argv[1]);i++){
		if(argv[1][i] == '.')
			break;
		fileName += argv[1][i];
	}
	
	if(fileptr.is_open()){
        string totalcode;
        while(getline(fileptr,totalcode)){
            //cout << totalcode;
            int i=0;
            while(i<(totalcode.length())){
                string tem=totalcode.substr(i,8);
                machinecode.push_back(tem);
                i=i+8;
            }
        }
    }
    for(int i=0;i<machinecode.size();i++){
        memory[i] = machinecode[i];
    }

    int numb;
    cout << "Enter 1 for memory dump or 2 for trace" << endl;
    cin >> numb;
    if(numb==1){
    cout << "\n Before Memory Dump";
    for(int i=0;i<machinecode.size();i++){
        if(i%4 == 0){
            string temp=masking(Hex(i),8);
            cout << "\n" << temp << "\t";
        }
        cout << machinecode[i] << "\t";
    }
    }   
    else{
        int A=0;
        int B=0;
        int SP=9999;
        int PC=0;
        emulate(A, B, SP, PC);
    }
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

string hexConvertbinary(string s){
    string temp="";
    for(int i=0;i<s.length();i++){
        temp+=findNum(s[i]);
    }
    return temp;
}

string findNum(char c){
    if(c=='0'){
        return "0000";
    }
    else if(c=='1'){
        return "0001";
    }
    else if(c=='2'){
        return "0010";
    }
    else if(c=='3'){
        return "0011";
    }
    else if(c=='4'){
        return "0100";
    }
    else if(c=='5'){
        return "0101";
    }
    else if(c=='6'){
        return "0110";
    }
    else if(c=='7'){
        return "0111";
    }
    else if(c=='8'){
        return "1000";
    }
    else if(c=='9'){
        return "1001";
    }
    else if(c=='a' || c=='A'){
        return "1010";
    }
    else if(c=='b' || c=='B'){
        return "1011";
    }
    else if(c=='c' || c=='C'){
        return "1100";
    }
    else if(c=='d' || c=='D'){
        return "1101";
    }
    else if(c=='e' || c=='E'){
        return "1110";
    }
    else{
        return "1111";
    }
}

string masking(string hex,int num){
    if(num == 6){
        if(hex.length() >= 6){
            return hex.substr(hex.length()-6,6);
        }
        else{
            int mask = 6 - hex.length();
            string temp = "";
            for(int i=0;i<mask;i++){
                temp+='0';
            }
            temp+=hex;
            return temp;
        }
    }
    if(num == 8){
        if(hex.length() >= 8){
            return hex.substr(hex.length()-8,8);
        }
        else{
            int mask = 8 - hex.length();
            string temp = "";
            for(int i=0;i<mask;i++){
                temp+='0';
            }
            temp+=hex;
            return temp;
        }
    }
}

int binaryTOdecimal(string s){
    int n=0;
    for(int i=0;i<s.length();i++){
        if(s[i]=='1'){
            if(i==0){
                n+=((-1)*pow(2,s.length()-1-i));
            }
            else{
                n+=pow(2,s.length()-1-i);
            }
        }
            
    }
    return n;
}


void emulate(int A,int B,int SP,int PC){
    vectorA.push_back(masking(Hex(A),8));
    vectorB.push_back(masking(Hex(B),8));
    vectorC.push_back(masking(Hex(SP),8));
    vectorD.push_back(masking(Hex(PC),8));
    cout << "PC = " << masking(Hex(PC),8) << "    " << "SP = " << masking(Hex(SP),8) << "    " << "A = " << masking(Hex(A),8) << "    " << "B = " << masking(Hex(B),8) << "    ";
    if(PC>=machinecode.size() || PC<0){
        cout << "Terminating loop since no instruction to load" << endl;
        exit(0);
    }
    string instr=machinecode[PC];
    string opcode=instr.substr(6,2);
    string operand=instr.substr(0,6);
    int valueORoffset=binaryTOdecimal(hexConvertbinary(operand));
    if(opcode=="00"){
        B = A;
        A = valueORoffset;
        cout << "ldc " << A << endl;
    }
    else if(opcode=="01"){
        A = A + valueORoffset;
        cout << "adc " << valueORoffset << endl;
    }
    else if(opcode=="02"){
        B = A;
        A = binaryTOdecimal(hexConvertbinary(memory[SP + valueORoffset]));
        cout << "ldl " << valueORoffset << endl;
    }
    else if(opcode=="03"){
        memory[SP + valueORoffset] = masking(Hex(A),8);
        A = B;
        cout << "stl " << valueORoffset << endl;
    }
    else if(opcode=="04"){
        A = binaryTOdecimal(hexConvertbinary(memory[A + valueORoffset]));
        cout << "ldnl " << valueORoffset << endl;
    }
    else if(opcode=="05"){
        memory[SP + valueORoffset] = masking(Hex(B),8);
        cout << "stnl " << valueORoffset << endl;
    }
    else if(opcode=="06"){
        A = B + A;
        cout << "add" << endl;
    }
    else if(opcode=="07"){
        A = B - A;
        cout << "sub" << endl;
    }
    else if(opcode=="08"){
        A = B << A;
        cout << "shl" << endl;
    }
    else if(opcode=="09"){
        A = B >> A;
        cout << "shr" << endl;
    }
    else if(opcode=="0a" || opcode =="0A"){
        SP = SP + valueORoffset;
        cout << "adj " << valueORoffset << endl;
    }
    else if(opcode=="0b" || opcode =="0B"){
        SP = A;
        A = B;
        cout << "a2sp " << endl;
    }
    else if(opcode=="0c" || opcode =="0C"){
        B = A;
        A = SP;
        cout << "sp2a " << endl;
    }
    else if(opcode=="0d" || opcode =="0D"){
        if(valueORoffset==-1){
            cout <<"\n Infinite Loop Branching. Terminating code";
            exit(1);
        }
        B = A;
        A = PC;
        PC = PC + valueORoffset;
        cout << "call" << valueORoffset << endl;
    }
    else if(opcode=="0e" || opcode =="0E"){
        PC = A;
        A = B;
        cout << "return" << endl;
    }
    else if(opcode=="0f" || opcode =="0F"){
        if(A==0){
            if(valueORoffset==-1){
                cout <<"\n Infinite Loop Branching. Terminating code";
                exit(1);
            }
            PC = PC + valueORoffset;
        }
        cout << "brz " << valueORoffset << endl;
    }
    else if(opcode=="10"){
        if(A<0){
            if(valueORoffset==-1){
                cout <<"\n Infinite Loop Branching. Terminating code" << endl;
                //cout << "Number of instruction executed =" << totaltrace;
                exit(1);
            }
            PC = PC + valueORoffset;
        }
        cout << "brlz " << valueORoffset << endl;
    }
    else if(opcode=="11"){
        if(valueORoffset==-1){
            cout <<"\n Infinite Loop Branching. Terminating code";
            exit(1);
        }
        PC = PC + valueORoffset;
        cout << "br " << valueORoffset << endl;
    }
    else if(opcode=="12"){
        cout << "HALT" <<endl;
        //cout << "Number of instruction executed =" << totaltrace;
        exit(0);
    }
    else{
        cout << "Couldn't Identify instruction!" << endl;
    }
    totaltrace++;
    PC++;
    emulate(A,B,SP,PC);
}