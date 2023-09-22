#include "BYQ.hpp"

using namespace std;

vector<string> GetArgs(string con,int conlen,int args) {
	con.erase(0,conlen+1);
	vector<string> ret;

	for (int i = 0;i < args;++i) {
		string thisstr = "";
		while (con.size() != 0) {
			if (con[0] == ' ') {
				ret.push_back(thisstr);
				thisstr = "";
			} else {
				thisstr += con[0];
			}
			con.erase(0,1);
		}
		if (con.size() == 0) {
			ret.push_back(thisstr);
		}
	}
	return ret;
}

void DisplayERROR(string s) {
	DisplayAtColor("error ",DisplayColor::red);
	cout << s;
}

void DisplayWarn(string s) {
	DisplayAtColor("warning ",DisplayColor::blue);
	cout << s;
}

void ReplaceString(std::string& subject, const std::string& search,const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

void Start(string file) {
	Command::FirstWork fw = Command::FirstWork(file);
	fw();
	Command::LastWork lw = Command::LastWork();
	lw();
}

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		DisplayERROR("没有源文件！\n");
	} else {
		vector<string> args;
		for (int i = 1;i < argc;++i) {
			string str = argv[i];
			args.push_back(str);
		}

		while (args.size() != 0) {
			for (int i = 0;i < args.size();++i) {
				if (args[i] == "-o") {
					if ((i+1) < args.size()) {
						EndFile = args[i+1];
						args.erase(args.begin()+i,args.begin()+i+2);
						break;
					} else {
						DisplayERROR("命令参数不正确。");
					}
				} else {
					ofstream writes("__temp__.source.temps",ios_base::app | ios_base::out);
					ifstream reads (args[i]);
					while (reads.good()) {
						char t = reads.get();
						if (t != -1) {
							writes << t;
						}
					}
					writes << "\n";
					args.erase(args.begin()+i);
					break;
				}
			}
		}
		Start("__temp__.source.temps");
#ifdef DEBUG
#else
#ifdef _WIN32
		system("del __temp__.source.temps");
#elif __linux__
		system("rm __temp__.source.temps");
#endif
#endif
	}
	return 0;
}

void Command::FirstWork::operator()() {
	const vector<char> NullCharVec = {'	',' ','\0','\r','\n'};
	const vector<char> LineEndVec  = {'\r','\n','\0'};
	string LineData = "";

	ifstream reads(FileName);
	while (!reads.eof()) {
		char gets = reads.get();
		//cout << LineData << endl;
		if (LineData.empty()) {
			bool NoIns = false;
			for (int i = 0;i < NullCharVec.size();++i) {
				NoIns = (gets == NullCharVec[i]);
				if (NoIns) break;
			}
			if (NoIns) continue;
		}
		bool WorkStr = false;
		for (int i = 0;i < LineEndVec.size();++i) {
			WorkStr = (gets == LineEndVec[i]);
			if (WorkStr) break;
		}
		if (WorkStr || reads.eof()) {
			try {
				if (LineData.starts_with(".orgs")) {
					StackBaseAddress = stoull(GetArgs(LineData,5,1).at(0));
				} else if (LineData.starts_with(".orgc")) {
					CodeBaseAddress  = stoull(GetArgs(LineData,5,1).at(0));
				} else if (LineData.ends_with(":")) {
					cout << (CodeNumber*CodeLong) << endl;
					LineData.erase(LineData.size()-1,1);
					LinkTableClass lt(LineData,to_string(CodeBaseAddress+CodeNumber*CodeLong));
					LinkTable.push_back(lt);
				} else {
					CodeNumber++;
					FileData += (LineData + "\n");
				}
			} catch (...) {
				DisplayERROR("'" + LineData + "'的语法不正确！\n");
				DontWork = true;
			}

			LineData = "";
		} else {
			LineData += gets;
		}
	}
}

void Command::LastWork::operator()() {
	if (DontWork) return;

	for (int i = 0;i < LinkTable.size();++i) {
		ReplaceString(FileData,LinkTable[i].LinkName,LinkTable[i].Replace);
	}
	ofstream writes(EndFile);

	int WriteChars = 0;
	for (int i = 0;i < FileData.size();++i) {
		if (FileData[i] == '\n' || FileData[i] == '\r') {
			if ((CodeWith - WriteChars) < 0) {
				DisplayERROR("指令过长！\n");
			}
			for (int i = 0;i < (CodeWith - WriteChars);++i) {
				writes << ' ';
			}
			writes << FileData[i];
			WriteChars = 0;
		} else {
			writes << FileData[i];
			++WriteChars;
		}
	}
}

#ifdef _WIN32
	namespace {
		#include <windows.h>
	}

	void SetColorAndBackground(int ForgC,int BackC) {
		::WORD wColor = ((BackC & 0x0F) << 4) + (ForgC & 0x0F);
		::SetConsoleTextAttribute(::GetStdHandle((::DWORD)-11), wColor);
	}
#endif

void DisplayAtColor(string str,int color) {
	if (color < 0x0 || color > 0xf) {
		cout << str;
	} else {
		#if _WIN32
			SetColorAndBackground(color);
			cout << str;
			SetColorAndBackground();
		#elif __linux__
    		cout << "\033[3"+to_string(color)+";1m" + str + "\033[0m";
		#else
			cout << str;
		#endif
	}
}