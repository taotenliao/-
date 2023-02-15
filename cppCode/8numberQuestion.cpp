#include <iostream>
#include <string>
#include <math.h>
#include <windows.h>
#include <vector>
#include <algorithm>
#include<map>
#include <queue>
#include <fstream>
#include <random>
#include <ctime>

//记录时间与将搜索结果写入文件的宏
#define CountTime(X,freq,head,tail,file1,file2,func) QueryPerformanceFrequency((LARGE_INTEGER*)&freq);\
                                         QueryPerformanceCounter((LARGE_INTEGER*)&head);\
                                         X;\
                                         QueryPerformanceCounter((LARGE_INTEGER*)&tail);\
                                         cout<<#func<<" use Time :"<<1000.0*(tail-head)/freq<<" ms"<<endl;\
                                         fp.time = 1000.0*(tail-head)/freq;\
                                         fp.node = Allpath.size();\
                                         record(Allpath, file1);\
                                         recordSearch(fp, file2);\
                                         memoryMap.clear();\
                                         fp.clear();\
                                         Allpath.clear();\
                                         PointNumber = 0


using namespace std;

int row, column;//行，列
int PointNumber = 0;//为节点分配编号

struct x_y
{
    int x;
    int y;
};

inline void swapString(string&, int&, int);
class Node
{
public:
    string status;//数字记录
    int value;//节点值
    int depth;//深度
    int whiteDot;//可移动的空白数字位置
    int father;//父节点
    int number;//节点编号
    vector<char> moveStrategy;//可移动的方向向量
    Node() {};
    Node(string initStatus,int initValue, int initDepth, int fatherNumber):status(initStatus), value(initValue), depth(initDepth),father(fatherNumber) {
        number = ++PointNumber;
    };
    void set(string setStatus, int setValue, int setDepth) { status = setStatus, value = setValue, depth = setDepth; };
    //评估节点可移动方向
    static void strategyEvaluation(Node& node) {
        node.whiteDot = node.status.find("*");
        if (node.whiteDot % column != column - 1) { node.moveStrategy.push_back('r');}
        if (node.whiteDot > row) { node.moveStrategy.push_back('d');}
        if (node.whiteDot % column != 0){node.moveStrategy.push_back('l');}
        if (node.whiteDot < row * (column - 1)) { node.moveStrategy.push_back('u');}
    }
    //按照评估结果进行移动
    static string nodeMove(Node& node) {
        string back = "";
        if (node.moveStrategy.empty()) { return back; }
        char op = node.moveStrategy.back();
        node.moveStrategy.pop_back();
        back = node.status;
        switch (op)
        {
        case 'r': swapString(back, node.whiteDot, node.whiteDot + 1);
            break;
        case 'd': swapString(back, node.whiteDot, node.whiteDot - row);
            break; 
        case 'l': swapString(back, node.whiteDot, node.whiteDot - 1);
            break;
        case 'u': swapString(back, node.whiteDot, node.whiteDot + row);
            break; 
        default:
            break;
        }
        return back;
    }
    bool operator<(const Node& node2) const {
        return this->value < node2.value;
    }
    bool operator>(const Node& node2) const {
        return this->value > node2.value;
    }
};
class findPost {
public:
    double time;
    int node;
    int layer;
    findPost():time(-1),node(-1),layer(-1) {};
    void clear() {
        time = node = layer = -1;
    }
};
findPost fp;
// 控制台输出路径，由于使用了web前端，因此暂时用不到，如果不使用前端页面，则可以考虑使用它来输出
inline void ShowPath(vector<string> showPath) {
    int size = showPath.size();
    cout << "path =================>" << endl;
    for (int i = 0; i < size; i++)
    {
        cout <<"第 " << i <<" 步" << endl;
        for (int r = 0; r < row; r++)
        {
            for (int c = 0; c < column; c++) {
                cout << showPath[i][r * column + c]<<" ";
            }
            cout << endl;
        }
        cout << "  | \n" << "  | \n" << "  V \n";
    }
    cout << "end =================>" << endl;
}
// 交换字符串中两个字符的位置
inline void swapString(string & source,int&p1, int p2) {
    char temp = source[p1];
    source[p1] = source[p2];
    source[p2] = temp;
}

void record(vector<Node> path,const string file) {
    ofstream write(file);
    int size = path.size();
    for (int i = 0; i < size; i++)
    {
        write << path[i].status << " ";
        write << path[i].value << " ";
        write << path[i].depth << " ";
        write << path[i].number << " ";
        write << path[i].father << " ";
    }
    write.close();
}
void recordSearch(const findPost fp, const string file) {
    ofstream write(file);
    write << fp.layer << " "<<fp.node<<" "<<fp.time<<" ";
    write.close();
}
vector<Node> Allpath;//记录所有搜索的节点
map<string, bool> memoryMap;//记录该数字状态是否已经被搜索过了
map<char, x_y> manhadunMap;//进行曼哈顿距离计算时保存目标状态字符的x、y值
bool findIt;//判断是否找到目标状态
// 判断是否已经出现过该节点
bool checkMap(string& sorce) {
    if (!memoryMap.count(sorce)) {//如果该值没有出现过
        memoryMap.insert(pair<string, bool>(sorce, true));
        return true;
    }
    return false;
}
// 深度优先扩展的递归函数
bool depthExtend(const string& target, const int &MaxDepth,vector<Node> NodeSet,
    void (*opBeforeExtend)(vector<Node>&))//在下次扩展前做的准备
{
    int countTimes=0;
    string nodeStatus;
    while (!NodeSet.empty()) {    
        int nodeDepth = NodeSet.back().depth + 1;//扩展后状态深度加一
        if ((nodeDepth < MaxDepth) && 
            ((nodeStatus = Node::nodeMove(NodeSet.back()))!="")) {//深度不能超过设定且扩展后获得新状态的值
            if (checkMap(nodeStatus)) {//确定新扩展的状态没有出现过
                ++countTimes;
                Node test(nodeStatus, 0, nodeDepth, NodeSet.back().number);
                Node::strategyEvaluation(test);//评估可以的扩展策略
                NodeSet.push_back(test);//评估后入栈
                Allpath.push_back(test);
                if (nodeStatus == target) {
                    fp.layer = nodeDepth;
                    fp.node = memoryMap.size();
                    cout << "\n在第 " << nodeDepth << " 层发现目标" << endl;
                    cout << "尝试扩展  " << countTimes << " 次" << endl;
                    cout << "最终扩展  " << memoryMap.size() << " 次" << endl;
                    return true;
                }
                if (opBeforeExtend != nullptr) { opBeforeExtend(NodeSet); }//扩展之前的准备，策略可能要对其进行处理
            }
        }
        else {
            NodeSet.pop_back();//节点扩展后深度过大或当前节点已经无法继续扩展，应当出栈
        }
    }
    return false;
}
// 启发式搜索的递归函数
bool inspireExtend(const string& target, const int& MaxDepth, priority_queue<Node, vector<Node>, greater<Node>> nodeHeap,
    int (*inspireFunc)(const string&, const string&, int))//启发策略
{
    int countTimes = 0;
    string nodeStatus;
    while (!nodeHeap.empty()) {
        int nodeDepth = nodeHeap.top().depth + 1;//扩展后状态深度加一
        Node move= nodeHeap.top();
        nodeHeap.pop();//推出，因为top（）只能返回 const类型，蓝瘦！！！
        if ((nodeDepth < MaxDepth) &&
            ((nodeStatus = Node::nodeMove(move)) != "")) {//深度不能超过设定且扩展后获得新状态的值
            nodeHeap.push(move);//推回
            if (checkMap(nodeStatus)) {//确定新扩展的状态没有出现过
                ++countTimes;
                int nodeValue = (inspireFunc == nullptr ? 0 : inspireFunc(nodeStatus, target, nodeDepth));//有启发策略则用启发策略计算
                Node test(nodeStatus, nodeValue, nodeDepth, move.number);
                Node::strategyEvaluation(test);//评估可以的扩展策略
                nodeHeap.push(test);//评估后入堆
                Allpath.push_back(test);
                if (nodeStatus == target) {
                    fp.layer = nodeDepth;
                    fp.node = memoryMap.size();
                    cout << "\n在第 " << nodeDepth << " 层发现目标" << endl;
                    cout << "尝试扩展  " << countTimes << " 次" << endl;
                    cout << "最终扩展  " << memoryMap.size() << " 次" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}
//启发策略一 计算每个数字与最终位置的异同
int inspireFunc1(const string& source, const string& target, int nodeDepth) {//价值等于  深度+差异位置
    int size = source.size();
    for (int i = 0; i < size; i++) { if (source[i] != target[i])nodeDepth++; }
    return nodeDepth;
}
//启发策略二 计算每个数字与到最终位置要移动的最短距离
int inspireFunc2(const string& source, const string& target, int nodeDepth) {
    int len = row * column;
    int manhadunReasult = nodeDepth;
    int tempX, tempY;
    for (int i = 0; i < len; i++)
    {
        x_y temp;
        temp.x = i % column;
        temp.y = i / row;
        manhadunMap.insert(pair<char, x_y>(target[i], temp));
    }
    for (int i = 0; i < len; i++)
    {
        tempX = i % column;
        tempY = i / row;
        manhadunReasult += abs(tempX - manhadunMap[source[i]].x) + abs(tempY - manhadunMap[source[i]].y);
    }
    return manhadunReasult;
}
//启发策略
bool inspire(string& source, string& target, int MaxDepth,
    int (*inspireFunc)(const string&, const string&, int))
{
    Node initNode(source, 0, 0,0);
    Node::strategyEvaluation(initNode);//评估可以的扩展策略
    memoryMap.insert(pair<string, bool>(source, true));
    priority_queue<Node, vector<Node>, greater<Node>> nodeHeap;
    nodeHeap.push(initNode);
    Allpath.push_back(initNode);
    return inspireExtend(target, MaxDepth, nodeHeap, inspireFunc);//启发策略
}
//深度优先遍历
bool depthFirst(const string& source,const string& target, const int& MaxDepth) {
    Node initNode(source,0,0,0);
    Node::strategyEvaluation(initNode);//评估可以的扩展策略
    vector<Node> nodeSet{ initNode };
    Allpath.push_back(initNode);
    memoryMap.insert(pair<string, bool>(source, true));
    return depthExtend(target,MaxDepth, nodeSet,nullptr);//递归前不需要处理
}
//随机策略
void opBeforeExtendRandom(vector<Node>& nodeSet) {
    int size = nodeSet.size();
    int randIndex = rand() % size;
    Node randNode = nodeSet[randIndex];
    nodeSet[randIndex] = nodeSet[0];
    nodeSet[0] = randNode;
}
//随机扩展
bool random(const string& source, const string& target, const int& MaxDepth) {
    Node initNode(source, 0, 0, 0);
    Node::strategyEvaluation(initNode);//评估可以的扩展策略
    vector<Node> nodeSet{ initNode };
    Allpath.push_back(initNode);
    memoryMap.insert(pair<string, bool>(source, true));
    srand(time(0));//初始化随机种子
    return depthExtend(target, MaxDepth, nodeSet, opBeforeExtendRandom);//递归前不需要处理
}
//宽度优先遍历
bool widthFirst(const string& source, const string& target, const int& MaxDepth) {
    Node initNode(source, 0, 0, 0);
    vector<Node> nodeSet_1{ initNode }, nodeSet_2;
    memoryMap.insert(pair<string, bool>(source, true));
    int count_Depth = 1;
    string nodeStatus;
    Allpath.push_back(initNode);
    while (count_Depth!= MaxDepth) {
        int size = nodeSet_1.size();
        for (int i = 0; i < size; i++) {
            Node::strategyEvaluation(nodeSet_1[i]);//计算可以的移动方式
            while ((nodeStatus=Node::nodeMove(nodeSet_1[i]))!="")//遍历所有移动方式
            {
                if (checkMap(nodeStatus)) {
                    Node newNode(nodeStatus, 0, count_Depth, nodeSet_1[i].number);
                    nodeSet_2.push_back(newNode);
                    Allpath.push_back(newNode);
                    if (nodeStatus == target) {
                        fp.layer = count_Depth;
                        fp.node = memoryMap.size();
                        cout << "\n在第 " << count_Depth << " 层发现目标" << endl;
                        cout << "扩展  " << memoryMap.size() << " 次" << endl;
                        return true;
                    }
                }
            }
        }
        if (nodeSet_2.empty()) { break; }
        nodeSet_1 = nodeSet_2;
        nodeSet_2.clear();
        ++count_Depth;
    }
    return false;
}

void controller() {
    ifstream readArgs("../softwareEngineeringFront-end/public/8numberArgs.txt");
    ifstream readfile("../softwareEngineeringFront-end/public/8numberFile.txt");
    vector<string> linesArgs, linesFile;
    string line;
    while (getline(readArgs,line)){ linesArgs.push_back(line);}
    readArgs.close();
    while (getline(readfile, line)) { linesFile.push_back(line); }
    readfile.close();
    int layer;
    string source, target;
    long long head, tail, freq;
    row = atoi(linesArgs[0].c_str());
    column = atoi(linesArgs[1].c_str());
    layer = atoi(linesArgs[2].c_str());
    source = linesArgs[3];
    target = linesArgs[4];
    CountTime(inspire(source, target, layer, inspireFunc1), head, tail, freq, linesFile[0], linesFile[1], inspireFunc1);//启发式算法一
    CountTime(inspire(source, target, layer, inspireFunc2), head, tail, freq, linesFile[2], linesFile[3], inspireFunc2);//启发式算法二
    CountTime(widthFirst(source, target, layer), head, tail, freq, linesFile[4], linesFile[5], widthFirst);//宽度优先策略
    CountTime(depthFirst(source, target, layer), head, tail, freq, linesFile[6], linesFile[7], depthFirst);//深度优先策略
    CountTime(random(source, target, layer), head, tail, freq, linesFile[8], linesFile[9], random);//随机扩展策略
}

int main()
{
    controller();
    return 0;
}

/*
143      123
7 6  ===>4 5         1437*6582 ===> 1234*5678        1234*5678  1*3425678   2831647*5    1238*4765
582      678
*/