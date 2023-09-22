#include "Display.hpp"
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

int main(int argc, char const *argv[]) {
	if (argc < 2) {
		DisplayERROR("没有源文件！\n");
	} else {
		Command::FirstWork fw(argv[1]);
		fw();
		Command::LastWork lw = Command::LastWork();
		lw();
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
			//cout << LineData << endl;

			try {
				if (LineData.starts_with(".orgs")) {
					//cout << GetArgs(LineData,5,1).at(0);
					StackBaseAddress = stoull(GetArgs(LineData,5,1).at(0));
				} else if (LineData.starts_with(".orgc")) {
					CodeBaseAddress  = stoull(GetArgs(LineData,5,1).at(0));
				} else if (LineData.ends_with(":")) {
					LineData.erase(LineData.size()-1,1);
					LinkTableClass lt(LineData,to_string(CodeNumber*CodeLong));
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
	/*cout << "	Code:" << endl;
	for (int i = 0;i < FileData.size();++i) {
		cout << FileData[i];
	}

	cout << "	Table:" << endl;*/
	for (int i = 0;i < LinkTable.size();++i) {
		string LN = LinkTable[i].LinkName;
		string RN = LinkTable[i].Replace;
		//cout << LN << " " << RN << endl;
		ReplaceString(FileData,LN,RN);
	}
	ofstream writes(EndFile);


	//cout << "	NewCode:" << endl;
	for (int i = 0;i < FileData.size();++i) {
		writes << FileData[i];
		//cout << FileData[i];
	}
}