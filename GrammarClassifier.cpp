#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <unordered_map>
using namespace std;
// 最大长度
const int N = 1000;
// 分隔符
const char *delim = ",";

// 文法类
class Grammar
{
public:
	vector<string> VN;
	vector<string> VT;
	vector<string> P;
	string S;
};

// 分割输入的字符串：以逗号作为分隔符
vector<string> split(const string &str, const string &delim)
{
	vector<string> res;
	if ("" == str)
		return res;
	char *strs = new char[str.length() + 1];
	strcpy(strs, str.c_str());
	char *d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());
	char *p = strtok(strs, d);
	while (p)
	{
		string s = p;
		res.push_back(s);
		p = strtok(NULL, d);
	}
	return res;
}

// 判断产生式是否合法
// 产生式合法与否的原则是：判断文法产生式中是否出现了未定义的非终结符，若出现，则该文法是非法的
bool check(const Grammar &G)
{
	vector<string> tmp = G.VN;
	unordered_map<string, int> m;
	for (auto x : tmp)
		m[x]++;
	for (auto x : G.P)
	{
		for (int i = 0; i < x.length(); i++)
		{
			string t = "";
			t = x[i];
			if (x[i] >= 'A' && x[i] <= 'Z' && !m[t])
				return false;
		}
	}
	return true;
}

bool JudgeZero(const Grammar &G)
{
	bool flag = false;
	vector<string> tmp = G.P;
	for (string str : tmp)
	{
		// 扫描每一条文法产生式，并判断其左部是否存在非终结符
		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] == ':' && str[i + 1] == ':' && str[i + 2] == '=')
			{
				for (int j = 0; j < i; j++)
				{
					if (str[j] >= 'A' && str[j] <= 'Z')
					{
						flag = true;
						break;
					}
				}
				break;
			}
		}
	}
	// 若没有非终结符，返回假
	return flag;
}

bool JudgeOne(const Grammar &G)
{
	// left记录产生式的左部
	// right记录产生式的右部
	bool flag = false;
	string left;
	string right;
	for (string str : G.P)
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] == ':' && str[i + 1] == ':' && str[i + 2] == '=')
			{
				left = str.substr(0, i);
				right = str.substr(i + 3, str.length() - i - 3);
			}
			if (right.length() >= left.length())
				flag = true;
			else if (left.length() == 1 && right == "ε")
				flag = true;
			else if (left.length() > right.length())
				return false;
		}
	}
	return flag;
}

bool JudgeTwo(const Grammar &G)
{
	string left = "";
	for (auto str : G.P)
	{
		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] == ':' && str[i + 1] == ':' && str[i + 2] == '=')
			{
				left = str.substr(0, i);
				if (left.length() > 1) return false;
			}
		}
	}
	return true;
}

bool JudgeThree(const Grammar &G)
{
	string right = "";
	// flag数组记录所有产生式的线性：1表示左线性，2表示右线性
	int idx = 0;
	int flag[N] = {0};
	for (auto str : G.P)
	{
		for (int i = 0; i <= str.length(); i++)
		{
			if (str[i] == ':' && str[i + 1] == ':' && str[i + 2] == '=')
			{
				right = str.substr(i + 3, str.length() - i - 3);
				if (right.length() == 0)
					return false;
				if (right.length() == 2)
				{
					// 产生式右部是两个非终结符，返回假
					if (right[0] >= 'A' && right[0] <= 'Z' && right[1] >= 'A' && right[1] <= 'Z')
						return false;
				}
				if (right.length() > 2 && right.length() <= 5)
				{
					int t = right.find('|');
					if (t == -1)
						return false;
					string l = right.substr(0, t);
					string r = right.substr(t + 1, right.length() - t - 1);
					// val_l和val_r记录左线性还是右线性：1表示左线性，2表示右线性
					// flag1用来记录特殊情况：l或r有一个长度为1，则flag1=1
					// cout << l << r << endl;
					int val_l = 0, val_r = 0;
					int flag1 = 0;
					if (l.length() == 2)
					{
						if (l[0] >= 'A' && l[0] <= 'Z' && l[1] >= 'A' && l[1] <= 'Z')
							return false;
						if (l[0] >= 'A' && l[0] <= 'Z')
							val_l = 1;
						else
							val_l = 2;
					}
					if (r.length() == 2)
					{
						if (r[0] >= 'A' && r[0] <= 'Z' && r[1] >= 'A' && r[1] <= 'Z')
							return false;
						if (r[0] >= 'A' && r[0] <= 'Z')
							val_r = 1;
						else
							val_r = 2;
					}
					if (l.length() == 1 || r.length() == 1)
						flag1 = 1;
					// 左右线性不一致直接返回假
					if (val_l != val_r && !flag1)
						return false;
					// 用数组记录下当前文法产生式的线性
					// S->Ab|B
					// 此时val_l为1，但是val_r为0，所以是左线性
					flag[idx++] = val_l == 0 ? val_r : val_l;
				}
				if (right.length() > 5)
				{
					// 若当前产生式右部有多个|链接，我们需要依次判断每两个|之间的式子是否符合要求
					// 例如：S::=Sa|b|c|d
					vector<string> t = split(right, "|");
					// 与flag数组一样，v数组记录这些子表达式是左线性还是右线性的
					int cnt = 0;
					int v[N] = {0};
					for (int j = 0; j < t.size(); j++)
					{
						// cout << t[j];
						if (t[j].size() > 2)
							return false;
						string tmp = t[j];
						if (tmp.length() == 2)
						{
							if (tmp[0] >= 'A' && tmp[0] <= 'Z' && tmp[1] >= 'A' && tmp[1] <= 'Z')
								return false;
							if (tmp[0] >= 'A' && tmp[0] <= 'Z')
								v[cnt] = 1;
							else
								v[cnt] = 2;
						}
						cnt++;
					}
					for (int i = 0; i < cnt - 1; ++i)
					{
						if (v[cnt] != v[cnt + 1] && v[cnt] != 0 && v[cnt + 1] != 0)
							return false;
					}
					flag[idx] = v[cnt - 1];
				}
				break;
			}
		}
	}
	for (int i = 0; i < idx - 1; ++i)
	{
		if (flag[i] != flag[i + 1] && flag[i] != 0 && flag[i + 1] != 0)
			return false;
	}
	return true;
}

// 判断文法产生式的类型
int cmp(const Grammar &G)
{
	// 非法的文法返回-1
	int flag = -1;
	if (!check(G))
		return flag;
	if (JudgeZero(G))
		flag = 0;
	if (JudgeOne(G) && flag == 0)
		flag = 1;
	if (JudgeTwo(G) && flag == 1)
		flag = 2;
	if (JudgeThree(G) && flag == 2)
		flag = 3;
	return flag;
}

int main()
{
	int flag = 1;
	while (flag)
	{
		system("cls");
		cout << "输入 1 进行文法检测，输入 0 退出：" << endl;
		cin >> flag;
		if (flag == 0)
			break;
		system("cls");
		cout << "请按照提示输入文法G：" << endl;
		cout << endl;
		cout << "文法输入格式：\n1、第一行输入非终结符\n2、第二行输入文法产生式\n3、第三行输入开始符号" << endl;
		cout << endl;
		cout << "例如：\n\tS,A,B\n\tS::=a|A,A|Bb,B::=b\n\tS\n";
		ofstream ofs;
		ofs.open("text.txt", ios::out | ios::trunc);
		for (int i = 0; i < 3; i++)
		{
			if (i == 0)
				cout << "请输入非终结符(以逗号隔开;英文大写字母)：";
			else if (i == 1)
				cout << "请输入文法产生式P(以逗号隔开每个产生式)：";
			else if (i == 2)
				cout << "请输入开始符号：";
			string tmp;
			cin >> tmp;
			ofs << tmp << endl;
		}
		ofs.close();

		Grammar G;
		ifstream ifs;
		ifs.open("text.txt", ios::in);
		if (!ifs.is_open())
		{
			cout << "文件打开失败！" << endl;
			system("pause");
			return 0;
		}
		// buf1——buf4分别存储：非终结符，终结符，文法产生式，开始符号
		string buf1, buf2, buf3, buf4;
		getline(ifs, buf1);
		getline(ifs, buf3);
		getline(ifs, buf4);
		G.VN = split(buf1, delim);
		G.P = split(buf3, delim);
		G.S = buf4;
		// buf存储所有的终结符，且以逗号隔开
		string buf = "";
		unordered_map<char, int> mp;
		for (auto str : G.P)
		{
			for (int i = 0; i < str.length(); ++i)
			{
				if (str[i] == ':' && str[i+1] == ':' && str[i+2] == '=')
				{
					string tmp = str.substr(i+3, str.length() - i - 3);
					if (tmp == "ε")
					{
						buf = tmp;
						buf += ",";
						break;
					}
				}
				if ((str[i] < 'A' || str[i] > 'Z') && str[i] != ':' && str[i] != '=' && str[i] != '|' && !mp.count(str[i]))
				{
					mp[str[i]]++;
					buf += str[i];
					buf += ",";
				}
			}
		}
		// 去掉buf串的最后一个逗号，将前面的长度为n-1的字串赋值给buf2
		buf2 = buf.substr(0, buf.length() - 1);
		G.VT = split(buf2, delim);
		int flag = cmp(G);
		cout << endl;
		printf("您输入的文法G是:\n");
		// printf只能输出c语言支持的内容，所以要把buf1的首地址指针取出来
		// 否则会输出乱码
		if (buf2 == "") printf("G[%s] = ({%s}, {ε}, P, %s)\n", G.S.c_str(), buf1.c_str(), G.S.c_str());
		else printf("G[%s] = ({%s}, {%s}, P, %s)\n", G.S.c_str(), buf1.c_str(), buf2.c_str(), G.S.c_str());
		if (buf2 == "") G.P[0] += "ε";
		printf("其中,   P为\n");
		for (auto x : G.P)
		{
			printf("\t");
			cout << x << endl;
		}
		if (flag == -1)
			cout << "非法！" << endl;
		else
		{
			if (buf2 == "") printf("该文法是Chomsky%d型文法\n", flag+1);
			else printf("该文法是Chomsky%d型文法\n", flag);
		}
			
		cout << endl;
		ifs.close();
		system("pause");
	}
	cout << "欢迎下次使用！" << endl;
	system("pause");
	return 0;
}
