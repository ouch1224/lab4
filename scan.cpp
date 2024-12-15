#include "pch.h"
#include "scan.h"


void scan::initialize() {

	// 清空原始输入文法内容  
	lines.clear();
	// 清空 First 和 Follow  
	First.clear();
	Follow.clear();
	// 重置 gramType  
	gramType = 0;  

	// 清空 Grams 向量  
	Grams.clear();
	scanState = START;
	endState = NOTEND;
	lineID = 0;
	charID=0;
	LR1Items.clear();
	LR1Edges.clear();
	LR0Edges.clear();
	LR0Items.clear();
	nextTokens.clear();
}

// 合并两个集合，右集合的元素(除了except外)都添加到左集合中
void scan::mergeSet(set<string>& A, const set<string>& B, string except) {
	for (auto& x : B)
		if (x != except) A.insert(x);
}
// 判断字符是否为' ', '\n', '\t'
bool scan::isBlank(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

// 按或符号划分文法
void scan::splitGrams() {
	
	vector<GRAM> grams = Grams;  // 临时变量
	Grams.clear();  // 清空原来的文法
	for (auto& gram : grams) {
		vector<string> tokens;  // 记录当前被拆分后获得的右部tokens
		for (auto& x : gram.tokens) {
			if (x != "|") {  // 没遇到"|"就继续添加右部
				tokens.push_back(x);
				continue;
			}
			Grams.push_back({ gram.name, tokens });  // 遇到"|"，新增文法
			tokens.clear();  // 清空当前右部记录，准备下一次的遍历
		}
		// (处理末尾数据)如果还有剩余的右部，直接加入到Grams中
		if (!tokens.empty()) Grams.push_back({ gram.name, tokens });
	}
}

// 扩充开始文法，如A->a，新增A'->A  (先消除左递归，再扩充文法)
void scan::extendStartGram() {
	
	suffix = (gramType == 1 ? '\'' : 'i');
	string newName = Grams[0].name + suffix;

	while (isNonterm(newName))  // 如果新名称已存在，就一直往后追加suf
		newName += suffix;
	Grams.insert(Grams.begin(), { newName, {Grams[0].name} });
}

int scan::nextChar() {
	if (charID >= lines[lineID].size()) {
		endState = (lineID >= lines.size() - 1 ? ENDLINES : ENDLINE);
		++charID;
		return EOF;
	}
	return lines[lineID][charID++];
}

void scan::backChar() {
	--charID;
	if (charID < 0) charID = 0;
}

void scan::getLeftName() {
	string name;  // 文法推导左式只能是字母和"'"组成
	for (int i = 0; i < lines[lineID].size(); ++i) {
		if (lines[lineID][i] == '>') {
			newGram.name = name;
			charID = i + 1;
			return;
		}
		if (isalpha(lines[lineID][i]) || lines[lineID][i] == '\'')
			name += lines[lineID][i];
	}
	newGram.name = name;
}

void scan::getRightTokenType1() {
	bool save;
	string token;
	scanState = START;
	endState = NOTEND;
	while (scanState != DONE) {
		char c = nextChar();
		save = true;  // 保存字符，c追加到token后
		switch (scanState) {
		case START:
			if (isupper(c))
				scanState = NONTERM;  // 非终结符
			else if (c == '|')
				scanState = OR;
			else if (isBlank(c))
				save = false;
			else
				scanState = TERMINAL;
			break;
		case NONTERM:
			if (c != '\'') {
				backChar();
				save = false;
				scanState = DONE;
			}
			break;
		case TERMINAL:
			backChar();
			save = false;
			scanState = DONE;
			break;
		case OR:
			backChar();
			save = false;
			scanState = DONE;
			break;
		case DONE:
		default: /* should never happen */
			cout << "Scanner Bug: state = " << scanState << '\n';
			scanState = DONE;
			break;
		}
		if (save) token += c;
	}
	newGram.tokens.push_back(token);
}

void scan::getRightTokenType2() {
	bool save;
	string token;
	scanState = START;
	endState = NOTEND;
	while (scanState != DONE) {
		char c = nextChar();
		save = true;  // 保存字符，c追加到token后
		switch (scanState) {
		case START:
			if (isBlank(c))
				save = false;
			else if (c == '|')
				scanState = OR;
			else
				scanState = KEEP;
			break;
		case KEEP:
			if (isBlank(c) || c == EOF) {
				save = false;
				scanState = DONE;
			}
			break;
		case OR:
			backChar();
			save = false;
			scanState = DONE;
			break;
		case DONE:
		default: /* should never happen */
			cout << "Scanner Bug: state = " << scanState << '\n';
			scanState = DONE;
			break;
		}
		if (save) token += c;
	}
	newGram.tokens.push_back(token);
}
void scan::getRightToken() {
	gramType == 1 ? getRightTokenType1() : getRightTokenType2();
}
// 如果第一个非终结符的第一个字符是大写字母，则Type=1，如果是小写字母，则Type=2
void scan::getGramType() {
	gramType = isupper(lines[0][0]) ? 1 : 2;
}

void scan::scanGrams() {
	getGramType();
	
	while (endState != ENDLINES) {
		endState = NOTEND;
		getLeftName();
		while (endState == NOTEND)
			getRightToken();
		Grams.push_back(newGram);
		//printGram(Grams.size() - 1);
		newGram.tokens.clear();
		++lineID, charID = 0;
	}
}



// 获取全部First集合的大小总和
int scan::FirstAllSize() {
	int size = 0;
	for (auto& x : First)
		size += x.second.size();
	return size;
}

// 迭代构建全部First集合
void scan::buildFirstAll() {
	int lastSize = -1, newSize = 0;
	while (lastSize != newSize) {
		for (auto& gram : Grams) {
			bool flag = false;  // 标志符，判断当前右部的token的First(token)是否存在@，如果存在，就得继续往后遍历右部
			for (auto& token : gram.tokens) {  // 遍历文法的右部
				if (isTerminal(token)) First[token] = { token };  // 终结符的First集合只有自身
				mergeSet(First[gram.name], First[token], EMPTY);  // 把First[token] - @合并到First[左部]
				flag = isInSet(EMPTY, First[token]);  // 如果@在First[token]，继续往后遍历
				if (!flag) break;  // 如果@不在First[token]，停止遍历
			}
			if (flag) First[gram.name].insert(EMPTY);  // 如果遍历完右部，flag还是true,说明@∈First[右部]
		}
		lastSize = newSize, newSize = FirstAllSize();  // 更新全部First集合的大小
	}
	// 补充还没遍历过的终结符的First集合
	for (auto& gram : Grams)
		for (auto& token : gram.tokens)
			if (isTerminal(token)) First[token] = { token };
}

// 获取全部Follow集合的大小总和
int scan::FollowAllSize() {
	int size = 0;
	for (auto& x : Follow)
		size += x.second.size();
	return size;
}

// 迭代构建全部Follow集合
void scan::buildFollowAll() {
	int lastSize = -1, newSize = 0;
	Follow[Grams[0].name] = { END };  // 初始化(默认第一条文法的左部为开始符号)
	newSize = FollowAllSize();  // 求当前全部Follow集合的大小总和
	while (lastSize != newSize) {  // 如果全部Follow集合的大小不再变化，就跳出计算
		for (auto& x : Grams) {
			for (int i = 0; i < x.tokens.size(); ++i) {  // 遍历右部的每一个token
				if (isTerminal(x.tokens[i])) continue;  // 只有非终结符才有Follow集合
				bool flag = (i == x.tokens.size() - 1);  // 判断@是否属于First(Xi+1Xi+2...Xn)
				for (int j = i + 1; j < x.tokens.size(); ++j) {  // 遍历token[i]后的tokens
					mergeSet(Follow[x.tokens[i]], First[x.tokens[j]], EMPTY);
					flag = isInSet(EMPTY, First[x.tokens[j]]);  // 判断@是否在First[Xj]中
					if (!flag) break;
				}
				if (flag) mergeSet(Follow[x.tokens[i]], Follow[x.name]);  // 将Follow[A]合并到Follow[Xi]中
			}
		}
		lastSize = newSize, newSize = FollowAllSize();  // 更新新旧Follow集合大小的总和
	}
}


// 求CLOSURE(I) - LR(0)  
void scan::closure0(int id) {
	int lastSize = -1, newSize = LR0Items[id].size();
	while (lastSize != newSize) {
		lastSize = newSize;
		for (int i = 0; i < newSize; ++i) {
			if (LR0Items[id].LR0Grams[i].gram.tokens.empty()) continue; // 如果没有待接收tokens，跳过  
			GRAM gram = LR0Items[id].LR0Grams[i].gram;  // 当前文法  
			string newName = gram.tokens[0];  // 首个token  
			if (isTerminal(newName)) continue;  // 如果首个token是终结符，跳过  
			for (auto& newGram : Grams) {  // 遍历原文法  
				if (newGram.name != newName) continue;  // 匹配左部  
				// 新增文法  
				LR0GRAM newLR0({ newGram, {} });
				if (find(LR0Items[id].LR0Grams.begin(), LR0Items[id].LR0Grams.end(), newLR0) != LR0Items[id].LR0Grams.end())
					continue;  // 如果已经存在，跳过  
				LR0Items[id].LR0Grams.push_back(newLR0);
			}
		}
		newSize = LR0Items[id].size(); // 更新项目数量  
	}
}

// LR(0) Go函数  
void scan::go0(int id, const string& token) {
	bool flag = false;  // 跳转是否成功  
	int newID = LR0Items.size();  // 新项目的ID  
	LR0ITEM newItem({ newID });    // 创建新的LR(0)项目集  
	for (auto& x : LR0Items[id].LR0Grams) {  // 遍历当前项目中的所有文法  
		if (x.gram.tokens.empty() || x.gram.tokens[0] != token) continue;
		flag = true;  // 跳转成功  
		GRAM newGram = x.gram;  // 当前文法  
		vector<string> newFormer = x.former; // 记录已经接受的tokens  
		newFormer.push_back(newGram.tokens[0]); // 移动待接收tokens  
		newGram.tokens.erase(newGram.tokens.begin());  // 移除已接收的token  
		newItem.LR0Grams.push_back({ newGram, newFormer }); // 新项目中加入新的文法  
	}
	if (!flag) return;  // 跳转失败，直接返回  
	LR0Items.push_back(newItem);  // 添加新项目  
	closure0(newID);  // 求新项目的CLOSURE  
}

// 构建LR(0)的项目集  
void scan::buildLR0Items() {
	LR0Items.push_back({ 0, {{Grams[0], {}} } }); // 初始化项目集  
	closure0(0);  // 求初始项目的CLOSURE  
	int id = 0;
	while (id < LR0Items.size()) {
		// 获取下一个可以接收的tokens  
		set<string> nextTokens;
		for (auto& x : LR0Items[id].LR0Grams) {
			if (x.gram.tokens.empty()) continue;
			nextTokens.insert(x.gram.tokens[0]);
		}
		for (const auto& token : nextTokens) {
			go0(id, token);  // 跳转到新项目  
		}
		++id;  // 更新当前项目ID  
	}
}


bool scan::SLR1Fun1() {
	for (const auto& item : LR0Items) {
		for (const auto& gram : item.LR0Grams) {
			string nextToken = getNextInputToken(gram);
			if (isReducePossible(gram) && isShiftPossible(item, nextToken)) {
				return true; // 存在移进-规约冲突  
			}
		}
	}
	return false; // 没有发现冲突  
}

bool scan::SLR1Fun2() {
	for (const auto& item : LR0Items) {
		set<string> reductions;
		for (const auto& gram : item.LR0Grams) {
			string reductionRule = getReductionRule(gram);
			if (reductions.count(reductionRule)) {
				return true; // 存在规约-规约冲突  
			}
			reductions.insert(reductionRule);
		}
	}
	return false; // 没有发现规约冲突  
}

string scan::getNextInputToken(const LR0GRAM& g) {
	auto it = g.gram.tokens.begin();
	while (it != g.gram.tokens.end()) {
		if (*it == ".") {
			++it;
			if (it != g.gram.tokens.end()) {
				return *it;
			}
			break;
		}
		++it;
	}
	return "";
}

bool scan::isReducePossible(const LR0GRAM& g) {
	return g.gram.tokens.empty() || (g.gram.tokens.size() == 1 && g.gram.tokens[0] == "ε");
}

bool scan::isShiftPossible(const LR0ITEM& item, const string& token) {
	// 此处需从item或相应结构中获取可以移进的tokens信息  
	set<string> shiftTokens; // 假设这里填充shiftTokens  
	return shiftTokens.count(token) > 0;
}

string scan::getReductionRule(const LR0GRAM& g) {
	return g.gram.name;
}

int scan::SLR1Analyse()
{
	bool flag1 = SLR1Fun1();
	bool flag2 = SLR1Fun2();
	if (flag1 && flag2)
	{
		return 3; // 两个冲突都有
	}
	else if (flag1)
	{
		return 1; // 移进规约冲突
	}
	else if (flag2)
	{
		return 2; // 规约规约冲突
	}
	// 没有冲突，是SLR(1)文法
	return 0;
}


bool scan::isLR1InItem(int id, const LR1GRAM& A) {
	for (auto& x : LR1Items[id].LRGrams)
		if (x == A) return true;
	return false;
}

int scan::getItemID(const LR1ITEM& A) {
	for (auto& item : LR1Items)
		if (item.LRGrams == A.LRGrams)
			return item.id;
	return -1;
}

// 求CLOSURE
void scan::closure(int id) {
	int lastSize = -1, newSize = LR1Items[id].size();
	while (lastSize != newSize) {  // 如果该项目的文法数量不再变化，就跳出循环
		lastSize = newSize;
		for (int i = 0; i < newSize; ++i) {
			if (LR1Items[id].LRGrams[i].gram.tokens.empty()) continue;  // 如果没有待接收的tokens了，就跳过
			GRAM gram = LR1Items[id].LRGrams[i].gram;  // 创建GRAM变量记录当前项目的LR(1)文法[i]的文法规则
			string newName = gram.tokens[0];  // 记录要接收的第一个
			if (isTerminal(newName)) continue;  // 首个token是终结符就不用再求closure
			for (auto& newGram : Grams) {  // 遍历原有的全部文法规则
				if (newGram.name != newName) continue;  // 如果当前文法的左部不等于newName，跳过
				// 求下一个可以接收的终结符
				// 如果当前的token已经是最后一个待接收token了，那就继承之前的下一个终结符
				set<string> terms = gram.tokens.size() > 1 ?
					First[gram.tokens[1]] : set<string>({ LR1Items[id].LRGrams[i].term });
				for (auto& newTerm : terms) {  // 新增文法
					LR1GRAM newLR1({ newGram, newTerm });
					if (isLR1InItem(id, newLR1)) continue;  // 如果当前文法已经存在该项目，跳过
					LR1Items[id].LRGrams.push_back(newLR1);
				}
			}
		}
		newSize = LR1Items[id].size();  // 更新当前的文法数量
	}
}
// LR1Items[id]接收token后，得到新的newItem=LR1Items[newID]
void scan::go(int id, string token) {
	bool flag = false;  // 跳转是否成功的标识符
	int newID = LR1Items.size();  // 创建新的ID
	LR1ITEM newItem({ newID });  // 新建一个LR(1)项目
	for (auto& x : LR1Items[id].LRGrams) {  // 遍历起点项目的所有文法
		// 如果待接收的tokens为空 或者 第一个待接收的token不等于当前的目标token，则跳过
		if (x.gram.tokens.empty() || x.gram.tokens[0] != token) continue;
		flag = true;  // 跳转成功
		GRAM newGram = x.gram;  // 记录变量
		vector<string> newFormer = x.former;  // 记录变量
		newFormer.push_back(newGram.tokens[0]);  // 把原来待接收的tokens第一个token移到新的former中
		newGram.tokens.erase(newGram.tokens.begin());  // 删除待接收的tokens中第一个token
		newItem.LRGrams.push_back({ newGram, x.term , newFormer });  // 向新的item中加入新的文法
	}
	if (!flag) return;  // 跳转失败，直接结束
	LR1Items.push_back(newItem);  // 加入新项目item
	closure(newID);  // 紧接着求这个新项目的CLOSURE
	int tmpID = getItemID(LR1Items[newID]);  // 求新的item在项目集中第一次出现时的下标
	// 如果找到的下标不等于新的ID，说明该item在接收token后成环，然后去掉这个新加的item
	if (newID != tmpID) LR1Items.pop_back();
	LR1Edges.push_back({ id, tmpID, token });  // 添加边数据
}


void scan::getNextTokens(int id) {
	nextTokens.clear();
	for (auto& x : LR1Items[id].LRGrams) {
		if (x.gram.tokens.empty()) continue;
		if (!isInVec(x.gram.tokens[0], nextTokens))
			nextTokens.push_back(x.gram.tokens[0]);
	}
}


// 构建LR(1)的项目集
void scan::buildLR1Item() {
	LR1Items.push_back({ 0, {{Grams[0], END}} });  // 用扩充的开始文法初始化LR(1)项目集
	closure(0);  // 立即求新item的CLOSURE
	int id = 0;
	while (id < LR1Items.size()) {  // 如果没有新增的item，id会到达临界值而跳出循环
		getNextTokens(id);  // 获取Items[id]下一次可以接收的tokens
		for (auto& token : nextTokens)
			go(id, token);  // 跳转到新item
		++id;
	}
}


// 构建LR(1)分析表
void scan::buildLR1Table() {
	// 移进动作 or 跳转动作
	for (auto& e : LR1Edges) {
		if (isTerminal(e.s)) LR1ACTION[{e.x, e.s}] = 's' + to_string(e.y);
		else LR1GOTO[{e.x, e.s}] = to_string(e.y);
	}
	// 规约动作
	for (auto& item : LR1Items) {
		for (auto& LR1 : item.LRGrams) {
			if (!LR1.gram.tokens.empty()) continue;
			GRAM gram({ LR1.gram.name, LR1.former });
			int id = getGramID(gram);
			LR1ACTION[{item.id, LR1.term}] = 'r' + to_string(id);
		}
	}
	LR1ACTION[{1, END}] = ACCEPT;
}


