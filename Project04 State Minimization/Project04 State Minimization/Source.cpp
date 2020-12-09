#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#define CMD
using namespace std;

struct TERM {
	string input;
	string current_state;
	string next_state;
	string output;
	bool operator==(const TERM& i) const{
		//return (input == i.input) && (output == i.output) && (next_state == i.next_state);//wrong compare!!!
		return (input == i.input) && (output == i.output) && ((next_state == i.next_state) || (next_state == i.current_state && i.next_state == current_state));
	}
	bool operator!=(const TERM& i) const{
		return !(*this == i);
	}
};
typedef vector<TERM> STATE;
bool operator==(const STATE& A, const STATE& B) {
	for (int i = 0; i < A.size(); i++) {
		bool flag = false;
		for (int j = 0; j < B.size(); j++) {
			if (A[i] == B[j]) {
				flag = true;
				break;
			}
		}
		if (!flag) return false;
	}
	return true;
}
class KISS {
public:
	int input_num,output_num,term_num,state_num;
	string start_state;
	map<string,STATE> states;
	void Minimization() {
		for (auto i = states.begin();i != states.end();++i) {
			for (auto j = next(i);j != states.end(); ++j) {
				if (i->second == j->second){
					if (start_state == j->first) start_state = i->first;
					for (auto& k : states){
						for (auto& m : k.second){
							if (m.next_state == j->first) m.next_state = i->first;
						}
					}
					states.erase(j);
					Minimization();
					return;
				}
			}
		}
	}
	void output_kiss(ofstream& output) {
		output << ".start_kiss\n";
		output << ".i " << input_num << '\n';
		output << ".o " << output_num << '\n';
		output << ".p " << states.size() * (1 << input_num) << '\n';
		output << ".s " << states.size() << '\n';
		output << ".r " << start_state << '\n';
		for (auto& i : states) {
			for (auto& j : i.second) {
				output << j.input << ' ' << i.first << ' ' << j.next_state << ' ' << j.output << '\n';
			}
		}
		output << ".end_kiss";
	}
	void output_dot(ofstream& dot) {
		dot << "digraph STG {\n\trankdir=LR;\n\n\tINIT [shape=point];\n";
		for (auto& i : states) {
			dot << '\t' << i.first << " [label=\"" << i.first << "\"];\n";
		}
		dot << "\n\n\tINIT -> " << start_state << ";\n";
		for (auto& i : states) {
			for (auto j = i.second.begin();j != i.second.end();j++) {
				dot << '\t' << i.first << " -> " << j->next_state << " [label=\"" << j->input << '/' << j->output;
				for (auto k = next(j);k != i.second.end();k++) {
					if (j->next_state == k->next_state) {
						dot << "," << k->input << '/' << k->output;
						i.second.erase(k--);
					}
				}
				dot << "\"];\n";
			}
		}
		dot << '}';
	}
};
int main(int argc, char* argv[]) {
#ifdef CMD
	if (argc != 4) {
		cout << "command error!" << endl;
		return 0;
	}
	ifstream input(argv[1]);//argv[0] 是本程式名稱
	ofstream output(argv[2]);
	ofstream dot(argv[3]);

#else
	ifstream input("input.kiss");
	ofstream output("output.kiss");
	ofstream dot("output.dot");
#endif // CMD
	KISS kiss;
	string word;
	bool start = false;
	while (input >> word) {
		if (word == ".start_kiss") start = true;
		else if (word == ".end_kiss") {
			kiss.Minimization();
			break;
		}
		if (start) {
			int tmpi;
			if (word == ".i") {
				input >> kiss.input_num;
			}
			else if (word == ".o") {
				input >> kiss.output_num;
			}
			else if (word == ".p") {
				input >> kiss.term_num;
			}
			else if (word == ".s") {
				input >> kiss.state_num;
			}
			else if (word == ".r") {
				input >> kiss.start_state;
				TERM tmp;
				for (int i = 0;i < kiss.term_num;i++) {
					input >> tmp.input >> tmp.current_state >> tmp.next_state >> tmp.output;
					kiss.states[tmp.current_state].push_back(tmp);
				}
				//KISS before_Minimization = kiss;
				//before_Minimization.output_dot(dot);
			}
		}
	}
	kiss.output_kiss(output);
	kiss.output_dot(dot);
	input.close();
	output.close();
	dot.close();
	return 0;
}