#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <bitset> //bitset is similar to bitprint, but you can choose how many bits you want printed. Made printing tag values easier

/*

//////////NOTICE\\\\\\\\\\\\\ 
-----> This program reads a txt file by inputing the input-object-code.txt file into source files on visual studio. It uses ifstream to open it <-----------

In this program, a two way associative cache is simulated using a struct comprised of [8] rows and [2] columns.
Memory is a [128] size array.
Registers are [8] size array.
Starting off, a txt file is read using getline. The data is then parsed by each newline, allowing the program to read 32 bits at a time.
Opcode, RS, RT, and Byte Offset is pulled out from the data. Byte Address then Word Address is calculated, follwed by Set Number and Tag.
The main operation (hit/miss) then starts. The cache is searched using the Index (Set Number), then valid bit is checked, and finally Tag
is checked. If all this adds up, you have a hit. Otherwise, it is a miss. The algorithm breaks down what happens in either case wether you are
using LW or SW. After this, the driver prints out all the Input object code and if it hit or missed. It then prints out the cache, followed by registers and
finally memory. The program then ends.


----------------I/O DESCRIPTION-------------
The only input is a object code file, of which has 24 32-bit binary numbers. These contain Opcode, RS, RT, and Byte Offset values. The program parses this data then applies the cache
algorithm to detect hits and misses, and the simulation of how caches really work. The output is the results of the cache, registers, hit/miss, and memory.
*/




using namespace std;

//headers to fix a few placement issues
int binToDec(int n);
int* decToBin(int n);
void reverseSTR(string& str);

//global variables

struct block{ //block struct for cache
	int valid;
	int tag;
	int data;
	int hit;
	int index;
	int raw[8][3];
	int LRU;
};
//cache
block block_0[8][2];

struct mem { //memory array
	int data;
};
//memory
mem memory[128];

struct cpuReg { //register array
	int data;
};
//registers
cpuReg reg_file[8];


//This function displays an integer number as 32 bits.      
void bitprint(int v){
	int mask = 1;                          //mask = 00000000000000000000000000000001
	mask <<= 31;  //shift left 31 positions: mask = 10000000000000000000000000000000
	for (int i = 1; i <= 32; i++)
	{
	if ((v & mask) == 0)   //check the leftmost bit only              
		cout<<"0";       
	else              
		cout << "1";                     
	v <<= 1;  //shift left 1 bit
	}       
	}      

int hitMiss(int index, int tag, int data, int opcode, int rt) { //hit or miss function plus cache actions for LW and SW
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 2; j++) {
			if (block_0[i][j].index == index) {
				if (block_0[i][j].valid == 1) {
					if (block_0[i][j].tag == tag) {//hit!
						if (opcode == 35) { //load
							reg_file[rt].data = block_0[i][j].data; //stores data into registers from cache
						}
						else if (opcode == 43) { //store
							block_0[i][j].data = reg_file[rt].data; //loads data from registers into cache
						}
						else {
							cout << "Error in logic (Opcode is not 35 or 43), check opcode function" << endl;
							return 0;
						}

						block_0[i][j].hit = 1; //check for hit
						block_0[i][j].LRU = 1; //marks most recently used block


						if ((j + 1) == 2) {
							block_0[i][0].LRU = 0; //marks least recently used block
							return 0;
						}
						else {
							block_0[i][1].LRU = 0; //marks least recently used block
							return 0;
						}
					}
				}
			}
		}
	}
	if (opcode == 35) { //load
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 2; j++) {
				if (block_0[i][j].index == index) {
					if ((block_0[i][j].LRU == 1) && ((j + 1) == 2)) {//j is 0 (LRU)
						
						if (block_0[i][j].valid == 1) {//dirty bit
							memory[((tag * 8) + index)].data = data; //finds memory address and updates data
						}
						block_0[i][0].data = memory[((tag * 8) + index)].data; //memory[((tag * 8) + index)].data is same as data
						block_0[i][0].valid = 1;
						block_0[i][0].tag = tag;
						block_0[i][0].LRU = 1;
						block_0[i][1].LRU = 0;
						reg_file[rt].data = block_0[i][0].data; //puts cache data in register
						//return 0;
						
					}
					else if ((block_0[i][j].LRU == 1) && ((j + 1) == 1)) {//j is 1 (LRU)
						
						if (block_0[i][j].valid == 1) { //dirty bit
							memory[((tag * 8) + index)].data = block_0[i][1].data; //finds memory address and updates data
						}
						block_0[i][1].data = memory[((tag * 8) + index)].data; //memory[((tag * 8) + index)].data is same as data
						block_0[i][1].valid = 1;
						block_0[i][1].tag = tag;
						block_0[i][1].LRU = 1;
						block_0[i][0].LRU = 0;
						reg_file[rt].data = block_0[i][1].data; //puts cache data in register
						//return 0;
					}
					else { //if there is no least recently used bit
						if (block_0[i][j].valid == 1) { //dirty bit
							memory[((tag * 8) + index)].data = block_0[i][1].data; //finds memory address and updates data
						}
						block_0[i][0].data = memory[((tag * 8) + index)].data; //memory[((tag * 8) + index)].data is same as data
						block_0[i][0].valid = 1;
						block_0[i][0].tag = tag;
						block_0[i][0].LRU = 1;
						block_0[i][1].LRU = 0;
						reg_file[rt].data = block_0[i][0].data; //puts cache data in register
						//return 0;
					}
				}
			}
		}
		return 0;
	}
	else if (opcode == 43) { //store
		memory[((tag * 8) + index)].data = reg_file[rt].data;
		return 0;
	}
	else {
		cout << "Error in logic (Opcode is not 35 or 43), check opcode function" << endl;
			return 0;
	}
}

string opcode(int x) { //opcode string and checker
	string s;
	int y;
	y = binToDec(x);
	if (y == 35) {
		s = "lw";
		return s;
	}
	else if (y == 43) {
		s = "sw";
		return s;
	}
	else {
		cout << "Opcode Error!" << endl;
		s = "ERROR";
		return s;
	}
}

int rs(string line) { //rs finder
	int rsINT;
	string rsSTR;
	rsSTR = line.substr(6, 5);
	reverseSTR(rsSTR); //reverse binary digits to make them correctly ordered
	stringstream temp(rsSTR);
	temp >> rsINT;
	return binToDec(rsINT);
}

int rt(string line) { //rt finder
	int rtINT;
	string rtSTR;
	rtSTR = line.substr(11, 5);
	//reverseSTR(rtSTR);
	stringstream temp(rtSTR);
	temp >> rtINT;
	return binToDec(rtINT);
}

void reverseSTR(string& str) { //reverses a string (for pulling out binary values)
	int n = str.length();

	for (int i = 0; i < n / 2; i++)
		swap(str[i], str[n - i - 1]);
}

int byte_offset(string line) { //byte offset finder
	int byte_offsetINT;
	string byte_offsetSTR;
	byte_offsetSTR = line.substr(16, 16);
	stringstream temp(byte_offsetSTR);
	temp >> byte_offsetINT;
	return byte_offsetINT;
}

int addr(int rs, int offset) { //address finder
	int temp1, temp2, address;
	temp1 = rs;
	temp2 = binToDec(offset); //convert to decimal to find word address

	address = temp1 + temp2;
	address = address / 4; //word address
	return address;
}

int binToDec(int n) { //binary to decimal
	int num = n;
	int dec_val = 0;

	int base = 1;
	int temp = num;
	while (temp) {
		int last_digit = temp % 10;
		temp = temp / 10;

		dec_val += last_digit * base;
		base = base * 2;
	}
	return dec_val;
}

int* decToBin(int n) { //decimal to binary
	int binNum[32];
	int i = 0;
	while (n > 0) {

		binNum[i] = n % 2;
		n = n / 2;
		i++;
	}
	return binNum;
}

void initialize() { //fills registers, cache, and mem with 0 and initializes all data structures with data
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 3; j++) {
			//data
			block_0[i][j].data = 0;
			//index
			block_0[i][j].index = i;
			//tag
			block_0[i][j].tag = 0;
			//valid bit
			block_0[i][j].valid = 0;
			//LRU bit (for checking)
			block_0[i][j].LRU = 1;
			//hit (1 yes 0 no)
			block_0[i][j].hit = 0;
		}
	}
	for (int i = 0; i < 129; i++) {
		memory[i].data = i + 5; //memory initialization
	}
	for (int i = 0; i < 7; i++) {
		reg_file[i].data = 0; //register initialization
	}
}


string stringGenerator(int* n, int size) { //turns int of size n into a string (ex: [1,0,1,1] = "1011")
	//this function is necessary because stoi and stringstream omit the 0's before any number
	//for ex: 0000101 = 101. While this may not seem bad, it makes printing the data more difficult
	string s;
	char c;
	int j;
	for (int i = 0; i < size; i++) {
		if ((n[i] < 0) || (n[i] > 1)) {
			j = 0;
			c = j + 48;
			s.push_back(c);
		}
		else {
			j = n[i];
			c = j + 48;
			s.push_back(c);
		}
	}
	return s;
}


tuple <int, string> setNum(int address) { //set# finder function
	int setNum, temp, setNumArrI, variable[4];
	int j = 3;
	int* setNumArr;
	string ss;
	temp = address;
	setNum = temp % 8;
	setNumArr = decToBin(setNum);
	for (int i = 0; i < 4; i++) {
		if ((*(setNumArr + j) < 0) || (*(setNumArr + j) > 1)) {
			variable[i] = 0;
		}
		else if ((*(setNumArr+j)) == 0) {
			variable[i] = 0;
		}
		else {
			variable[i] = 1;
		}

		j--;
	}
	ss = stringGenerator(variable, 4); //turns int[4] into string
	return make_tuple(setNum, ss); //returns tuple of set number (INT) and the set number (string)
}

tuple<int, string> tag(int address) { //tag finder function
	int tagI, temp, tagArrI, variable[3];
	int* tagArr;
	int j = 2;
	string ss;
	temp = address;
	tagI = temp / 8; //find Tag
	tagArr = decToBin(tagI); //convert to binary list
	for (int i = 0; i < 3; i++) {
		if ((*(tagArr + j) < 0) || (*(tagArr + j) > 1)) {
			variable[i] = 0;
		}
		else if ((*(tagArr + j)) == 0) {
			variable[i] = 0;
		}
		else {
			variable[i] = 1;
		}

		j--;
	}
	ss = stringGenerator(variable, 3); //turns int[3] into string
	return make_tuple(tagI, ss); //returns tuple of tag (INT) and tag (string)
}

int bit_print(string line, int index) { //prints each line along with hit or miss
	int check = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 2; j++) {
			if(block_0[i][j].index == index)
				if (block_0[i][j].hit == 1) { //checks .hit in struct block to see if it was a hit or miss
					cout << line << " " << "Hit" << endl;
					return 0;
				}
				else {
					cout << line << " " << "Miss" << endl;
					return 0;
				}
		}
	}
}

void cache_print() { //prints caches
	string s;
	int* tagI;
	int* dataI;
	cout << "\nCache Block 0                                                  Cache Block 1" << endl;
	cout << "Set#     Valid     Tag     Data                                     Set#     Valid     Tag     Data" << endl;
	for (int i = 0; i < 8; i++) {
		cout << i << "        " << block_0[i][0].valid << "         ";
		bitset<4> y(block_0[i][0].tag);
		cout << y << "    ";
		bitprint(block_0[i][0].data);
		
		tagI = decToBin(block_0[i][1].tag);
		dataI = decToBin(block_0[i][1].data);
		cout << "         " << i << "        " << block_0[i][1].valid << "         ";
		bitset<4> z(block_0[i][1].tag);
		cout << z << "    ";
		bitprint(block_0[i][1].data);
		cout << endl;
	}
}

void reg_print() { //prints registers
	cout << "\nRegisters" << endl;
	bitset<32> a(reg_file[0].data);
	bitset<32> b(reg_file[1].data);
	bitset<32> c(reg_file[2].data);
	bitset<32> d(reg_file[3].data);
	bitset<32> e(reg_file[4].data);
	bitset<32> f(reg_file[5].data);
	bitset<32> g(reg_file[6].data);
	bitset<32> h(reg_file[7].data);
	cout << "$S0      " << a << endl;
	cout << "$S1      " << b << endl;
	cout << "$S2      " << c << endl;
	cout << "$S3      " << d << endl;
	cout << "$S4      " << e << endl;
	cout << "$S5      " << f << endl;
	cout << "$S6      " << g << endl;
	cout << "$S7      " << h << endl;
	cout << endl;
}

void main_memPrint() { //prints memory
	cout << "Memory   Data" << endl;
	for (int i = 0; i < 128; i++) {
		bitset<32> a(memory[i].data);
		cout << i << "        " << a << endl;
	}
}

int main() //driver
{
	string line, opcodeSTR, indexS, tagS;
	char newline = '\r';
	int opcodeINT, rsINT, rtINT, byte_offsetINT, address_INT[4], address_I, indexI, tagI;
	int address;
	ifstream myfile("Input-object-code.txt");
	initialize();
	if (myfile.is_open()) {
		while (!myfile.eof()) {
			getline(myfile, line, '\r');
			//opcode
			opcodeSTR = line.substr(0, 6);
			stringstream temp(opcodeSTR);
			temp >> opcodeINT;
			opcodeSTR = opcode(opcodeINT); //opcode str is not needed for display purpose but is left in to check if program
			//is reading the binary code correctly
			//rs & rt
			rsINT = rs(line);
			rtINT = rt(line);
			//byte_offset
			byte_offsetINT = byte_offset(line); //calculates the word address

			address = addr(rsINT, byte_offsetINT);  //get word address
			//make tuple for index (string for printing and int for data) = setNum(address_I);
			//ironically the tuples are not needed (I do not need the strings) as I forgot that struct block has the index and tag
			//in it, but I left it in just for reference
			tie(indexI, indexS) = setNum(address); // get set#

			tie(tagI,tagS)  = tag(address); //get tag

			//hit or miss plus cache actions
			hitMiss(indexI, tagI, binToDec(byte_offsetINT), binToDec(opcodeINT), binToDec(rtINT));


			bit_print(line, indexI); //prints out each txt file line followed by wether you got a hit or miss
			}
		myfile.close();
		//print all remaining data structures
		cache_print();
		reg_print();
		main_memPrint();
	}
	else
		cout << "Unable to Open File" << endl;
	}


