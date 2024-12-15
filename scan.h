#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<set>
#include <algorithm>  
#include<map>
#include<unordered_map>


using namespace std;
#define EMPTY "@"  //�մ���
#define END "$"  //�ս��
#define ACCEPT "acc"


// �����ķ��ṹ
struct GRAM {
	string name;  // �ķ��������(���ս��)
	vector<string> tokens;  // �ķ������Ҳ������tokens

	// �ж������ķ��Ƿ�����ͬ
	bool operator==(const GRAM& other) const {
		return name == other.name && tokens == other.tokens;
	}
	// �ж������ķ��Ƿ��ǲ�ͬ
	bool operator!=(const GRAM& other) const {
		return name != other.name || tokens != other.tokens;
	}
};


// LR(0)��Ŀ�ṹ  
struct LR0GRAM {
	GRAM gram;               // �ķ�����  
	vector<string> former;   // �ѽ��ܵ�tokens  
	bool operator==(const LR0GRAM& other) const {
		return gram == other.gram && former == other.former;
	}
};

// LR(0)��Ŀ���ṹ  
struct LR0ITEM {
	int id;                      // ��ĿID  
	vector<LR0GRAM> LR0Grams;   // ��Ŀ�е��ķ�  
	int size() {
		return LR0Grams.size();
	}
};

// LR(1)��Ŀ���е��ķ��ṹ
struct LR1GRAM {
	GRAM gram;
	string term;  // �ս��
	vector<string> former;  // �Ѿ����ܹ���token
	bool operator==(const LR1GRAM& other) const {
		return term == other.term && former == other.former && gram == other.gram;
	}
};

// LR(1)��Ŀ���ṹ
struct LR1ITEM {
	int id;
	vector<LR1GRAM> LRGrams;
	int size() {
		return LRGrams.size();
	}
};

// DFA�ı���Ϣ
struct LREDGE {
	int x, y;
	string s;
};



class scan {
public:
	vector<string> lines;  // ԭʼ�����ķ����ݣ����л�ȡ
    unordered_map<string, set<string>> First, Follow;
	int gramType;
	vector<GRAM> Grams;
	char suffix;  // �����ķ��ĺ�׺��
	typedef enum { START, DONE, NONTERM, TERMINAL, KEEP, OR, NOTEND, ENDLINE, ENDLINES } State;
	// START ��ʼ�����ַ�
	// DONE ���һ��Token�Ľ���
	// NONTERM ��ǰҪ���յ�Token�Ƿ��ս��
	// TERMINAL ��ǰҪ���յ�Token���ս��
	// KEEP ���������ַ��������ո�ֹͣ(�ڵڶ�����д��ʽ��ʹ��)
	// OR '|'�������ַ�
	// NOTEND ��û����ɨ���״̬
	// ENDLINE ɨ���굱ǰ�е�״̬
	// ENDLINES ɨ����ȫ����lines��״̬

	State scanState , endState;
	int lineID , charID ;
	GRAM newGram;
	void initialize();
	
	int getGramID(string name) {
		for (int i = 0; i < Grams.size(); ++i)
			if (Grams[i].name == name)
				return i;
		return -1;
	}
	// ͨ���ķ����󲿺��Ҳ���ͬ����
	int getGramID(const GRAM& gram) {
		for (int i = 0; i < Grams.size(); ++i)
			if (Grams[i] == gram)
				return i;
		return -1;
	}
	// �ж����ս�����Ƿ��ս��
	bool isNonterm(string name) {
		return getGramID(name) != -1;
	}

	bool isTerminal(string name) {
		return getGramID(name) == -1;
	}
	// �жϿմ�
	bool isEmpty(string name) {
		return name == EMPTY;
	}

	// �ж�Ԫ��x�Ƿ��ڼ���A��
	bool isInSet(string x, const set<string>& A) {
		return A.find(x) != A.end();
	}
	bool isInVec(string x, const vector<string>& A) {
		for (auto& y : A)
			if (x == y) return true;
		return false;
	}
	
	
	// �ϲ��������ϣ��Ҽ��ϵ�Ԫ��(����except��)����ӵ��󼯺���
	void mergeSet(set<string>& A, const set<string>& B, string except = "");
	// �ж��ַ��Ƿ�Ϊ' ', '\n', '\t'
	bool isBlank(char c);

	// ������Ż����ķ�
	void splitGrams();

	// ���俪ʼ�ķ�����A->a������A'->A  (��������ݹ飬�������ķ�)
	void extendStartGram();
	
	int nextChar();
	void backChar();
	void getLeftName();
	void getRightTokenType1();
	void getRightTokenType2();
	void getRightToken();
	// �����һ�����ս���ĵ�һ���ַ��Ǵ�д��ĸ����Type=1�������Сд��ĸ����Type=2
	void getGramType();

	void scanGrams();
	// ��ȡȫ��First���ϵĴ�С�ܺ�
	int FirstAllSize();
	// ��������ȫ��First����
	void buildFirstAll();

	// ��ȡȫ��Follow���ϵĴ�С�ܺ�
	int FollowAllSize();
	// ��������ȫ��Follow����
	void buildFollowAll();

	vector<LR0ITEM> LR0Items;  // �洢LR(0)��Ŀ��  
	vector<LREDGE> LR0Edges;  // �洢��  

	void closure0(int id);               // ��CLOSURE(I)�ĺ���  
	void go0(int id, const string& token); // ת�ƺ���  
	void buildLR0Items();               // ����LR(0)��Ŀ���ĺ���  
	
	bool SLR1Fun1();
	bool SLR1Fun2();
	string getNextInputToken(const LR0GRAM& gram);
	bool isReducePossible(const LR0GRAM& gram);
	bool isShiftPossible(const LR0ITEM& item, const string& token);
	string getReductionRule(const LR0GRAM& gram);
	int SLR1Analyse();
	vector<LR1ITEM> LR1Items;
	

	vector<LREDGE> LR1Edges;
	

	// ��go������ʹ��
	vector<string> nextTokens;

	map<pair<int, string>, string> LR1ACTION, LR1GOTO;
	map<pair<vector<int>, string>, string> ACTION, GOTO;
	bool isLR1InItem(int id, const LR1GRAM& A);

	int getItemID(const LR1ITEM& A);

	// ��CLOSURE(I)
	void closure(int id);
	// LR1Items[id]����token�󣬵õ��µ�newItem=LR1Items[newID]
	void go(int id, string token);
	void getNextTokens(int id);


	// ����LR(1)����Ŀ��
	void buildLR1Item();


	// ����LR(1)������
	void buildLR1Table() ;


};