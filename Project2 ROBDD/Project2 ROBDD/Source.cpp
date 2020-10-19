#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<sstream>
#include<bitset>
using namespace std;
class Node {
public:
	int else_edge,then_edge;//index of arrayreduced_order()
	string value="";
	bool redundent = false;
};
class ROBDD {
public:
	ROBDD() {}
	~ROBDD() {
		delete[] btree;
	}
	void create_tree() {
		size = (1 << i_count) + 1;
		btree = new Node[size];
		btree[0].value = "0";
		btree[size-1].value = "1";
	}
	void create_layer(string input,int layer_count) {
		for (int i = (1 << layer_count);i < (1 << (layer_count + 1));i++) {
			btree[i].value = input;
			if (layer_count == i_count - 1) continue;
			btree[i].else_edge = i * 2;
			btree[i].then_edge = i * 2 + 1;
		}
	}
	void output(ofstream& dot) {
		for (int i = (1 << (i_count - 1)),j = 0;i < size - 1;i++,j+=2) {
			btree[i].else_edge = get_result(j) ? size - 1 : 0;
			btree[i].then_edge = get_result(j+1) ? size - 1 : 0;
		}
		reduced_order();//recursive
		//output to file
		dot << "dfgdfh";
	}
	bool get_result(int num) {//get boolean function result
		bitset<8> bin(num);
		bool result = false;
		for (int i = 0;i < product_terms.size();i++) {
			bool line_result = true;
			for (int j = 0;j < i_count;j++) {
				if (product_terms[i][j] == '-') continue;
				if (!bin[i_count - j - 1]) {// found 0
					line_result = product_terms[i][j] == '0';
					break;
				}
			}
			if (product_terms[i].back() == '0') line_result = !line_result;

			if (line_result) {
				result = true;
				break;
			}
		}
		return result;
	}
	void reduced_order() {//main algorithm here
		print_tree();
	}
	void print_tree() {
		cout << "index" << '\t' << "Variable" << "Else-edge"  << "Then-edge" << '\t' << "Comment" << endl;
		for (int i = 1;i < size-1;i++) {
			cout << i << '\t' << btree[i].value << '\t' << btree[i].else_edge << '\t' << btree[i].then_edge << '\t' << (btree[i].redundent ? "redundent" : "") << endl;
		}
		cout << "-----------------------------------------------------------------------" << endl;
	}
	int i_count = 0, o_count = 0 , size = 0;
	Node* btree = nullptr;
	vector<string> product_terms;
};
int main() {
	ifstream input("input.pla");
	ofstream dotfile("output.dot");
	string line,word;
	ROBDD robdd;
	while (1) {
		//sline.str(line);
		input >> word;
		if (word == ".i") {
			input >> robdd.i_count;
			robdd.create_tree();
		}
		else if(word == ".o"){
			input >> robdd.o_count;
		}
		else if (word == ".ilb") {
			string tmp;
			for (int i = 0;i < robdd.i_count;i++) {
				input >> tmp;
				robdd.create_layer(tmp,i);
			}
		}
		else if (word == ".ob") {
			string tmp;
			input >> tmp;
		}
		else if (word == ".p") {
			int line_count;
			char nl;
			input >> line_count;//Åª±¼´«¦æ
			input.ignore();
			for (int i = 0;i < line_count;i++) {
				getline(input, line);
				robdd.product_terms.push_back(line);
			}
		}
		else if (word == ".e") {
			break;
		}
		else {
			cout << "error reading pla file!" << endl;
			return 0;
		}
	}
	robdd.output(dotfile);
	return 0;
}