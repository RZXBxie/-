#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <unordered_map>
using namespace std;
// ��󳤶�
const int N = 1000;
// �ָ���
const char *delim = ",";

// �ķ���
class Grammar
{
public:
	vector<string> VN;
	vector<string> VT;
	vector<string> P;
	string S;
};

// �ָ�������ַ������Զ�����Ϊ�ָ���
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

// �жϲ���ʽ�Ƿ�Ϸ�
// ����ʽ�Ϸ�����ԭ���ǣ��ж��ķ�����ʽ���Ƿ������δ����ķ��ս���������֣�����ķ��ǷǷ���
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
		// ɨ��ÿһ���ķ�����ʽ�����ж������Ƿ���ڷ��ս��
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
	// ��û�з��ս�������ؼ�
	return flag;
}

bool JudgeOne(const Grammar &G)
{
	// left��¼����ʽ����
	// right��¼����ʽ���Ҳ�
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
			else if (left.length() == 1 && right == "��")
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
	// flag�����¼���в���ʽ�����ԣ�1��ʾ�����ԣ�2��ʾ������
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
					// ����ʽ�Ҳ����������ս�������ؼ�
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
					// val_l��val_r��¼�����Ի��������ԣ�1��ʾ�����ԣ�2��ʾ������
					// flag1������¼���������l��r��һ������Ϊ1����flag1=1
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
					// �������Բ�һ��ֱ�ӷ��ؼ�
					if (val_l != val_r && !flag1)
						return false;
					// �������¼�µ�ǰ�ķ�����ʽ������
					// S->Ab|B
					// ��ʱval_lΪ1������val_rΪ0��������������
					flag[idx++] = val_l == 0 ? val_r : val_l;
				}
				if (right.length() > 5)
				{
					// ����ǰ����ʽ�Ҳ��ж��|���ӣ�������Ҫ�����ж�ÿ����|֮���ʽ���Ƿ����Ҫ��
					// ���磺S::=Sa|b|c|d
					vector<string> t = split(right, "|");
					// ��flag����һ����v�����¼��Щ�ӱ��ʽ�������Ի��������Ե�
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

// �ж��ķ�����ʽ������
int cmp(const Grammar &G)
{
	// �Ƿ����ķ�����-1
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
		cout << "���� 1 �����ķ���⣬���� 0 �˳���" << endl;
		cin >> flag;
		if (flag == 0)
			break;
		system("cls");
		cout << "�밴����ʾ�����ķ�G��" << endl;
		cout << endl;
		cout << "�ķ������ʽ��\n1����һ��������ս��\n2���ڶ��������ķ�����ʽ\n3�����������뿪ʼ����" << endl;
		cout << endl;
		cout << "���磺\n\tS,A,B\n\tS::=a|A,A|Bb,B::=b\n\tS\n";
		ofstream ofs;
		ofs.open("text.txt", ios::out | ios::trunc);
		for (int i = 0; i < 3; i++)
		{
			if (i == 0)
				cout << "��������ս��(�Զ��Ÿ���;Ӣ�Ĵ�д��ĸ)��";
			else if (i == 1)
				cout << "�������ķ�����ʽP(�Զ��Ÿ���ÿ������ʽ)��";
			else if (i == 2)
				cout << "�����뿪ʼ���ţ�";
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
			cout << "�ļ���ʧ�ܣ�" << endl;
			system("pause");
			return 0;
		}
		// buf1����buf4�ֱ�洢�����ս�����ս�����ķ�����ʽ����ʼ����
		string buf1, buf2, buf3, buf4;
		getline(ifs, buf1);
		getline(ifs, buf3);
		getline(ifs, buf4);
		G.VN = split(buf1, delim);
		G.P = split(buf3, delim);
		G.S = buf4;
		// buf�洢���е��ս�������Զ��Ÿ���
		string buf = "";
		unordered_map<char, int> mp;
		for (auto str : G.P)
		{
			for (int i = 0; i < str.length(); ++i)
			{
				if (str[i] == ':' && str[i+1] == ':' && str[i+2] == '=')
				{
					string tmp = str.substr(i+3, str.length() - i - 3);
					if (tmp == "��")
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
		// ȥ��buf�������һ�����ţ���ǰ��ĳ���Ϊn-1���ִ���ֵ��buf2
		buf2 = buf.substr(0, buf.length() - 1);
		G.VT = split(buf2, delim);
		int flag = cmp(G);
		cout << endl;
		printf("��������ķ�G��:\n");
		// printfֻ�����c����֧�ֵ����ݣ�����Ҫ��buf1���׵�ַָ��ȡ����
		// ������������
		if (buf2 == "") printf("G[%s] = ({%s}, {��}, P, %s)\n", G.S.c_str(), buf1.c_str(), G.S.c_str());
		else printf("G[%s] = ({%s}, {%s}, P, %s)\n", G.S.c_str(), buf1.c_str(), buf2.c_str(), G.S.c_str());
		if (buf2 == "") G.P[0] += "��";
		printf("����,   PΪ\n");
		for (auto x : G.P)
		{
			printf("\t");
			cout << x << endl;
		}
		if (flag == -1)
			cout << "�Ƿ���" << endl;
		else
		{
			if (buf2 == "") printf("���ķ���Chomsky%d���ķ�\n", flag+1);
			else printf("���ķ���Chomsky%d���ķ�\n", flag);
		}
			
		cout << endl;
		ifs.close();
		system("pause");
	}
	cout << "��ӭ�´�ʹ�ã�" << endl;
	system("pause");
	return 0;
}
