#include <iostream>
#include <map>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <iterator>
#include <string>
#include <stack>
#include <algorithm>

using namespace std;

typedef map < int, vector < int > > graph;
typedef pair< int, vector < int > > mapPair;
typedef map < int, bool > bitmap;
typedef map < int, int > intmap;
typedef pair< vector< int >, vector< int > > vecpair;


// GLOBAL VARIABLES
int s = NULL; // keeps track of the vertex from which the last DFS call was invoked
int t = 0;  // keeps track of the number of vertices that have been fully explored


void print_graph(graph &v)
{
    graph::iterator J;
    vector<int>::iterator I;
    for (J=v.begin(); J!=v.end(); ++J)
    {
        cout << "Key: " << (*J).first << "::";
        for (I=(*J).second.begin();I!=(*J).second.end(); ++I)
        {
            cout << " " << *I;
        }
        cout << endl;
    }
}

void print_intmap(intmap x)
{
    for (intmap::iterator it = x.begin(); it!=x.end(); ++it)
    {
        cout << it->first << " : " << it->second << endl;
    }
}

bitmap initializeBitMap(graph &G)
{
    bitmap initBitMap;
    graph::iterator it;
    for (it=G.begin(); it!=G.end(); ++it)
    {
        initBitMap[it->first] = false;
    }
    return initBitMap;

}

intmap initializeIntMap(graph &G, int initVal)
{
    intmap initIntMap;
    graph::iterator it;
    for (it=G.begin(); it!=G.end(); ++it)
    {
        initIntMap[it->first] = initVal;
    }
    return initIntMap;
}



graph loadGraphFromText(string filePath)
{
    graph G;

    ifstream infile(filePath.c_str());

    if (!infile) {
        cout << "There was a problem opening file "
             << " for reading."
             << endl;
        return G;
    }
    cout << "Opened for reading.  Reading contents...." << endl;

    int val=0;
    vector<int> intVec;
    string s = "";
    while (infile.good()) {

        getline(infile,s);
        //cout << s << endl;

        istringstream iss(s);
        int count = 0;
        int keyVal = 0;
        while (iss>>val)
        {
            if (count==0 && G.find(val)==G.end())
            {
                G.insert( mapPair(val,intVec) );
                keyVal = val;
            }
            else if (count==0)
            {
                keyVal = val;
            }
            else if (count>0)
            {
                G[keyVal].push_back(val);
            }
            //cout << val << endl;
            ++count;
        }

    }

    //print_graph(G);
    return G;
}


graph reverseGraph(graph &G)
{
    graph Grev;

    for (graph::iterator git=G.begin(); git!=G.end(); ++git)
    {
        for (vector<int>::iterator vit=(*git).second.begin(); vit!=(*git).second.end(); ++vit)
        {
            Grev[*vit].push_back((*git).first);
        }
    }

    return Grev;
}




void DFS(graph &G, int startVertex, bitmap &exploredMap, intmap &leaderMap, intmap &finishMap)
{
    exploredMap[startVertex] = true;
    leaderMap[startVertex] = s;
    stack<int> Z;
    Z.push(startVertex); //cout<< "RESET: " << startVertex << endl;
    while (!Z.empty())
    {
        int Ztop = Z.top(); //cout<< "Current Top: " << Ztop << endl;
        int nUnexploredVertices = 0;
        for (vector<int>::reverse_iterator it=G[Ztop].rbegin(); it!= G[Ztop].rend(); ++it)
        {
            if (!exploredMap[*it])
            {
                exploredMap[*it] = true; //cout<<"MARKED: " << *it << endl;
                Z.push(*it); //cout<< "Adding: " << *it << endl;
                ++nUnexploredVertices;
            }

        }
        if (nUnexploredVertices==0)
        {
            ++t;
            finishMap[Ztop] = t; //cout<< "FINISHED: " << "f(" << Ztop << ")= " << t << endl;
            leaderMap[Ztop] = s; //cout<< "Ztop: " << Ztop << " Assigned Leader: " << s << endl;
            Z.pop(); //cout << "REMOVE: " << Ztop << endl;
        }
    }


}


void DFS_Recursive(graph &G, int startVertex, bitmap &exploredMap, intmap &leaderMap, intmap &finishMap)
{
    exploredMap[startVertex] = true;
    leaderMap[startVertex] = s;

    for (vector<int>::iterator it=G[startVertex].begin(); it!= G[startVertex].end(); ++it)
    {
        if (!exploredMap[*it])
        {
            DFS_Recursive(G, *it, exploredMap, leaderMap, finishMap);
        }

    }

    ++t;
    finishMap[startVertex] = t;

}



intmap DFS_LOOP(graph &G,intmap loopIndex, int mode)
{
    // initialize global variables
    t = 0;
    s = NULL;

    // initialze exploredMap, leaderMap, finishMap
    bitmap exploredMap = initializeBitMap(G);
    intmap leaderMap = initializeIntMap(G,-1);
    intmap finishMap = initializeIntMap(G,-1);

    //print_intmap(loopIndex);
    intmap::reverse_iterator rit;
    for (rit=loopIndex.rbegin(); rit!=loopIndex.rend(); ++rit)
    {
        if (!exploredMap[rit->second])
        {
            s = rit->second;
            DFS(G, s, exploredMap,leaderMap,finishMap);
        }

    }

    /*
    // DEBUG OUTPUT
    cout << "ExploredMap" << endl;
    for(bitmap::iterator it=exploredMap.begin();it!=exploredMap.end();++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    cout << "FinishMap" << endl;
    for(intmap::iterator it=finishMap.begin();it!=finishMap.end();++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    cout << "LeaderMap" << endl;
    for(intmap::iterator it=leaderMap.begin();it!=leaderMap.end();++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    */

    if (mode==1)
    {
        return finishMap;
    }
    else
    {
        return leaderMap;
    }
}

vector<int> countSCC(intmap leaderMap)
{
    intmap leaderCount;
    for (intmap::iterator it=leaderMap.begin(); it!=leaderMap.end(); ++it)
    {
        intmap::iterator idx = leaderCount.find(it->second);
        if (idx==leaderCount.end())
        {
            leaderCount[it->second]=1;
        }
        else
        {
            ++(idx->second);
        }
    }

    vector<int> sccCount;
    for (intmap::iterator it=leaderCount.begin(); it!=leaderCount.end();++it)
    {
        //cout << it->first << ": " << it->second << endl;
        sccCount.push_back(it->second);
    }

    return sccCount;
}


bool sortFunc (int i,int j) { return (i>j); }

int main()
{

    graph G = loadGraphFromText("C:\\Documents and Settings\\c\\My Documents\\Downloads\\SCC_test.txt");
    cout << "G Size: " << G.size() << endl;
    //print_graph(G);

    graph Grev = reverseGraph(G);
    cout << "Grev Size: " << Grev.size() << endl;
    //print_graph(Grev);

    // create loop index
    intmap loopIndex1;
    for (graph::iterator it=Grev.begin(); it!=Grev.end();++it)
    {
       loopIndex1[it->first] = it->first;
    }
    for (graph::iterator it=G.begin(); it!=G.end();++it)
    {
        if (loopIndex1.find(it->first)==loopIndex1.end())
        {
            loopIndex1[it->first] = it->first;
        }

    }
    // Run DFS-loop subroutine on G_rev to get finishing times mode=1
    cout << "FIRST DFS LOOP " << endl;
    intmap finalFinishMap = DFS_LOOP(Grev,loopIndex1,1);

    // create loop index
    // RUN DFS loops on subroutine in decreasing order of f(v) to assign a leader to each vertex v
    intmap loopIndex2;
    int maxKey2 = -1;
    for (intmap::iterator it2=finalFinishMap.begin();it2!=finalFinishMap.end();++it2)
    {
        loopIndex2[it2->second] = it2->first;
        maxKey2 = it2->second>maxKey2?it2->second:maxKey2;
    }
    cout << "SECOND DFS LOOP " << endl;
    intmap finalLeaderMap = DFS_LOOP(G,loopIndex2,0);


    //cout << "LeaderMap" << endl;
    //print_intmap(finalLeaderMap);


    vector<int> result = countSCC(finalLeaderMap);
    sort(result.begin(),result.end(),sortFunc);

    int N=0;
    for (vector<int>::iterator it3=result.begin(); it3!=result.end();++it3)
    {
        cout<< *it3 << endl;
        ++N;
        if (N==5)
        {
            break;
        }
    }

    /*
    // DFS() TEST CASE
    bitmap exploredMap = initializeBitMap(G);
    intmap leaderMap = initializeIntMap(G,-1);
    intmap finishMap = initializeIntMap(G,-1);
    t = 0;
    s = 9;
    DFS(Grev, 9, exploredMap,leaderMap,finishMap);
    cout << "ExploredMap" << endl;
    for(bitmap::iterator it=exploredMap.begin();it!=exploredMap.end();++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    cout << "FinishMap" << endl;
    for(intmap::iterator it=finishMap.begin();it!=finishMap.end();++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    cout << "LeaderMap" << endl;
    for(intmap::iterator it=leaderMap.begin();it!=leaderMap.end();++it)
    {
        cout << it->first << ":" << it->second << endl;
    }
    */


    return 0;
}
