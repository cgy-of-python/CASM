#include <iostream>
#include <cstring>
#include <string>
#include <cstdio>
#include <map>
#include <queue>
#include <stack>
#include <cstdlib>
#include <list>
#include "hint.hpp"

using namespace std;
bool DEBUG = false;
map<string, list<string>> codes;
map<string, HyperInt> intVar;
map<string, string> stringVar;
map<string, bool> boolVar;
map<string, double> doubleVar;
map<string, float> floatVar;
map<string, unsigned char> charVar;
stack<HyperInt> intData;
stack<string> stringData;
stack<bool> boolData;
stack<double> doubleData;
stack<float> floatData;
stack<unsigned char> charData;
int nowType;//0:int,1:string,2:bool,3:double,4:float,5:char

struct Lines {
    string op;
    string params[25];

    Lines() {
        op = "";
        memset(params, 0, sizeof(params));
        strData = "";
    }

    int len = -1;
    string strData;
    /* ABOUT LEN:
     *  -1: AN EMPTY LINE;
     *  0:OPERATOR ONLY;
     *  N:OPERATOR WITH N PARAMETERS.
     */
};

HyperInt str2Hyper(const string &str) {
    HyperInt data = 0;
    for (char i: str) {
        data = data * 10;
        data = data + i - '0';
    }
    return data;
}

Lines analysisLine(const string &line);

void knifeCode();

void runCode(const string &name);

void setType(const string &type) {
    if (type == "INT") {
        nowType = 0;
    } else if (type == "STR") {
        nowType = 1;
    } else if (type == "BOOL") {
        nowType = 2;
    } else if (type == "DOUBLE") {
        nowType = 3;
    } else if (type == "FLOAT") {
        nowType = 4;
    } else if (type == "CHAR") {
        nowType = 5;
    } else {
        cout << "ERR:UnKnow Type.";
        exit(1);
    }
}

void push(Lines &line) {
    try {
        if (nowType == 0) {
            HyperInt data = 0;
            for (char i: line.params[0]) {
                data = data * 10;
                data = data + i - '0';
            }
            intData.push(data);

        } else if (nowType == 1) {
            stringData.push(line.strData);
        } else if (nowType == 2) {
            boolData.push(line.params[0] != "FALSE");
        } else if (nowType == 3) {
            doubleData.push(stod(line.params[0]));//stod:string to double
        } else if (nowType == 4) {
            floatData.push(stof(line.params[0]));//stof:string to float
        } else {
            charData.push(line.params[0][0]);
        }
    } catch (exception &e) {
        cout << "ERROR: Type error.";
        exit(1);
    }
}

void out() {
    if (nowType == 0) {
        cout << intData.top();
        intData.pop();
    } else if (nowType == 1) {
        cout << stringData.top();
        stringData.pop();
    } else if (nowType == 2) {
        cout << boolData.top();
        boolData.pop();
    } else if (nowType == 3) {
        cout << doubleData.top();
        doubleData.pop();
    } else if (nowType == 4) {
        cout << floatData.top();
        floatData.pop();
    } else {
        cout << charData.top();
        charData.pop();
    }
}

void endLine() {
    cout << endl;
}

void pushComment() {
    charData.push(';');
}

void pop() {
    if (nowType == 0) {
        intData.pop();
    } else if (nowType == 1) {
        stringData.pop();
    } else if (nowType == 2) {
        boolData.pop();
    } else if (nowType == 3) {
        doubleData.pop();
    } else if (nowType == 4) {
        floatData.pop();
    } else {
        charData.pop();
    }
}

void add() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        intData.push(a + b);
    } else if (nowType == 1) {
        string a = stringData.top();
        stringData.pop();
        string b = stringData.top();
        stringData.pop();
        stringData.push(a + b);
    } else if (nowType == 2) {
        bool a = boolData.top();
        boolData.pop();
        bool b = boolData.top();
        boolData.pop();
        boolData.push(a || b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        doubleData.push(a + b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        floatData.push(a + b);
    } else {
        int a = (int) charData.top();
        charData.pop();
        int b = (int) charData.top();
        charData.pop();
        charData.push((char) (a + b));
    }

}

void sub() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        intData.push(a - b);
    } else if (nowType == 1) {
        cout << "TypeError.";
        exit(1);
    } else if (nowType == 2) {
        bool a = boolData.top();
        boolData.pop();
        bool b = boolData.top();
        boolData.pop();
        boolData.push(a ^ b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        doubleData.push(a - b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        floatData.push(a - b);
    } else {
        int a = (int) charData.top();
        charData.pop();
        int b = (int) charData.top();
        charData.pop();
        charData.push((char) (a - b));
    }
}

void mul() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        intData.push(a * b);
    } else if (nowType == 1) {
        cout << "TypeError.";
        exit(1);
    } else if (nowType == 2) {
        bool a = boolData.top();
        boolData.pop();
        bool b = boolData.top();
        boolData.pop();
        boolData.push(a && b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        doubleData.push(a * b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        floatData.push(a * b);
    } else {
        int a = (int) charData.top();
        charData.pop();
        int b = (int) charData.top();
        charData.pop();
        charData.push((char) (a * b));
    }
}

void div() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        intData.push(a / b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        doubleData.push(a / b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        floatData.push(a / b);
    } else {
        cout << "TypeError.";
        exit(1);
    }
}

void mod() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        intData.push(a % b);
    } else {
        cout << "TypeError.";
        exit(1);
    }
}

void casmIn() {
    if (nowType == 0) {
        HyperInt a;
        cin >> a;
        intData.push(a);
    } else if (nowType == 1) {
        string a;
        stringData.push(a);
    } else if (nowType == 2) {
        bool a;
        cin >> a;
        boolData.push(a);
    } else if (nowType == 3) {
        double a;
        cin >> a;
        doubleData.push(a);
    } else if (nowType == 4) {
        float a;
        cin >> a;
        floatData.push(a);
    } else {
        char a;
        cin >> a;
        charData.push(a);
    }
}

void var(const string &varName) {
    if (nowType == 0) {
        if (!intVar.count(varName))intVar[varName] = 0;
    } else if (nowType == 1) {
        if (!stringVar.count(varName))stringVar[varName] = "";
    } else if (nowType == 2) {
        if (!boolVar.count(varName))boolVar[varName] = false;
    } else if (nowType == 3) {
        if (!doubleVar.count(varName))doubleVar[varName] = 0;
    } else if (nowType == 4) {
        if (!floatVar.count(varName))floatVar[varName] = 0;
    } else {
        if (!charVar.count(varName))charVar[varName] = '\0';
    }
}

void newVar(const string &varName) {
    if (nowType == 0) {
        intVar[varName] = 0;
    } else if (nowType == 1) {
        stringVar[varName] = "";
    } else if (nowType == 2) {
        boolVar[varName] = false;
    } else if (nowType == 3) {
        doubleVar[varName] = 0;
    } else if (nowType == 4) {
        floatVar[varName] = 0;
    } else {
        charVar[varName] = '\0';
    }
}

void getvar(const string &varName) {
    if (nowType == 0) {
        intVar[varName] = intData.top();
        intData.pop();
    } else if (nowType == 1) {
        stringVar[varName] = stringData.top();
        stringData.pop();
    } else if (nowType == 2) {
        boolVar[varName] = boolData.top();
        boolData.pop();
    } else if (nowType == 3) {
        doubleVar[varName] = doubleData.top();
        doubleData.pop();
    } else if (nowType == 4) {
        floatVar[varName] = floatData.top();
        floatData.pop();
    } else {
        charVar[varName] = charData.top();
        charData.pop();
    }
}

void pushVar(const string &varName) {
    if (nowType == 0) {
        intData.push(intVar[varName]);
    } else if (nowType == 1) {
        stringData.push(stringVar[varName]);
    } else if (nowType == 2) {
        boolData.push(boolVar[varName]);
    } else if (nowType == 3) {
        doubleData.push(doubleVar[varName]);
    } else if (nowType == 4) {
        floatData.push(floatVar[varName]);
    } else {
        charData.push(charVar[varName]);
    }
}

void ifEqual() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        boolData.push(a == b);
    } else if (nowType == 1) {
        string a = stringData.top();
        stringData.pop();
        string b = stringData.top();
        stringData.pop();
        boolData.push(a == b);
    } else if (nowType == 2) {
        bool a = boolData.top();
        boolData.pop();
        bool b = boolData.top();
        boolData.pop();
        boolData.push(a == b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        boolData.push(a == b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        boolData.push(a == b);
    } else {
        unsigned char a = charData.top();
        charData.pop();
        unsigned char b = charData.top();
        charData.pop();
        boolData.push(a == b);
    }
}

void ifBig() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        boolData.push(a > b);
    } else if (nowType == 1) {
        string a = stringData.top();
        stringData.pop();
        string b = stringData.top();
        stringData.pop();
        boolData.push(a > b);
    } else if (nowType == 2) {
        bool a = boolData.top();
        boolData.pop();
        bool b = boolData.top();
        boolData.pop();
        boolData.push(a > b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        boolData.push(a > b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        boolData.push(a > b);
    } else {
        unsigned char a = charData.top();
        charData.pop();
        unsigned char b = charData.top();
        charData.pop();
        boolData.push(a > b);
    }
}

void ifSmall() {
    if (nowType == 0) {
        HyperInt a = intData.top();
        intData.pop();
        HyperInt b = intData.top();
        intData.pop();
        boolData.push(a < b);
    } else if (nowType == 1) {
        string a = stringData.top();
        stringData.pop();
        string b = stringData.top();
        stringData.pop();
        boolData.push(a < b);
    } else if (nowType == 2) {
        bool a = boolData.top();
        boolData.pop();
        bool b = boolData.top();
        boolData.pop();
        boolData.push(a < b);
    } else if (nowType == 3) {
        double a = doubleData.top();
        doubleData.pop();
        double b = doubleData.top();
        doubleData.pop();
        boolData.push(a < b);
    } else if (nowType == 4) {
        float a = floatData.top();
        floatData.pop();
        float b = floatData.top();
        floatData.pop();
        boolData.push(a < b);
    } else {
        unsigned char a = charData.top();
        charData.pop();
        unsigned char b = charData.top();
        charData.pop();
        boolData.push(a < b);
    }
}

char casmIf(const string &code) {
    if (code == "RET" && boolData.top()) {
        boolData.pop();
        return 'b';
    }
    if (boolData.top()) {
        boolData.pop();
        runCode(code);
    }
    return '\0';
}


void casmRand(const string &num1, const string &num2) {
    long long int data1 = 0;
    for (char i: num1) {
        data1 = data1 * 10;
        data1 = data1 + i - '0';
    }
    long long int data2 = 0;
    for (char i: num2) {
        data2 = data2 * 10;
        data2 = data2 + i - '0';
    }
    intData.push((HyperInt) rand() % data2 + data1);
}

void clear() {
    if (nowType == 0) {
        while (!intData.empty()) {
            intData.pop();
        }
    } else if (nowType == 1) {
        while (!stringData.empty()) {
            stringData.pop();
        }
    } else if (nowType == 2) {
        while (!boolData.empty()) {
            boolData.pop();
        }
    } else if (nowType == 3) {
        while (!doubleData.empty()) {
            doubleData.pop();
        }
    } else if (nowType == 4) {
        while (!floatData.empty()) {
            floatData.pop();
        }
    } else {
        while (!charData.empty()) {
            charData.pop();
        }
    }
}

void casmNot() {
    bool x = !boolData.top();
    boolData.pop();
    boolData.push(x);
}

void casmFor(const string &varName, const string &funcName) {
    var(varName);
    HyperInt i = intData.top(); intData.pop();
    HyperInt end = intData.top(); intData.pop();
    for (; i < end; i++) {
        intVar[varName]++;
        runCode(funcName);
    }
    newVar(varName);
}

int main(int argc, char *argv[]) {
    srand(time(nullptr));
    if (argc == 1) {
        cout << "Please input the file name" << endl;
        return 0;
    } else if (argc == 3) {
        const char *deb = argv[2];
        if (strcmp(deb, "--debug") == 0) {
            DEBUG = true;
        }
    }
    const char *name = argv[1];
    freopen(name, "r", stdin);
    knifeCode();
    fclose(stdin);
    freopen("CON", "r", stdin);
    cin.clear();
    setType("INT");
    runCode("MAIN");
    return 0;
}

char upper(const char &i) {
    char x = i;
    if ('a' <= i && i <= 'z') x -= 32;
    return x;
}

Lines analysisLine(const string &line) {
    /* EACH LINE IN CASM(EXCEPT COMMENT) HAS ONLY A OPERATOR AND MANY
         * PARAMETER. THE ONLY THINGS THAT IT NEEDS TO DO IS RECORD THEM
         * AND RUN THEM.
         */
    Lines res;
    bool flag = false;
    for (char i: line) {
        if (i == ' ') {
            if (res.len == -1 && (!flag)) continue;
            res.len++;
            if (res.len != 0)
                res.strData += i;
            continue;
        } else {
            if (res.len == -1) flag = true;
        }
        if (i == ';') break;
        if (res.len == -1) res.op += upper(i);
        else res.params[res.len] += i, res.strData += i;
    }
    res.len++; /* NO SPACE FOLLOWS LAST ARG */
    return res;
}

void knifeCode() {
    string code;
    string name;
    while (getline(cin, code, '\n')) {
        Lines l = analysisLine(code);
        if (l.op == "FUNC") {
            if (DEBUG) {
                cout << "DEBUG:FUNC " << l.params[0] << " created." << endl;
            }
            name = l.params[0];
            list<string> lines;
            codes.insert(pair<string, list<string>>(name, lines));
        } else {
            codes[name].push_back(code);
        }
    }
}

void runCode(const string &name) {
    int i = 0;
    for (auto &nowLine: codes[name]) {
        i++;
        if (DEBUG) {
            cout << "DEBUG:in function " << name << ",line " << i << ":\n"
                 << "\tLINE: " << nowLine << endl;
            cout << "\tSTACK TOP:";
            if (!intData.empty())cout << " INT->" << intData.top();
            if (!stringData.empty())cout << " STR->" << stringData.top();
            if (!floatData.empty())cout << " FLOAT->" << floatData.top();
            if (!boolData.empty())cout << " BOOL->" << boolData.top();
            if (!doubleData.empty())cout << " DOUBLE->" << doubleData.top();
            if (!charData.empty())cout << " CHAR->" << charData.top();
            cout << endl;
            while (true) {
                string x;
                cout << "DEBUG:input a var name(pass ; to quit):)";
                cin >> x;
                if (x == ";") break;
                cout << "INT->" << intVar[x] << ";STR->" << stringVar[x]
                     << ";FLOAT->" << floatVar[x] << ";BOOL->" << boolVar[x] << ";DOUBLE->"
                     << doubleVar[x] << ";CHAR->" << charVar[x] << endl;
            }
        }
        Lines line = analysisLine(nowLine);
        if (line.op == "TYPE") {
            setType(line.params[0]);
        } else if (line.op == "PUSH") {
            push(line);
        } else if (line.op == "EXIT") {
            exit(0);
        } else if (line.op == "OUT") {
            out();
        } else if (line.op == "ENDL") {
            endLine();
        } else if (line.op == "PUSHCOMMENT") {
            pushComment();
        } else if (line.op == "POP") {
            pop();
        } else if (line.op == "ADD") {
            add();
        } else if (line.op == "SUB") {
            sub();
        } else if (line.op == "MUL") {
            mul();
        } else if (line.op == "DIV") {
            div();
        } else if (line.op == "IN") {
            casmIn();
        } else if (line.op == "MOD") {
            mod();
        } else if (line.op == "VAR") {
            var(line.params[0]);
        } else if (line.op == "GETVAR") {
            getvar(line.params[0]);
        } else if (line.op == "PUSHVAR") {
            pushVar(line.params[0]);
        } else if (line.op == "IFE") {
            ifEqual();
        } else if (line.op == "IFB") {
            ifBig();
        } else if (line.op == "IFS") {
            ifSmall();
        } else if (line.op == "GOTO") {
            runCode(line.params[0]);
        } else if (line.op == "IF") {
            if (casmIf(line.params[0]) == 'b')break;
        } else if (line.op == "RAND") {
            casmRand(line.params[0], line.params[1]);
        } else if (line.op == "CLEAR") {
            clear();
        } else if (line.op == "NOT") {
            casmNot();
        } else if (line.op == "GTSTR") {
            if (nowType != 2) {
                cout << "Error!";
                exit(1);
            }
            runCode(stringVar[line.params[0]]);
        } else if (line.op == "NEWVAR") {
            newVar(line.params[0]);
        } else if (line.op == "FOR") {
            casmFor(line.params[0], line.params[1]);
        }
        if (DEBUG) {
            cout << "LINE COMPLETED." << endl;
        }
    }
}
