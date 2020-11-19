#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<set>
#include<map>
using namespace std;
//#define CMD
class sop {
public:
	sop() {}
	sop(string i,bool dc):bits(i),dont_care(dc){}
	bool dont_care = false;
	bool merged = false;
	string bits;
	set<sop*> minterms;
	int one_count() {
		int count = 0;
		for (char& i : bits){
			if (i == '1')count++;
		}
		return count;
	}
	sop& operator=(const sop& in) {
		dont_care = in.dont_care;
		merged = in.merged;
		bits = in.bits;
		minterms = in.minterms;
		return *this;
	}
	bool operator==(const sop& x)const { return bits == x.bits; }
	char& operator[](const int index) { return bits[index]; }
};

class MINI {
public:
	~MINI() {
		for (auto i : all) {
			delete i;
		}
	}
	int variable_num = 0;
	int output_num = 0;
	string variables;
	string output;
	vector<sop*> all;
	map<string, vector<sop*> > terms;
	vector<sop> result;
	//vector<vector<sop*> > group;
	void new_sop(string in, bool dont_care) {
		int i;
		for (i = 0;i < in.size();i++) {
			if (in[i] == '-') {
				in[i] = '0';
				new_sop(in, dont_care);
				in[i] = '1';
				new_sop(in, dont_care);
				return;
			}
		}
		if (terms.find(in) != terms.end())return;
		sop* n = new sop(in, dont_care);
		n->minterms.insert(n);
		all.push_back(n);
		terms[in].push_back(n);
	}
	bool merge(sop& a, sop& b, sop& result) {
		int different_count = 0;
		result.bits.resize(variable_num);
		for (int i = 0;i < variable_num;i++) {
			if ((a[i] == '-' && b[i] != '-') || (a[i] != '-' && b[i] == '-')) return false;
			if (a[i] != b[i]) {
				different_count++;
				result[i] = '-';
			}
			else {
				result[i] = a[i];
			}
		}
		if (different_count == 1) {
			result.minterms.insert(a.minterms.begin(), a.minterms.end());
			result.minterms.insert(b.minterms.begin(),b.minterms.end());
			return true;
		}
		return false;
	}
	void Quine_McCluskey() {
		map<string, sop>* literal = new map<string, sop>[variable_num];
		map<string, sop> essential_pi;
		map<sop*, int> count;
		for (int i = 0;i < all.size();i++) {
			literal[0][all[i]->bits] = *all[i];
			if (!all[i]->dont_care) {
				count[all[i]] = 0;
			}
		}
		for (int i = 1;i < variable_num;i++) {
			for (auto j = literal[i - 1].begin();j != literal[i - 1].end();j++) {
				for (auto k = next(j);k != literal[i - 1].end();k++) {
					sop merge_result;
					//if (merge(j->second, k->second, merge_result)) {
					//	j->second.merged = true;
					//	k->second.merged = true;
					//	literal[i][merge_result.bits] = merge_result;
					//}
				}

				if (!j->second.merged || i == variable_num - 1) {
					essential_pi[j->second.bits] = j->second;
					for (auto g : j->second.minterms) {
						count[g]++;
					}
				}
			}
		}
		for (auto& i : count) {
			if (i.second != 1) continue;
			int g = 0;
			for (auto it = essential_pi.begin();it != essential_pi.end();it++) {
				if (it->second.minterms.find(i.first) != it->second.minterms.end()) {
					result.push_back(it->second);
					for (auto& k : it->second.minterms) {
						count[k] = -1;
					}
					essential_pi.erase(it);
					it--;
				}
			}
			i.second = -1;
		}
		vector<sop*> remain_minterm;
		vector<sop> remain_pi;
		for (auto& i : essential_pi) {
			remain_pi.push_back(i.second);
		}
		for (auto it = count.begin();it != count.end();it++) {
			if (it->second != -1) {
				remain_minterm.push_back(it->first);
			}
		}
		Petrick_Method(remain_minterm, remain_pi);
		delete[] literal;
	}
	void Petrick_Method(vector<sop*> remain_minterm, vector<sop> remain_pi) {
		for (auto i : remain_minterm) {
			cout << i->bits << endl;
		}
		for (auto i : remain_pi) {
			cout << i.bits << endl;
		}
		//for (int i = 1;i < variable_num;i++) {
		//	vector<int> index(i);
		//	int count = 0;
		//	set<sop*> this_turn;
		//	for (int j = 0;j < i;j++,count++) {
		//		index[j] = count;
		//	}
		//}
	}
};

int main(int argc, char* argv[]) {
#ifdef CMD
	if (argc != 3) {
		cout << "command error!" << endl;
		return 0;
	}
	ifstream input(argv[1]);//argv[0] 是本程式名稱
	ofstream dotfile(argv[2]);
#else
	ifstream input("input.pla");
	ofstream dotfile("output.pla");
#endif // CMD
	string line,word;
	MINI mini;
	while (input >> word) {
		if (word == ".i") {
			input >> mini.variable_num;
		}
		else if (word == ".o") {
			input >> mini.output_num;
		}
		else if (word == ".ilb") {
			string tmp;
			for (int i = 0;i < mini.variable_num;i++) {
				input >> tmp;
				mini.variables += tmp;
			}
		}
		else if (word == ".ob") {
			string tmp;
			for (int i = 0;i < mini.output_num;i++) {
				input >> tmp;
				mini.output += tmp;
			}
		}
		else if (word == ".p") {
			int line_count;
			string sop;
			input >> line_count;
			input.ignore();
			for (int i = 0;i < line_count;i++) {
				getline(input, line);
				bool dont_care = false;
				switch (line.back())
				{
				case '0': {
					for (char& i : line) {
						if (i == '0') i = '1';
						else if (i == '1') i = '0';
					}
				}break;
				case '1':break;
				case '-':dont_care = true;break;
				default:
					break;
				}
				mini.new_sop(line.substr(0, mini.variable_num), dont_care);
			}
			mini.Quine_McCluskey();
		}
		else if (word == ".e") {
			break;
		}
		else {
			cout << "error reading pla file!" << endl;
			return 0;
		}
	}
	return 0;
}