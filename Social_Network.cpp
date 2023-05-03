#include<iostream>
#include<string>
#include<vector>
#include<unordered_map>
#include<fstream>
#include<sstream>
#include<stack>
#include<queue>

using namespace std;

#define ll long long

//---------------------------------NODE DEFINITION---------------------------------------------
class node{
public:
    int nodeid;
    string username;
    string name;
    int age;
    node(){
        nodeid =0;
        username = "";
        name = "";
        age=0;
    }
    node(int id,string un,string n,int a){
        nodeid = id;
        username = un;
        name = n;
        age = a;
    }
};
// struct nodeHash {
//     size_t operator()(const node& n) const {
//         size_t h1 = hash<string>{}(n.name);
//         size_t h2 = hash<int>{}(n.age);
//         size_t h3 = hash<string>{}(n.username);
//         return h1 ^ (h2 << 1) ^ (h3 << 2);
//     }
// };
//---------------------------------NODE DEFINITION---------------------------------------------


unordered_map<int,vector<int>> adj;
unordered_map<string,node> username_node;
unordered_map<int,node> id_node;
int below30= 0;



//----------------------------------------FETCH DATA--------------------------------------------
ifstream nodein("data.txt");
ifstream edgesin("edges.txt");

int fetch_nodes(ifstream& in) {
    vector<pair<int,node>> nodes;
    string line;
    int count = 0;
    while (getline(in, line)) {
        stringstream ss(line);
        string name;
        string username;
        int nodeid;
        int age;
        if (ss >>nodeid >> username >> name >> age) {
            node a(nodeid,username,name,age);
            nodes.push_back({nodeid,a});
            count++;
            if(age <30){
                below30++;
            }
        }
    }

    for(auto it : nodes){
        int id = it.first;
        string person = it.second.username;
        node n = it.second;
        id_node[id] = n;
        username_node[person] = n;
    }

    return count;
    
}


void fetch_edges(ifstream& in){
    string line;
    while (getline(in, line)) {
        stringstream ss(line);
        int id1;
        int id2;
        if (ss >> id1 >> id2) {
            adj[id1].push_back(id2);
        }
    }
}
//----------------------------------------FETCH DATA--------------------------------------------

//-----------------------------------------PRINT ALL PEOPLE----------------------------------------------
void printpeople(){
    for(auto it : username_node){
        cout<<it.first<<" - ";
        cout<<"details --> "<<it.second.nodeid<<" "<<it.second.name<<" "<<it.second.age<<endl;
    }
    cout<<endl;
}
//---------------------------------------PRINT ALL PEOPLE-------------------------------------------------------

//---------------------------------------PRINT ALL PEOPLE WHO THEY FOLLOW-----------------------------------
void printfriends(){
    for(auto it : adj){
        cout<<id_node[it.first].username<<" --> ";
        for(auto neighbour : it.second){
            cout<<id_node[neighbour].username<<" ";
        }
        cout<<endl;
    }
}
//---------------------------------------PRINT ALL PEOPLE WHO THEY FOLLOW-----------------------------------

//-----------------------------------------DSU------------------------------------------------------ 
class dset{
public:
    vector<int> rank;
    vector<int> parent;
    vector<int> size;

    dset(){}
    dset(int n){
        rank.resize(n+1,0);
        size.resize(n+1,1);
        parent.resize(n+1);
        for(int i =0;i<=n;i++){
            parent[i] = i;
        }
    }

    int findparent(int node){
        if(parent[node] == node){
            return node;
        }
        return parent[node] = findparent(parent[node]);
    }

    void unionbyrank(int u,int v){
        int parentu = findparent(u);
        int parentv = findparent(v);
        if(parentu == parentu){
            return;
        }
        else{
            if(rank[parentu]<rank[parentv]){
                parent[parentu] = parentv;
            }
            else if(rank[parentv]<rank[parentu]){
                parent[parentv] = parentu;
            }
            else{
                parent[parentv] = parentu;
                rank[parentu]++;
            } 
        }
    }

    void unionbysize(int u,int v){
        int parentu = findparent(u);
        int parentv = findparent(v);
        if(parentu == parentu){
            return;
        }
        else{
            if(size[parentu] < size[parentv]){
                parent[parentu] = parentv;
                size[parentv] += size[parentu];
            }
            else{
                parent[parentv] = parentu;
                size[parentu] += size[parentv];
            }
        }
    }

};
//---------------------------------------------DSU---------------------------------------------------------

//--------------------------------------------NUMBER OF FRIEND CIRCLES---------------------------------
void dfs(int node, unordered_map<int,vector<int>>& adjList, vector<bool>& visited, stack<int>& s) {
    visited[node] = true;
    for(auto neighbour : adjList[node]) {
        if(!visited[neighbour]) {
            dfs(neighbour, adjList, visited, s);
        }
    }
    s.push(node);
}

void dfs2(int node, unordered_map<int,vector<int>>& adjList, vector<bool>& visited) {
    visited[node] = true;
    for(auto neighbour : adjList[node]) {
        if(!visited[neighbour]) {
            dfs2(neighbour, adjList, visited);
        }
    }
}

int countSCC(unordered_map<int,vector<int>>& adjList) {
    int n = adjList.size();
    vector<bool> visited(n+1, false);
    stack<int> s;
    for(int i=1; i<=n; i++) {
        if(!visited[i]) {
            dfs(i, adjList, visited, s);
        }
    }
    unordered_map<int,vector<int>> transposeList;
    for(auto node : adjList) {
        for(auto neighbour : node.second) {
            transposeList[neighbour].push_back(node.first);
        }
    }
    visited.assign(n+1, false);
    int count = 0;
    while(!s.empty()) {
        int node = s.top();
        s.pop();
        if(!visited[node]) {
            dfs2(node, transposeList, visited);
            count++;
        }
    }
    return count;
}
//--------------------------------------------NUMBER OF FRIEND CIRCLES---------------------------------


//-----------------------------------------------NUMBER OF MEDIATORS-------------------------------------------

void dfs1(int node, int parent, int& time, unordered_map<int, vector<int>>& adj, vector<int>& disc, vector<int>& low, vector<bool>& articulation_points) {
    disc[node] = low[node] = time++;
    int child_count = 0;
    for (int i = 0; i < adj[node].size(); i++) {
        int child = adj[node][i];
        if (disc[child] == -1) {
            child_count++;
            dfs1(child, node, time, adj, disc, low, articulation_points);
            low[node] = min(low[node], low[child]);
            if (parent == -1 && child_count > 1)
                articulation_points[node] = true;
            if (parent != -1 && low[child] >= disc[node])
                articulation_points[node] = true;
        }
        else if (child != parent)
            low[node] = min(low[node], disc[child]);
    }
}
int countArticulationPoints(unordered_map<int, vector<int>>& adj) {
    int n = adj.size();
    vector<int> disc(n, -1), low(n, -1);
    vector<bool> articulation_points(n, false);
    int time = 0;
    for (int i = 0; i < n; i++) {
        if (disc[i] == -1) {
            dfs1(i, -1, time, adj, disc, low, articulation_points);
        }
    }
    int count = 0;
    for (int i = 0; i < n; i++) {
        if (articulation_points[i])
            count++;
    }
    return count;
}
//--------------------------------------------NUMBER OF MEDIATORS----------------------------------------------

//--------------------------------------TOP INFLUENCERS-----------------------------------------
vector<pair<int,int>> top_k_incoming_nodes(unordered_map<int,vector<int>>& adj, int k) {
    vector<int> incoming_count(adj.size()+1, 0);
    for(auto& it : adj) {
        for(auto& neighbor: it.second) {
            incoming_count[neighbor]++;
        }
    }
    priority_queue<pair<int,int>> pq;
    for(int i=1; i<incoming_count.size(); i++) {
        pq.push(make_pair(incoming_count[i], i));
    }

    vector<pair<int,int>> top_k;
    while(!pq.empty() && top_k.size()<k) {
        top_k.push_back({pq.top().second,pq.top().first});
        pq.pop();
    }

    return top_k;
}
//--------------------------------------TOP INFLUENCERS-----------------------------------------



int main(){
    int v = fetch_nodes(nodein);
    fetch_edges(edgesin);

    // cout<<"done"<<endl;
    printfriends();
    cout<<endl;
    // printpeople();
    cout<<"Number of friend circles : "<<countSCC(adj)<<endl;
    cout<<endl;
    cout<<"Number of mediators : "<<countArticulationPoints(adj)<<endl;
    cout<<endl;
    // // cout<<"Number of people below 30 : "<<below30<<endl;

    vector<pair<int,int>> top10 = top_k_incoming_nodes(adj,10);
    cout<<"Top 10 Influencers : ";
    for(int i = 0 ;i<top10.size();i++){
        cout<<id_node[top10[i].first].username<<", ";
    }
    cout<<endl;
    
    return 0;
}   