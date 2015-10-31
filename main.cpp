#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <set>
#include <cmath>
#include <map>
#include <sstream>

#ifdef DEBUG_PROGRAM
#define DEBUG(x) cout << (x) << endl;
#else
#define DEBUG(x)
#endif

#define PRINT(x) cout << x << endl;

using namespace std;
typedef pair<bool, int> ChordPair;
typedef vector<ChordPair> ChordVector;

class Network
{
public:
    // Constructor
    // Simulates a network in which the DHT resides
    Network(int S) :
            mS(S)
            {}

    // Add node (peer) to the netork
    void AddNode(int nID)
    {
        mNodes.insert(nID);
    }

    // Checks the peer to peer connection in the network
    // insures that it is valid.
    int Check(int succ)
    {
        set<int>::iterator it = mNodes.lower_bound(succ);
        if (it == mNodes.end())
        {
            return *mNodes.begin();
        }

        return *it;
    }

private:
    int mS;
    set<int> mNodes;
};

class Node
{
public:
    // Constructor
    // Creates a node that simulates a peer in a DHT netowrk.
    Node(int nodeId, int S) :
            mNodeId(nodeId),
            mS(S),
            mKey(-1)
    { };

    // Process Query
    // Takes a input "key" and returns true if the node (peer) has the
    // Hash key in question or false otherwise.
    // Returns the next successor if false.
    bool ProcessQuery(int key, int &successor)
    {
        if (mKey == key)
        {
            successor = -1;
            return true;
        }

        for (auto it = mSuccessors.begin(); it != mSuccessors.end(); ++it)
        {
            if (key < it->first)
            {
                --it;
                successor =  it->second;
                return false;
            }
        }

        successor = mSuccessors.rbegin()->second;
        return false;
    }

    // GetID
    // Returns the node (peer) id.
    int GetId()
    {
        return mNodeId;
    }

    // AddKey
    // inserts a key value pair into the node.
    // In this case the value is not present.
    void AddKey(int key)
    {
        mKey = key;
    }

    // InsertSuccessor
    // Creates a map of <finger id, successor>
    // Simulates the connection between the peers in the DHT
    void InsertSuccessor(int nodeid, int succ)
    {
        mSuccessors.insert(pair<int, int>(nodeid, succ));
    }

private:
    int mS;
    int mNodeId;
    int mKey;
    string mValue;
    map<int, int> mSuccessors;
};


/////////////////////////////////////////////////////////////////////////////////////////
int main(int argn, char *argv[]) {
    // Intial Values
    // S = the max number of nodes in the network 2^n - 1 = S
    // N = the number of current nodes in the network
    // M = the number of key-value pairs to insert in the network
    int S, N, M;

    // Check for argument
    if (argn < 2)
    {
        cout << "Need to provide test case file. (ie ./chorddht testcase1.txt)" << endl;
        return 1;
    }

    string inFileName = argv[1];
    ifstream inputFile(inFileName.c_str(), std::ios::binary);

    // Get Initial vars
    string line;
    getline(inputFile, line);
    S = atoi(line.c_str()) + 1;
    getline(inputFile, line);
    N = atoi(line.c_str());
    getline(inputFile, line);
    M = atoi(line.c_str());

    // Populate Buffer
    getline(inputFile, line);
    Network network(S);
    set<int> tempNodes;
    vector<Node> nodes(S, Node(S, -1));

    int newpos = 0;
    int oldpos = 0;
    for (int i = 0; i < N; ++i)
    {
        newpos = line.find(",", oldpos);
        int id = atoi(line.substr(oldpos, newpos-oldpos).c_str());
        oldpos = newpos+1;

        network.AddNode(id);
        tempNodes.insert(id);
        nodes[id] = Node(id, S);
    }

    // Create Fingers/Successors
    for (const int &id : tempNodes)
    {
        for (int j = 0; pow(2, j) < S; ++j)
        {
            int nid = (int) (id + pow(2, j))%S;
            int succ = network.Check(nid);

            nodes[id].InsertSuccessor(nid, succ);
        }
    }

    // Insert the Key Value
    getline(inputFile, line);
    oldpos = 0;
    for (int i = 0; i < M; ++i)
    {
//        int key = atoi(&line[i*2]);
        newpos = line.find(",", oldpos);
        int key = atoi(line.substr(oldpos, newpos-oldpos).c_str());
        oldpos = newpos+1;

        nodes[network.Check(key)].AddKey(key);
    }

    // Process the queries.
    getline(inputFile, line);
    string solution;

    oldpos = 0;
    newpos = line.find(",", oldpos);
    int queryKey = atoi(line.substr(oldpos, newpos-oldpos).c_str());
    int queryID = atoi(line.substr(newpos+1, -1).c_str());
    while (queryKey >= 0)
    {
        int steps = 0;
        while(!nodes[queryID].ProcessQuery(queryKey, queryID))
        {
            ++steps;
        }

        PRINT("THE NUMBER OF STEPS FOR QUERY: " << steps)
        string stepnumber = static_cast<ostringstream*>( &(ostringstream() << steps) )->str();
        solution = solution + stepnumber + "\n";

        getline(inputFile, line);
        oldpos = 0;
        newpos = line.find(",", oldpos);
        queryKey = atoi(line.substr(oldpos, newpos-oldpos).c_str());
        queryID = atoi(line.substr(newpos+1, -1).c_str());
    }

    inputFile.close();

    //Process outputfile
    ofstream outfile(inFileName.substr(0, inFileName.find(".")) + ".out");
    outfile << solution;
    outfile.close();

    return 0;
}