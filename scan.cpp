#include "pch.h"
#include "scan.h"


void scan::initialize() {

	// ���ԭʼ�����ķ�����  
	lines.clear();
	// ��� First �� Follow  
	First.clear();
	Follow.clear();
	// ���� gramType  
	gramType = 0;  

	// ��� Grams ����  
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

// �ϲ��������ϣ��Ҽ��ϵ�Ԫ��(����except��)����ӵ��󼯺���
void scan::mergeSet(set<string>& A, const set<string>& B, string except) {
	for (auto& x : B)
		if (x != except) A.insert(x);
}
// �ж��ַ��Ƿ�Ϊ' ', '\n', '\t'
bool scan::isBlank(char c) {
	return c == ' ' || c == '\t' || c == '\n';
}

// ������Ż����ķ�
void scan::splitGrams() {
	
	vector<GRAM> grams = Grams;  // ��ʱ����
	Grams.clear();  // ���ԭ�����ķ�
	for (auto& gram : grams) {
		vector<string> tokens;  // ��¼��ǰ����ֺ��õ��Ҳ�tokens
		for (auto& x : gram.tokens) {
			if (x != "|") {  // û����"|"�ͼ�������Ҳ�
				tokens.push_back(x);
				continue;
			}
			Grams.push_back({ gram.name, tokens });  // ����"|"�������ķ�
			tokens.clear();  // ��յ�ǰ�Ҳ���¼��׼����һ�εı���
		}
		// (����ĩβ����)�������ʣ����Ҳ���ֱ�Ӽ��뵽Grams��
		if (!tokens.empty()) Grams.push_back({ gram.name, tokens });
	}
}

// ���俪ʼ�ķ�����A->a������A'->A  (��������ݹ飬�������ķ�)
void scan::extendStartGram() {
	
	suffix = (gramType == 1 ? '\'' : 'i');
	string newName = Grams[0].name + suffix;

	while (isNonterm(newName))  // ����������Ѵ��ڣ���һֱ����׷��suf
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
	string name;  // �ķ��Ƶ���ʽֻ������ĸ��"'"���
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
		save = true;  // �����ַ���c׷�ӵ�token��
		switch (scanState) {
		case START:
			if (isupper(c))
				scanState = NONTERM;  // ���ս��
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
		save = true;  // �����ַ���c׷�ӵ�token��
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
// �����һ�����ս���ĵ�һ���ַ��Ǵ�д��ĸ����Type=1�������Сд��ĸ����Type=2
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



// ��ȡȫ��First���ϵĴ�С�ܺ�
int scan::FirstAllSize() {
	int size = 0;
	for (auto& x : First)
		size += x.second.size();
	return size;
}

// ��������ȫ��First����
void scan::buildFirstAll() {
	int lastSize = -1, newSize = 0;
	while (lastSize != newSize) {
		for (auto& gram : Grams) {
			bool flag = false;  // ��־�����жϵ�ǰ�Ҳ���token��First(token)�Ƿ����@��������ڣ��͵ü�����������Ҳ�
			for (auto& token : gram.tokens) {  // �����ķ����Ҳ�
				if (isTerminal(token)) First[token] = { token };  // �ս����First����ֻ������
				mergeSet(First[gram.name], First[token], EMPTY);  // ��First[token] - @�ϲ���First[��]
				flag = isInSet(EMPTY, First[token]);  // ���@��First[token]�������������
				if (!flag) break;  // ���@����First[token]��ֹͣ����
			}
			if (flag) First[gram.name].insert(EMPTY);  // ����������Ҳ���flag����true,˵��@��First[�Ҳ�]
		}
		lastSize = newSize, newSize = FirstAllSize();  // ����ȫ��First���ϵĴ�С
	}
	// ���仹û���������ս����First����
	for (auto& gram : Grams)
		for (auto& token : gram.tokens)
			if (isTerminal(token)) First[token] = { token };
}

// ��ȡȫ��Follow���ϵĴ�С�ܺ�
int scan::FollowAllSize() {
	int size = 0;
	for (auto& x : Follow)
		size += x.second.size();
	return size;
}

// ��������ȫ��Follow����
void scan::buildFollowAll() {
	int lastSize = -1, newSize = 0;
	Follow[Grams[0].name] = { END };  // ��ʼ��(Ĭ�ϵ�һ���ķ�����Ϊ��ʼ����)
	newSize = FollowAllSize();  // ��ǰȫ��Follow���ϵĴ�С�ܺ�
	while (lastSize != newSize) {  // ���ȫ��Follow���ϵĴ�С���ٱ仯������������
		for (auto& x : Grams) {
			for (int i = 0; i < x.tokens.size(); ++i) {  // �����Ҳ���ÿһ��token
				if (isTerminal(x.tokens[i])) continue;  // ֻ�з��ս������Follow����
				bool flag = (i == x.tokens.size() - 1);  // �ж�@�Ƿ�����First(Xi+1Xi+2...Xn)
				for (int j = i + 1; j < x.tokens.size(); ++j) {  // ����token[i]���tokens
					mergeSet(Follow[x.tokens[i]], First[x.tokens[j]], EMPTY);
					flag = isInSet(EMPTY, First[x.tokens[j]]);  // �ж�@�Ƿ���First[Xj]��
					if (!flag) break;
				}
				if (flag) mergeSet(Follow[x.tokens[i]], Follow[x.name]);  // ��Follow[A]�ϲ���Follow[Xi]��
			}
		}
		lastSize = newSize, newSize = FollowAllSize();  // �����¾�Follow���ϴ�С���ܺ�
	}
}


// ��CLOSURE(I) - LR(0)  
void scan::closure0(int id) {
	int lastSize = -1, newSize = LR0Items[id].size();
	while (lastSize != newSize) {
		lastSize = newSize;
		for (int i = 0; i < newSize; ++i) {
			if (LR0Items[id].LR0Grams[i].gram.tokens.empty()) continue; // ���û�д�����tokens������  
			GRAM gram = LR0Items[id].LR0Grams[i].gram;  // ��ǰ�ķ�  
			string newName = gram.tokens[0];  // �׸�token  
			if (isTerminal(newName)) continue;  // ����׸�token���ս��������  
			for (auto& newGram : Grams) {  // ����ԭ�ķ�  
				if (newGram.name != newName) continue;  // ƥ����  
				// �����ķ�  
				LR0GRAM newLR0({ newGram, {} });
				if (find(LR0Items[id].LR0Grams.begin(), LR0Items[id].LR0Grams.end(), newLR0) != LR0Items[id].LR0Grams.end())
					continue;  // ����Ѿ����ڣ�����  
				LR0Items[id].LR0Grams.push_back(newLR0);
			}
		}
		newSize = LR0Items[id].size(); // ������Ŀ����  
	}
}

// LR(0) Go����  
void scan::go0(int id, const string& token) {
	bool flag = false;  // ��ת�Ƿ�ɹ�  
	int newID = LR0Items.size();  // ����Ŀ��ID  
	LR0ITEM newItem({ newID });    // �����µ�LR(0)��Ŀ��  
	for (auto& x : LR0Items[id].LR0Grams) {  // ������ǰ��Ŀ�е������ķ�  
		if (x.gram.tokens.empty() || x.gram.tokens[0] != token) continue;
		flag = true;  // ��ת�ɹ�  
		GRAM newGram = x.gram;  // ��ǰ�ķ�  
		vector<string> newFormer = x.former; // ��¼�Ѿ����ܵ�tokens  
		newFormer.push_back(newGram.tokens[0]); // �ƶ�������tokens  
		newGram.tokens.erase(newGram.tokens.begin());  // �Ƴ��ѽ��յ�token  
		newItem.LR0Grams.push_back({ newGram, newFormer }); // ����Ŀ�м����µ��ķ�  
	}
	if (!flag) return;  // ��תʧ�ܣ�ֱ�ӷ���  
	LR0Items.push_back(newItem);  // �������Ŀ  
	closure0(newID);  // ������Ŀ��CLOSURE  
}

// ����LR(0)����Ŀ��  
void scan::buildLR0Items() {
	LR0Items.push_back({ 0, {{Grams[0], {}} } }); // ��ʼ����Ŀ��  
	closure0(0);  // ���ʼ��Ŀ��CLOSURE  
	int id = 0;
	while (id < LR0Items.size()) {
		// ��ȡ��һ�����Խ��յ�tokens  
		set<string> nextTokens;
		for (auto& x : LR0Items[id].LR0Grams) {
			if (x.gram.tokens.empty()) continue;
			nextTokens.insert(x.gram.tokens[0]);
		}
		for (const auto& token : nextTokens) {
			go0(id, token);  // ��ת������Ŀ  
		}
		++id;  // ���µ�ǰ��ĿID  
	}
}


bool scan::SLR1Fun1() {
	for (const auto& item : LR0Items) {
		for (const auto& gram : item.LR0Grams) {
			string nextToken = getNextInputToken(gram);
			if (isReducePossible(gram) && isShiftPossible(item, nextToken)) {
				return true; // �����ƽ�-��Լ��ͻ  
			}
		}
	}
	return false; // û�з��ֳ�ͻ  
}

bool scan::SLR1Fun2() {
	for (const auto& item : LR0Items) {
		set<string> reductions;
		for (const auto& gram : item.LR0Grams) {
			string reductionRule = getReductionRule(gram);
			if (reductions.count(reductionRule)) {
				return true; // ���ڹ�Լ-��Լ��ͻ  
			}
			reductions.insert(reductionRule);
		}
	}
	return false; // û�з��ֹ�Լ��ͻ  
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
	return g.gram.tokens.empty() || (g.gram.tokens.size() == 1 && g.gram.tokens[0] == "��");
}

bool scan::isShiftPossible(const LR0ITEM& item, const string& token) {
	// �˴����item����Ӧ�ṹ�л�ȡ�����ƽ���tokens��Ϣ  
	set<string> shiftTokens; // �����������shiftTokens  
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
		return 3; // ������ͻ����
	}
	else if (flag1)
	{
		return 1; // �ƽ���Լ��ͻ
	}
	else if (flag2)
	{
		return 2; // ��Լ��Լ��ͻ
	}
	// û�г�ͻ����SLR(1)�ķ�
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

// ��CLOSURE
void scan::closure(int id) {
	int lastSize = -1, newSize = LR1Items[id].size();
	while (lastSize != newSize) {  // �������Ŀ���ķ��������ٱ仯��������ѭ��
		lastSize = newSize;
		for (int i = 0; i < newSize; ++i) {
			if (LR1Items[id].LRGrams[i].gram.tokens.empty()) continue;  // ���û�д����յ�tokens�ˣ�������
			GRAM gram = LR1Items[id].LRGrams[i].gram;  // ����GRAM������¼��ǰ��Ŀ��LR(1)�ķ�[i]���ķ�����
			string newName = gram.tokens[0];  // ��¼Ҫ���յĵ�һ��
			if (isTerminal(newName)) continue;  // �׸�token���ս���Ͳ�������closure
			for (auto& newGram : Grams) {  // ����ԭ�е�ȫ���ķ�����
				if (newGram.name != newName) continue;  // �����ǰ�ķ����󲿲�����newName������
				// ����һ�����Խ��յ��ս��
				// �����ǰ��token�Ѿ������һ��������token�ˣ��Ǿͼ̳�֮ǰ����һ���ս��
				set<string> terms = gram.tokens.size() > 1 ?
					First[gram.tokens[1]] : set<string>({ LR1Items[id].LRGrams[i].term });
				for (auto& newTerm : terms) {  // �����ķ�
					LR1GRAM newLR1({ newGram, newTerm });
					if (isLR1InItem(id, newLR1)) continue;  // �����ǰ�ķ��Ѿ����ڸ���Ŀ������
					LR1Items[id].LRGrams.push_back(newLR1);
				}
			}
		}
		newSize = LR1Items[id].size();  // ���µ�ǰ���ķ�����
	}
}
// LR1Items[id]����token�󣬵õ��µ�newItem=LR1Items[newID]
void scan::go(int id, string token) {
	bool flag = false;  // ��ת�Ƿ�ɹ��ı�ʶ��
	int newID = LR1Items.size();  // �����µ�ID
	LR1ITEM newItem({ newID });  // �½�һ��LR(1)��Ŀ
	for (auto& x : LR1Items[id].LRGrams) {  // ���������Ŀ�������ķ�
		// ��������յ�tokensΪ�� ���� ��һ�������յ�token�����ڵ�ǰ��Ŀ��token��������
		if (x.gram.tokens.empty() || x.gram.tokens[0] != token) continue;
		flag = true;  // ��ת�ɹ�
		GRAM newGram = x.gram;  // ��¼����
		vector<string> newFormer = x.former;  // ��¼����
		newFormer.push_back(newGram.tokens[0]);  // ��ԭ�������յ�tokens��һ��token�Ƶ��µ�former��
		newGram.tokens.erase(newGram.tokens.begin());  // ɾ�������յ�tokens�е�һ��token
		newItem.LRGrams.push_back({ newGram, x.term , newFormer });  // ���µ�item�м����µ��ķ�
	}
	if (!flag) return;  // ��תʧ�ܣ�ֱ�ӽ���
	LR1Items.push_back(newItem);  // ��������Ŀitem
	closure(newID);  // ���������������Ŀ��CLOSURE
	int tmpID = getItemID(LR1Items[newID]);  // ���µ�item����Ŀ���е�һ�γ���ʱ���±�
	// ����ҵ����±겻�����µ�ID��˵����item�ڽ���token��ɻ���Ȼ��ȥ������¼ӵ�item
	if (newID != tmpID) LR1Items.pop_back();
	LR1Edges.push_back({ id, tmpID, token });  // ��ӱ�����
}


void scan::getNextTokens(int id) {
	nextTokens.clear();
	for (auto& x : LR1Items[id].LRGrams) {
		if (x.gram.tokens.empty()) continue;
		if (!isInVec(x.gram.tokens[0], nextTokens))
			nextTokens.push_back(x.gram.tokens[0]);
	}
}


// ����LR(1)����Ŀ��
void scan::buildLR1Item() {
	LR1Items.push_back({ 0, {{Grams[0], END}} });  // ������Ŀ�ʼ�ķ���ʼ��LR(1)��Ŀ��
	closure(0);  // ��������item��CLOSURE
	int id = 0;
	while (id < LR1Items.size()) {  // ���û��������item��id�ᵽ���ٽ�ֵ������ѭ��
		getNextTokens(id);  // ��ȡItems[id]��һ�ο��Խ��յ�tokens
		for (auto& token : nextTokens)
			go(id, token);  // ��ת����item
		++id;
	}
}


// ����LR(1)������
void scan::buildLR1Table() {
	// �ƽ����� or ��ת����
	for (auto& e : LR1Edges) {
		if (isTerminal(e.s)) LR1ACTION[{e.x, e.s}] = 's' + to_string(e.y);
		else LR1GOTO[{e.x, e.s}] = to_string(e.y);
	}
	// ��Լ����
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


