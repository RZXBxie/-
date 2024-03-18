#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

const char *DELIM = ",";
const char *NONTERMINALS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char *TERMINALS = "abcdefghijklmnopqrstuvwxyz";
const char *SPECIAL_TERMINALS = "ε ";

static vector<string> split(const string &str, const string &delim);

class Grammar
{
private:
    vector<string> VN;
    vector<string> VT;
    vector<string> P;
    string S;

    void generateVT()
    {
        for (string &production : P)
        {
            for (char c : production)
            {
                if (c >= 'a' && c <= 'z' || c == SPECIAL_TERMINALS[0])
                {
                    VT.push_back(string(1, c));
                }
            }
        }
        sort(VT.begin(), VT.end());
        VT.erase(unique(VT.begin(), VT.end()), VT.end());
    }

public:
    Grammar() {}
    Grammar(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cout << "Error opening file " << filename << endl;
            return;
        }
        string line;
        size_t count = 0;
        while (getline(file, line))
        {
            count += 1;
            vector<string> parts = split(line, DELIM);
            if (count == 1)
            {
                VN = parts;
            }
            else if (count == 2)
            {
                P = parts;
            }
            else if (count == 3)
            {
                S = parts[0];
            }
        }
        file.close();
        generateVT();
    }
    Grammar(const vector<string> &vn, const vector<string> &p, const string &s)
    {
        VN = vn;
        P = p;
        S = s;
        generateVT(); // 自动生成VT
    }

    void setVN(const vector<string> &vn) { VN = vn; }
    void setP(const vector<string> &p)
    {
        P = p;
        generateVT(); // 自动生成VT
    }
    void setS(const string &s) { S = s; }

    vector<string> getVN() const { return VN; }
    vector<string> getVT() const { return VT; }
    vector<string> getP() const { return P; }
    string getS() const { return S; }
};

class GrammarClassifer
{
private:
    Grammar G;

    bool checkValidation()
    {
        for (const string &prodcution : G.getP())
        {
            size_t pos = prodcution.find("::=");
            if (pos != string::npos)
            {
                string left = prodcution.substr(0, pos);
                string right = prodcution.substr(pos + 3);
                if (!left.length() || !right.length())
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    bool checkNonterminalSymbols()
    {
        unordered_map<string, bool> map;
        for (const string &symbol : G.getVN())
            map[symbol] = true;
        for (const string &production : G.getP())
        {
            for (char c : production)
            {
                string s = string(1, c);
                if (c >= 'A' && c <= 'Z' && map.find(s) == map.end())
                    return false;
            }
        }
        return true;
    }

    bool isZeroType()
    {
        for (const string &production : G.getP())
        {
            size_t pos = production.find("::=");
            if (pos != string::npos)
            {
                string left = production.substr(0, pos);
                if (left.find_first_of(NONTERMINALS) == string::npos)
                    return false;
            }
            else
            {
                throw runtime_error("Invalid production: '::=' not found");
            }
        }
        return true;
    }

    bool isOneType()
    {
        for (const string &production : G.getP())
        {
            size_t pos = production.find("::=");
            if (pos != string::npos)
            {
                string left = production.substr(0, pos);
                string right = production.substr(pos + 3);
                if (left.length() > right.length())
                    return false;
            }
            else
            {
                throw runtime_error("Invalid production: '::=' not found");
            }
        }
        return true;
    }

    bool isTwoType()
    {
        for (const string &production : G.getP())
        {
            size_t pos = production.find("::=");
            if (pos != string::npos)
            {
                string left = production.substr(0, pos);
                if (left.length() != 1)
                    return false;
            }
            else
            {
                throw runtime_error("Invalid production: '::=' not found");
            }
        }
        return true;
    }

    bool isThreeType()
    {
        int left_linear = 0, right_linear = 0; // 记录每条产生式的左线性和右线性
        for (const string &production : G.getP())
        {
            size_t pos = production.find("::=");
            if (pos != string::npos)
            {
                string left = production.substr(0, pos);
                if (left.size() != 1)
                {
                    return false;
                }
                string right = production.substr(pos + 3);
                vector<string> strs = split(right, "|");
                for (const string &str : strs)
                {
                    if (str.find_first_of(TERMINALS) == string::npos)
                    {
                        return false;
                    }
                    if (str.length() > 2)
                    {
                        return false;
                    }
                    if (str.length() == 2)
                    {
                        if (str[0] <= 'z' && str[0] >= 'a' && str[1] <= 'Z' && str[1] >= 'A')
                        {
                            right_linear++;
                        }
                        else if (str[0] <= 'Z' && str[0] >= 'A' && str[1] <= 'z' && str[1] >= 'a')
                        {
                            left_linear++;
                        }
                    }
                }
            }
            else
            {
                throw runtime_error("Invalid production: '::=' not found");
            }
        }
        return !left_linear || !right_linear;
    }

public:
    GrammarClassifer(const Grammar &grammar) : G(grammar) {}

    int checkType()
    {
        int flag = -2;
        if (!checkValidation())
            return -2;
        if (!checkNonterminalSymbols())
            return -1;
        if (isZeroType())
            flag = 0;
        if (isOneType())
            flag = 1;
        if (isTwoType())
            flag = 2;
        if (isThreeType())
            flag = 3;
        return flag;
    }
};

static vector<string> split(const string &str, const string &delim)
{
    vector<string> res;
    size_t start = 0, end;
    while ((end = str.find(delim, start)) != string::npos)
    {
        res.push_back(str.substr(start, end - start));
        start = end + delim.length();
    }
    res.push_back(str.substr(start));
    return res;
}

static void runGrammarCheck()
{
    cout << "请按照提示输入文法G：" << endl;
    cout << "文法输入格式：\n1、第一行输入非终结符\n2、第二行输入文法产生式\n3、第三行输入开始符号" << endl;
    cout << "例如：\n\tS,A,B\n\tS::=a|A,A|Bb,B::=b\n\tS\n";

    Grammar G;
    for (int i = 0; i < 3; i++)
    {
        if (i == 0)
            cout << "请输入非终结符(以逗号隔开英文大写字母)：";
        else if (i == 1)
            cout << "请输入文法产生式P(以逗号隔开每个产生式)：";
        else if (i == 2)
            cout << "请输入开始符号：";
        string tmp;
        cin >> tmp;
        vector<string> parts = split(tmp, DELIM);
        switch (i)
        {
        case 0:
            G.setVN(parts);
            break;
        case 1:
            G.setP(parts);
            break;
        case 2:
            G.setS(parts[0]);
            break;
        default:
            break;
        }
    }

    GrammarClassifer checker(G);
    int type = checker.checkType();

    try
    {
        if (type == -2)
            throw runtime_error("Invalid production: either '::=' not found or no left_side or right_side over '::='");
        else if (type == -1)
            throw runtime_error("Invalid Nonterminal symbols");
    }
    catch (exception &e)
    {
        cerr << "Exception caught: " << e.what() << endl;
        system("pause");
        system("cls");
        return;
    }

    cout << "您输入的文法G是:" << endl;
    cout << "G[" << G.getS() << "] = ({";
    for (size_t i = 0; i < G.getVN().size(); ++i)
    {
        cout << G.getVN()[i];
        if (i != G.getVN().size() - 1)
            cout << ",";
    }
    cout << "}, {";
    for (size_t i = 0; i < G.getVT().size(); ++i)
    {
        cout << G.getVT()[i];
        if (i != G.getVT().size() - 1)
            cout << ",";
    }
    cout << "}, P, " << G.getS() << ")" << endl;

    cout << "其中,   P为:" << endl;
    for (const string &production : G.getP())
        cout << "\t" << production << endl;

    if (type == -1)
        cout << "非法！" << endl;
    else
        cout << "该文法是Chomsky" << type << "型文法" << endl;
    cout << endl;
    system("pause");
    system("cls");
}

int main()
{
    int flag = 1;
    while (flag)
    {
        cout << "输入 1 进行文法检测，输入 0 退出：" << endl;
        cin >> flag;
        if (flag == 0)
            break;

        runGrammarCheck();
    }
    system("pause");
    return 0;
}