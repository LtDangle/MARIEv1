#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <Windows.h>

///////////////Preexisting Containers////////////////
std::string One = "0001", Zero = "0000", Zero3 = "000";
std::string IOPrompt = ">> ";
std::string InputFile;
bool ProgramFilled = false;
bool DebugMode = false;
bool AssignMapFail = false;
//For "Fancy Stuff" at bottom
unsigned int NumericBase = 10;
bool Terminated = false;

std::vector<std::string>ProgramLines;
std::map<std::string, std::string>Label;
std::map<int8_t, int8_t>CharToInt = {
	{'0', 0},
	{'1', 1},
	{'2', 2},
	{'3', 3},
	{'4', 4},
	{'5', 5},
	{'6', 6},
	{'7', 7},
	{'8', 8},
	{'9', 9},
	{'A', 10}, {'a', 10},//////so as not to have to have to make input uppercase
	{'B', 11}, {'b', 11},
	{'C', 12}, {'c', 12},
	{'D', 13}, {'d', 13},
	{'E', 14}, {'e', 14},
	{'F', 15}, {'f', 15},
	{'G', 16},
	{'H', 17},
	{'I', 18},
	{'J', 19},
	{'K', 20},
	{'L', 21},
	{'M', 22},
	{'N', 23},
	{'O', 24},
	{'P', 25},
	{'Q', 26},
	{'R', 27},
	{'S', 28},
	{'T', 29},
	{'U', 30},
	{'V', 31},
	{'W', 32},
	{'X', 33},
	{'Y', 34},
	{'Z', 35}
};
char IntToChar[36] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y'};
std::map<std::string, char>InstructionToChar = {
	{"JNS", '0'},
	{"LOAD", '1'},
	{"STORE", '2'},
	{"ADD", '3'},
	{"SUBT", '4'},
	{"INPUT", '5'},
	{"OUTPUT", '6'},
	{"HALT", '7'},
	{"SKIPCOND", '8'},
	{"JUMP", '9'},
	{"CLEAR", 'A'},
	{"ADDI", 'B'},
	{"JUMPI" , 'C'},
	{"LOADI", 'D'},
	{"STOREI", 'E'}
};

////////////////////////////////////////////////////



///////////////////////////////////Basic Use Functions////////////////////////////////////////
unsigned int HexStringToInt(std::string& x) {
	unsigned int Result = 0, numPlace = 1;
	for (unsigned int i = x.length() - 1; i <= 3; i--) {
		Result += CharToInt[x[i]]*numPlace;
		numPlace *= 16;
	}
	return Result;
}
std::string IntToHexString(unsigned int& x) {
	unsigned int Input = x;
	std::string Result = Zero;
	for (unsigned int i = 3; i <= 3; i--) {
		unsigned int Remainder = Input % 16;
		Result[i] = IntToChar[Remainder];
		Input /= 16;
	}
	return Result;
}
std::string ConvertNumericBase(std::string x,const unsigned int& Base1, const unsigned int& Base2) {
	std::string Num = "";
	unsigned int temp = 0, numPlace = 1, b10Num = 0;

	if (x == "0")
		return x;
	//if (Base1 == Base2)
	//	return x;

	////////////convert to base 10
	while (!x.empty()) {
		b10Num += CharToInt[x.back()]*numPlace;
		numPlace *= Base1;
		x.pop_back();
	}
	/////////////convert to desired base
	while (b10Num != 0) {
		unsigned int temp = b10Num%Base2;
		Num = IntToChar[temp] + Num;
		b10Num /= Base2;
	}
	return Num;
}
std::string StringsToHexString(std::string& A, std::string& B) {
	if (A == "HEX")
		return B;

	const unsigned int Hex = 16;
	unsigned int BaseFrom = 16;
	if (A == "DEC")BaseFrom = 10;
	else if (A == "OCT")BaseFrom = 8;
	else if (A == "BIN")BaseFrom = 2;
	return ConvertNumericBase(B, BaseFrom, Hex);
}

std::string hexAddition(std::string& A, std::string& B) {
	std::string Result = "";
	unsigned int intA = HexStringToInt(A), intB = HexStringToInt(B), Sum = intA + intB;
	Result = IntToHexString(Sum);
	while (Result.length()>A.length())
		Result.erase(0, 1);
	return Result;
}
std::string hexSubtraction(std::string& A, std::string& B) {////hexadecimal subtraction function--returns string thelength of A
	std::string Result = "";
	unsigned int intA = HexStringToInt(A), intB = HexStringToInt(B), Difference = intA - intB;
	Result = IntToHexString(Difference);
	while (Result.length()>A.length())
		Result.erase(0, 1);
	return Result;
}
std::string CapitalizeString(const std::string& x) {
	std::string Result = "";
	for (unsigned int i = 0; i<x.length(); i++) {
		Result += toupper(x[i]);
	}
	return Result;
}

bool isLabel(std::string& x) {////checks map:Label for string:x
	std::map<std::string, std::string>::iterator it = Label.find(x);
	return(it != Label.end());
}
bool isInstruction(std::string& x) {
	return(x == "JNS" ||
		x == "LOAD" ||
		x == "STORE" ||
		x == "ADD" ||
		x == "SUBT" ||
		x == "INPUT" ||
		x == "OUTPUT" ||
		x == "HALT" ||
		x == "SKIPCOND" ||
		x == "JUMP" ||
		x == "CLEAR" ||
		x == "ADDI" ||
		x == "JUMPI" ||
		x == "LOADI" ||
		x == "STOREI");
}
/////////////////////////////////////////////////////////////////////////////////////////////



//////////////////MARIE Registers and Instructions//////////
//PC:Program Counter----------4 char
//IR: Instruction Register----1 char
//MAR:Memory-Address Register-4 char
//MBR:Memory-Buffer Register--4 char but actually depends
//AC:Accumulator--------------4 char
std::string PC, IR, MAR, MBR, AC;
bool Halted = false;
std::map<std::string, std::string>Memory;

//Instructions-----must be declared before Instructions' map
void JnS() {
	//Store the PC at address X and jump to X + 1
	MBR = PC;     //MBR<-PC
				  //MAR=IR.substr(1);//MAR<-X
	Memory[MAR] = MBR;
	MBR = IR.substr(1);//MBR<-X
					   //AC=hexAddition(One,MBR);//AC<-MBR+1
	PC = hexAddition(MBR, One);    //PC<-AC
}
void Load() {
	//Load contents of address X into AC
	AC = MBR;
}
void Store() {
	//Store the contents of AC at address X
	Memory[MAR] = AC;
}
void Add() {
	//Add the contents of address X to AC
	AC = hexAddition(AC, MBR);
}
void Subt() {
	//Subtract the contents of address X from AC
	AC = hexSubtraction(AC, MBR);
}
void Input() {
	//Input a value from the keyboard into AC
	std::string tempInput = "";
	bool WrongBase = false;

	do {
		std::cout << "INPUT: " << std::endl << IOPrompt;
		std::cin >> tempInput;
		WrongBase = false;
		for (unsigned int i = 0; i < tempInput.length(); i++) {
			if (CharToInt[tempInput[i]] >= NumericBase) {
				WrongBase = true;
				break;
			}
		}
		if (WrongBase)
			std::cout << "Base is " << NumericBase << std::endl;
	} while (WrongBase);
	tempInput = CapitalizeString(tempInput);
	//std::cout << tempInput << std::endl;
	tempInput = ConvertNumericBase(tempInput, NumericBase, 16);
	tempInput = hexAddition(Zero, tempInput);
	AC = tempInput;
}
void Output() {
	//Output the value in AC to the display
	std::string tempOutput = AC;
	tempOutput = ConvertNumericBase(tempOutput, 16, NumericBase);
	std::cout << "OUTPUT: " << std::endl << IOPrompt << tempOutput << std::endl;
}
void Halt() {
	//Terminate Program
	Halted = true;
}
void Skipcond() {
	//Skip next instruction on condition
	//Note regarding use of SKIPCOND :
	//The two address bits closest to the opcode field, bits 10 and
	//	11 specify the condition to be tested.If the two address bits
	//	are 00, this translates to "skip if the AC is negative".If the
	//	two address bits are 01, this translates to "skip if the AC is
	//	equal to 0". Finally, if the two address bits are 10 (or 2),
	//	this translates to "skip if the AC is greater than 0".
	//	Example: the instruction Skipcond 800 will skip the
	//	instruction that follows if the AC is greater than 0.
	if (IR[1] >= '0'&&IR[1] <= '3') {
		if (AC[0] == '8' || AC[0] == '9' || AC[0] >= 'A'&&AC[0] <= 'F')
			PC = hexAddition(PC, One);
		return;
	}
	if (IR[1] >= '4'&&IR[1] <= '7') {
		if (AC == "0000")
			PC = hexAddition(PC, One);
		return;
	}
	if (IR[1] == '8' || IR[1] == '9' || IR[1] == 'A' || IR[1] == 'B') {
		if (AC[0] >= '0'&&AC[0] <= '7'&&AC != "0000")
			PC = hexAddition(PC, One);
		return;
	}
	return;
}
void Jump() {
	//Load the value of X into PC
	PC = IR.substr(1);
}
void Clear() {
	//Put all zeros in AC
	AC = Zero;
}
void AddI() {
	//Add Indirect: Use the value at X as the actual 
	//      address of the data operand to add to AC
	MAR = MBR;
	MBR = Memory[MAR];
	AC = hexAddition(AC, MBR);
}
void JumpI() {
	//Use the value X as the address to jump to 
	PC = MBR;
}
void LoadI() {
	//Load Indirect: Use the value at X as the 
	//            address of the value to load
	MAR = MBR;
	MBR = Memory[MAR];
	AC = MBR;
}
void StoreI() {
	//Store Indirect: Use X the value at X as the
	//        address of where to store the value
	MAR = MBR;
	Memory[MAR] = AC;
}
//////////////////////////////
typedef void(*Instruction)();
std::map<char, Instruction>MARIEinstruction = {
	{ '0',(*JnS) },
	{ '1',(*Load) },
	{ '2',(*Store) },
	{ '3',(*Add) },
	{ '4',(*Subt) },
	{ '5',(*Input) },
	{ '6',(*Output) },
	{ '7',(*Halt) },
	{ '8',(*Skipcond) },
	{ '9',(*Jump) },
	{ 'A',(*Clear) },
	{ 'B',(*AddI) },
	{ 'C',(*JumpI) },
	{ 'D',(*LoadI) },
	{ 'E',(*StoreI) }
};
//////////////////////////////////////////////////////////



/////////////////////////////////////////////Pre-MARIE Functions//////////////////////////////////////////////
void PickFile() {
	//std::cout << "Input File:" << std::endl << IOPrompt;
	std::cout << IOPrompt;
	std::cin >> InputFile;
}
void FillProgramLines() {
	std::string line;
	std::ifstream myfile(InputFile);
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
			ProgramLines.push_back(line);
		} 
		myfile.close();
		ProgramFilled = true;
	}
	else {
		std::cout << "Unable to open file." << std::endl;
		ProgramFilled = false;
	}
}
void SearchAndAssignLabels() {
	std::size_t comma, space;
	std::string tempLabel;

	////Finding named variables
	for (unsigned int i = 0; i < ProgramLines.size(); i++) {
		if ((comma = ProgramLines[i].find(',')) != std::string::npos) {
			//space = ProgramLines[i].find(' ');
			tempLabel = ProgramLines[i].substr(0, comma);
			//std::cout << tempLabel << std::endl;
			Label[tempLabel] = hexAddition(Zero3, IntToHexString(i));
			ProgramLines[i].erase(0, comma + 2);
			//std::cout << Label[tempLabel] << ", " << tempLabel << std::endl;
		}
	}
}
void AssignMemoryMap() {
	std::string tempMemory = Zero, PLsub[3];//ProgramLines substrings
	char space = ' ';

	////has to be rewritten to just create the line numbers and not have to write them in the txt file
	//****editted to no longer need file to have line numbers
	PLsub[0] = Zero3;
	for (unsigned int i = 0; i < ProgramLines.size(); i++) {
		std::stringstream Line(ProgramLines[i]);
		std::string checkFormat = "";
		PLsub[1] = PLsub[2] = "";
		//std::getline(Line, PLsub[0], space);
		std::getline(Line, PLsub[1], space);
		std::getline(Line, PLsub[2], space);
		if (std::getline(Line, checkFormat, space)) {
			std::cout << "File not formatted properly" << std::endl;
			AssignMapFail = true;
			break;
		}
		//std::cout << PLsub[0] << ' ' << PLsub[1] << ' ' << PLsub[2] << std::endl;
		PLsub[0]=CapitalizeString(PLsub[0]);////commented to test if required
		PLsub[1]=CapitalizeString(PLsub[1]);
		if(!isLabel(PLsub[2]))
			PLsub[2]=CapitalizeString(PLsub[2]);
		//PLsub[0]=hexAddition(Zero3,PLsub[0]);

		if (i == 0)PC = PLsub[0];
		if (PLsub[2].empty() && !isInstruction(PLsub[1]))
			tempMemory = PLsub[1];
		else if (isInstruction(PLsub[1])) {
			tempMemory[0] = InstructionToChar[PLsub[1]];
			if (!PLsub[2].empty())
				tempMemory.replace(1, 3, (isLabel(PLsub[2]) ? Label[PLsub[2]] : hexAddition(Zero3, PLsub[2])));
			else {
				tempMemory[1] = tempMemory[2] = tempMemory[3] = '0';
			}
		}
		else {
			tempMemory = StringsToHexString(PLsub[1], PLsub[2]);
			tempMemory = hexAddition(Zero, tempMemory);
		}
		Memory[PLsub[0]] = tempMemory;
		//std::cout << PLsub[0] << ' ' << tempMemory << std::endl;////uncomment to see how MARIE code looks after first pass
		PLsub[0] = hexAddition(PLsub[0], One);
	}
	//system("PAUSE");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////Debug/////////////////////
void AskDebugMode() {
	unsigned int Debug = 0;
	std::cout << "Debug Mode will show how registers are affected" << std::endl;
	std::cout << "1: ON\n2: OFF" << std::endl;
	do {
		std::cout << IOPrompt;
		std::cin >> Debug;
		if (Debug != 1 && Debug != 2)
			std::cout << "what..." << std::endl;
	} while (Debug != 1 && Debug != 2);
	DebugMode = (Debug == 1 ? true : false);
}
void ShoweMarie() {
	for (std::map<std::string, std::string>::iterator it = Memory.begin(); it != Memory.end(); ++it)
		std::cout << it->first << ' ' << it->second << '\n';
}
void Registers() {
	std::cout << PC << ' ' << IR << ' ' << MAR << ' ' << MBR << ' ' << AC << std::endl;
}
void ChangeNumericBase() {
	std::cout << "New Base" << std::endl;
	do {
		std::cout << IOPrompt;
		std::cin >> NumericBase;
		if (NumericBase <= 1)
			std::cout << "Inproper Base" << std::endl;
	} while (NumericBase <= 1);
}
////////////////////////////////////////////////////



/////////////////////////////////////////////MARIE Functions////////////////////////////////////////////////
void Fetch() {
	MAR = PC;//MAR<-PC
	if (DebugMode)Registers();
	IR = Memory[MAR];//IR<-Memory[MAR]
	if (DebugMode)Registers();
	PC = hexAddition(PC, One);//PC<-PC+1
	if (DebugMode)Registers();
}
void Decode() {
	MAR = IR.substr(1);
	if (DebugMode)Registers();
}
void Get_Operand() {
	MBR = Memory[MAR];//MBR<-M[MAR]
	if (DebugMode)Registers();
}
void Execute() {
	MARIEinstruction[IR[0]]();
	if (DebugMode)Registers();
}
void mainMARIE() {
		std::cout << "=====================================================" << std::endl;
		std::cout << "*All Inputs and Outputs in Base " << NumericBase << "*" << std::endl;
		std::cout << "*Largest Number supported: " << ConvertNumericBase("ffff", 16, NumericBase) << std::endl;
		if (ProgramFilled) {
			//First Pass
			SearchAndAssignLabels();
			AssignMapFail = false;
			AssignMemoryMap();
			if (AssignMapFail) {
				std::cout << "=====================================================" << std::endl;
				return;
			}

			if (DebugMode) {
				std::cout << "Program Lines:" << std::endl;
				ShoweMarie();//Here to view MARIE code
				std::cout << "ENTER to continue with program" << std::endl;
				system("PAUSE>NUL");
				std::cout << "PC   IR   MAR  MBR  AC" << std::endl;
				Registers();
			}
			//Second Pass
			while (!Halted) {
				Fetch();
				Decode();
				Get_Operand();
				Execute();
			}
		}
		else
			std::cout << "No Program Loaded" << std::endl;
		std::cout << "=====================================================" << std::endl;	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////PostMARIE functions/////////////////
void ClearContainers() {
	Memory.clear();
	Label.clear();
	ProgramLines.clear();
}
////////////////////////////////////////////////////////


//////////////////////////////////////////////Fancy Stuff//////////////////////////////////////////////////

/////////////////////////////
std::vector<std::string> History;
void LoadHistory() {
	History.clear();
	std::string line;
	std::ifstream myfile("history.txt");
	if (myfile.is_open()) {
		unsigned int i = 1;
		while (getline(myfile, line)) {
			History.push_back(line);
			std::cout << i << ": " << line << std::endl;
			i++;
		}
		myfile.close();
	}
	else {
		std::cout << "history.txt missing!" << std::endl;
	}
}
void AddToHistory() {
	bool FileInHistory = false;
	unsigned int FilePosition = 0;
	
	//check if file being used is in history
	for (unsigned int i = 0; i < History.size(); i++) {
		if (InputFile == History[i]) {
			FilePosition = i;
			FileInHistory = true;
			break;
		}
	}
	//update history to reflect most recently used file
	History.insert(History.begin(), InputFile);
	//remove previous position of file if duplicated after updating
	if (FileInHistory) {
		History.erase(History.begin() + FilePosition + 1);
	}

	//Pass vector to history.txt
	std::ofstream ToHistoryFile("history.txt", std::ofstream::trunc);
	for (unsigned int i = 0; i < History.size(); i++) {
		ToHistoryFile << History[i] << std::endl;
	}
}
/////////////////////////////


unsigned int MenuChoice = 0;
void ShowMenuOptions() {
	std::cout << "1. Input File" << std::endl;
	std::cout << "2. History" << std::endl;
	std::cout << "3. Settings" << std::endl;
	std::cout << "4. Quit" << std::endl;
}
void Menu() {
	ShowMenuOptions();
	std::cout << IOPrompt;
	std::cin >> MenuChoice;
}
void ChooseFile() {
	PickFile();
	FillProgramLines();
	mainMARIE();
}
void ChooseHistory() {
	LoadHistory();

	unsigned int HistoryChoice = 0;
	do {
		if (History.empty()) {
			std::cout << "History Empty" << std::endl;
			return;
		}
		else {
			std::cout << IOPrompt;
			std::cin >> HistoryChoice;
			if (HistoryChoice<1 || HistoryChoice>History.size()) {
				std::cout << "Unable to choose file" << std::endl;
			}
		}
	} while ( HistoryChoice < 1 || HistoryChoice > History.size() );
	InputFile = History[HistoryChoice - 1];

	FillProgramLines();
	mainMARIE();
}
void ChooseSetting() {//Needs more stuff
	//not writing a function just for this menu
	unsigned int SettingsChoice = 0;

	std::cout << "1. Debug Mode\t\t" << (DebugMode ? "ON" : "OFF") << std::endl;
	std::cout << "2. Change Numeric Base\t" << NumericBase << std::endl;
	
	while (SettingsChoice == 0) {
		std::cout << IOPrompt;
		std::cin >> SettingsChoice;
		switch (SettingsChoice) {
		case 1: AskDebugMode(); break;
		case 2: ChangeNumericBase(); break;
		default: SettingsChoice = 0; std::cout << "No Item Chosen" << std::endl;
		}
	}
}
void Terminate() {
	char WantsToGo = 'N';
	std::cout << "Are you sure? (Y/N)" << std::endl;
	std::cout << IOPrompt;
	std::cin >> WantsToGo;
	if (WantsToGo == 'Y' || WantsToGo == 'y') {
		Terminated = true;
	}
}
void MenuFunction(unsigned int& x) {
	switch (x) {
	case 1: 
		std::cout << "Input File" << std::endl << "==========" << std::endl; 
		ChooseFile(); 
		return;
	case 2: 
		std::cout << "History" << std::endl << "========" << std::endl; 
		ChooseHistory(); 
		return;
	case 3: 
		std::cout << "Settings" << std::endl << "========" << std::endl; 
		ChooseSetting();
		return;
	case 4:
		std::cout << "Quit" << std::endl << "====" << std::endl;
		Terminate();
		return;
	default:std::cout << "no" << std::endl; return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
	do {
		system("CLS");
		Menu();
		system("CLS");
		MenuFunction(MenuChoice);
		if (!InputFile.empty())
			AddToHistory();
		//Prepare for new MARIE
		ClearContainers();
		Halted = false;
		///////////////////////
		if (!Terminated) {
			std::cout << "ENTER to return to menu" << std::endl;
			system("PAUSE>NULL");
		}
	} while (!Terminated);

	std::cout << "Program Terminated" << std::endl;
	Sleep(1500);
	return 0;
}