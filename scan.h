#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<set>
#include <algorithm>  
#include<map>
#include<unordered_map>


using namespace std;
#define EMPTY "@"  //空串符
#define END "$"  //终结符
#define ACCEPT "acc"


// 单条文法结构
struct GRAM {
	string name;  // 文法规则的左部(非终结符)
	vector<string> tokens;  // 文法规则右部的组成tokens

	// 判定两个文法是否是相同
	bool operator==(const GRAM& other) const {
		return name == other.name && tokens == other.tokens;
	}
	// 判定两个文法是否是不同
	bool operator!=(const GRAM& other) const {
		return name != other.name || tokens != other.tokens;
	}
};


// LR(0)项目结构  
struct LR0GRAM {
	GRAM gram;               // 文法规则  
	vector<string> former;   // 已接受的tokens  
	bool operator==(const LR0GRAM& other) const {
		return gram == other.gram && former == other.former;
	}
};

// LR(0)项目集结构  
struct LR0ITEM {
	int id;                      // 项目ID  
	vector<LR0GRAM> LR0Grams;   // 项目中的文法  
	int size() {
		return LR0Grams.size();
	}
};

// LR(1)项目集中的文法结构
struct LR1GRAM {
	GRAM gram;
	string term;  // 终结符
	vector<string> former;  // 已经接受过的token
	bool operator==(const LR1GRAM& other) const {
		return term == other.term && former == other.former && gram == other.gram;
	}
};

// LR(1)项目集结构
struct LR1ITEM {
	int id;
	vector<LR1GRAM> LRGrams;
	int size() {
		return LRGrams.size();
	}
};

// DFA的边信息
struct LREDGE {
	int x, y;
	string s;
};



class scan {
public:
	vector<string> lines;  // 原始输入文法内容，按行获取
    unordered_map<string, set<string>> First, Follow;
	int gramType;
	vector<GRAM> Grams;
	char suffix;  // 新增文法的后缀名
	typedef enum { START, DONE, NONTERM, TERMINAL, KEEP, OR, NOTEND, ENDLINE, ENDLINES } State;
	// START 开始接收字符
	// DONE 完成一个Token的接收
	// NONTERM 当前要接收的Token是非终结符
	// TERMINAL 当前要接收的Token是终结符
	// KEEP 持续接受字符，遇到空格停止(在第二种书写形式中使用)
	// OR '|'或，特殊字符
	// NOTEND 还没结束扫描的状态
	// ENDLINE 扫描完当前行的状态
	// ENDLINES 扫描完全部行lines的状态

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
	// 通过文法的左部和右部共同查找
	int getGramID(const GRAM& gram) {
		for (int i = 0; i < Grams.size(); ++i)
			if (Grams[i] == gram)
				return i;
		return -1;
	}
	// 判断是终结符还是非终结符
	bool isNonterm(string name) {
		return getGramID(name) != -1;
	}

	bool isTerminal(string name) {
		return getGramID(name) == -1;
	}
	// 判断空串
	bool isEmpty(string name) {
		return name == EMPTY;
	}

	// 判断元素x是否在集合A中
	bool isInSet(string x, const set<string>& A) {
		return A.find(x) != A.end();
	}
	bool isInVec(string x, const vector<string>& A) {
		for (auto& y : A)
			if (x == y) return true;
		return false;
	}
	
	
	// 合并两个集合，右集合的元素(除了except外)都添加到左集合中
	void mergeSet(set<string>& A, const set<string>& B, string except = "");
	// 判断字符是否为' ', '\n', '\t'
	bool isBlank(char c);

	// 按或符号划分文法
	void splitGrams();

	// 扩充开始文法，如A->a，新增A'->A  (先消除左递归，再扩充文法)
	void extendStartGram();
	
	int nextChar();
	void backChar();
	void getLeftName();
	void getRightTokenType1();
	void getRightTokenType2();
	void getRightToken();
	// 如果第一个非终结符的第一个字符是大写字母，则Type=1，如果是小写字母，则Type=2
	void getGramType();

	void scanGrams();
	// 获取全部First集合的大小总和
	int FirstAllSize();
	// 迭代构建全部First集合
	void buildFirstAll();

	// 获取全部Follow集合的大小总和
	int FollowAllSize();
	// 迭代构建全部Follow集合
	void buildFollowAll();

	vector<LR0ITEM> LR0Items;  // 存储LR(0)项目集  
	vector<LREDGE> LR0Edges;  // 存储边  

	void closure0(int id);               // 求CLOSURE(I)的函数  
	void go0(int id, const string& token); // 转移函数  
	void buildLR0Items();               // 构建LR(0)项目集的函数  
	
	bool SLR1Fun1();
	bool SLR1Fun2();
	string getNextInputToken(const LR0GRAM& gram);
	bool isReducePossible(const LR0GRAM& gram);
	bool isShiftPossible(const LR0ITEM& item, const string& token);
	string getReductionRule(const LR0GRAM& gram);
	int SLR1Analyse();
	vector<LR1ITEM> LR1Items;
	

	vector<LREDGE> LR1Edges;
	

	// 在go函数中使用
	vector<string> nextTokens;

	map<pair<int, string>, string> LR1ACTION, LR1GOTO;
	map<pair<vector<int>, string>, string> ACTION, GOTO;
	bool isLR1InItem(int id, const LR1GRAM& A);

	int getItemID(const LR1ITEM& A);

	// 求CLOSURE(I)
	void closure(int id);
	// LR1Items[id]接收token后，得到新的newItem=LR1Items[newID]
	void go(int id, string token);
	void getNextTokens(int id);


	// 构建LR(1)的项目集
	void buildLR1Item();


	// 构建LR(1)分析表
	void buildLR1Table() ;


};