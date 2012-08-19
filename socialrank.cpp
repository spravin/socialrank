//   Copyright 2012 Mangesh Gupte, Pravin Shankar, Jing Li, S Muthukrishnan, Liviu Iftode
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0

//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

// implementation of Mangesh's social agony algorithm

#include <cstdio>
#include <string>
#include <vector>
#include <set>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <iomanip>

#define MAXV 100000

using namespace std;

struct edge { 
    int u, v, weight;

    edge(int _u, int _v) {
        u = _u, v = _v, weight = -1;
    }
    
    edge(int _u, int _v, int _weight) {
        u = _u, v = _v, weight = _weight;
    }
};

// general stuff
int LPvalue; // value for the dual LP
int cycleCount;
int V, E;
vector <edge> edges;
vector <edge> original_edges;
ofstream results, summary;

// stuff for labeling
vector <int> edgesDAG; // edges left over from eulerian subgraph
vector <int> edgesEulerian; // maximal eulerian subgraph
set<int> adjDAG[MAXV]; // these adjacency lists still just keep edge indices
set<int> adjEulerian[MAXV];
bool inEulerian[MAXV];

// stuff for Bellman-Ford
int dist[MAXV];        // vertex -> distance

// stuff for DFS
int oncycle;
set<int> adj[MAXV];
int pre[MAXV];
int state[MAXV]; // 0 undiscovered, 1 visiting, 2 done

void dfs_visit(int u) {
    if (oncycle != -1) 
	return;
    state[u] = 1;
	
    // iterate through neighbors
    set <int> :: iterator iter = adj[u].begin();
    while (iter != adj[u].end() && oncycle == -1) {
	int j = *iter;
	int v = edges[j].v;
	if (state[v] == 0) {
	    pre[v] = j;
	    dfs_visit(v);
	} else if (state[v] == 1) {
	    pre[v] = j;
	    oncycle = v;
	    state[u] = 2;
	    return;
	}
	iter++;
    }
    state[u] = 2;
}


void dfs() {
    oncycle = -1;
    memset(state, 0, sizeof(state));

    for(int i=0;i < V; i++) {
	pre[i] = -1;
    }
    
        
    for (int i = 0; i < V; i++) {
	if (oncycle != -1)
	    break;
	if (state[i] == 0) {
	    dfs_visit(i); // find a way to quit this even earlier
	}
    }
}

vector <int> dfs_find_cycle() {
    bool visited[V]; 

    if (oncycle == -1) {
	return vector <int>();
    }
    int start = oncycle;
    vector <int> cycle; // vector of edge indices
    memset(visited, 0, sizeof(visited));
	
    visited[start] = true;
    int next = pre[start];
    while(!visited[edges[next].u]) {
        cycle.push_back(next);
        int u = edges[next].u;
        visited[u] = true;
        next = pre[u];
    }
    cycle.push_back(next);
    return cycle;
}

void bellman_ford(int *pred) {

    for(int i = 0; i < V; i++) {
        dist[i] = V;
	pred[i] = -1;
    }

    dist[0] = 0;
    
    int i;
    for(i = 0; i < V; i++) {
        bool changed = false;
	//printf("%d ",i);
        for(int j = 0; j < E; j++) {
	
            int u = edges[j].u;
            int v = edges[j].v;
            int w = edges[j].weight;
            if(dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pred[v] = j;
                changed = true;
            }
        }
	
	if(dist[0] < 0) {
	    break;
	}
	
        if(!changed) {
            printf("Bellman Ford done after %d of %d iters\n", i, V);
	    //results << "Bellman-Ford done after " << i << " of " << V << " iters" << endl;
            break;
        }
    }
    printf("Bellman Ford took %d of %d iters\n", i+1, V);
}

vector <int> bf_find_cycle(int *pred) {
    bool visited[V];
    int start = -1;
    int u,v,w;
    int prev;
    int unew;
    
    for(int j = 0; j < E; j++) {
	u = edges[j].u;
	v = edges[j].v;
	w = edges[j].weight;
        if((dist[u] + w < dist[v]) && (pred[u] != -1)) {
	    
	    memset(visited, 0, sizeof(visited));
	    visited[edges[j].u] = true;
	    prev = pred[edges[j].u];
	    
	    while(!visited[edges[prev].u]) {
		//printf(" %d %d %d %d\n",prev,edges[prev].u, edges[prev].v, edges[prev].weight);
		unew = edges[prev].u;
		visited[unew] = true;
		prev = pred[unew];
		// Dead end
		if(prev == -1)
		   break;
	    }

	    if(pred[unew] != -1){
		// Found a cycle
		start = prev;
		break;
	    }
        }
    }

    if(start == -1)
        return vector <int>();

    memset(visited, 0, sizeof(visited));
    vector <int> cycle; // vector of edge indices
    while(!visited[edges[prev].u]) {
        cycle.push_back(prev);
	u = edges[prev].u;
        visited[u] = true;
        prev = pred[u];
    }
    return cycle;
}

void cancel_cycle(const vector <int> &cycle) {
    //printf("Cancelling cycle\n");
    int old_lpvalue = LPvalue;
    
    for(int i = 0; i < cycle.size(); i++) {
        int j = cycle[i];
        //printf("%d %d (weight %d)\n", edges[j].u, edges[j].v, edges[j].weight);
	//results << "cancelling:" << edges[j].u << " " << edges[j].v << " (weight) " << edges[j].weight << endl;
        int u = edges[j].u;
	int v = edges[j].v;
	int w = edges[j].weight;
	
	edges[j].u = v; // flip edges
        edges[j].v = u;
        edges[j].weight = -w;
	
	LPvalue += edges[j].weight; // update LP value
	if (w == -1) {
	    // This is only for DFS
	    adj[u].erase(j);
	} 
    }
//    if(LPvalue % 100 == 0) {
    printf("\nCycle %d: LPvalue = %d ", ++cycleCount,LPvalue);
//    }

    assert(LPvalue > old_lpvalue);
    
    //results << "CYCLE DONE:" << cycleCount << "\t LPvalue =" << LPvalue << endl;
}


void get_max_eulerian_subgraph() {
    LPvalue = 0;
    int pred[V+1]; // vertex -> edge index
    vector <int> cycle;
    
    while(true) {
	dfs();
	cycle = dfs_find_cycle();
	if(cycle.size() == 0)
            break;
        cancel_cycle(cycle);

    }
    
    printf("\nDoing Bellman-Ford on cycle %d\n",cycleCount+1);
    while(true) {
	//results << "LP Value :" << LPvalue << "\n\nDoing Bellman-Ford on cycle " << cycleCount+1 << endl;
	bellman_ford(pred);
        cycle = bf_find_cycle(pred);
        if(cycle.size() == 0)
            break;
        cancel_cycle(cycle);
    }
    
    results << "Dual LP Value :" << LPvalue << endl;

    printf("Max Eulerian Subgraph edges: \n");
    results << "Max Eulerian Subgraph edges:" << endl;
    memset(inEulerian, 0, sizeof(inEulerian));

    // Print out the eulerian subgraph
    printf("Edges\n");
    for(int j = 0; j < E; j++) {
        if(edges[j].weight == 1) {
	    int u = edges[j].u;
	    int v = edges[j].v;
	    edgesEulerian.push_back(j);
	    adjEulerian[u].insert(j);
	    inEulerian[u] = true;
	    inEulerian[v] = true;

            printf("%d %d , ", u, v);
	    results << u << " " << v << endl;
	}
    }
    
    // Print out the DAG
    printf("\n\nDAG edges: \n");
    results << "\n\nDAG edges:" << endl;
    
    for(int j = 0; j < E; j++) {
        if(edges[j].weight == -1) {
	    int u = edges[j].u;
	    int v = edges[j].v;
	    edgesDAG.push_back(j);
	    adjDAG[u].insert(j);
	    inEulerian[u] = false;
	    inEulerian[v] = false;
            printf("%d %d , ", u, v);
	    results << u << " " << v << endl;
	}
    }
}

void labeling(string rankfile) {
    
    int labels[V+1];

    printf("\n\nStarted labeling \n");
    results << "started labeling" << endl;

    for(int i=0;i < V;i++) {
	labels[i] = -1;
    }
    

    // Find the indegrees of all the vertices
    int indegree[V+1]; // node number -> indegree
    memset(indegree, 0, sizeof(indegree));	
    for(int k = 0; k < edgesDAG.size(); k++) { // get indegrees of each node
	indegree[edges[edgesDAG[k]].v]++;
    }

    // Set to 0, the labels of vertices with indegree 0
    // Set to V, labels of all other vertices
    for(int i = 0; i < V; i++) {
	// if(indegree[i] == 0) {
	//     dist[i] = 0;
	// } else {
	//     dist[i] = V;
	// }
	dist[i] = 0;
    }

    // printf("\n\nEdges of the graph\n\n");

    // for(int j = 0; j < E; j++) {
    // 	printf("%d %d %d\n",edges[j].u,edges[j].v,edges[j].weight);
    // }
    
    // Run bellman-ford on this graph
    for(int i = 0; i < V; i++) {
        bool changed = false;
        for(int j = 0; j < E; j++) {
            int u = edges[j].u;
            int v = edges[j].v;
            int w = edges[j].weight;
            if(dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                changed = true;
            }
        }
        if(!changed) {
            printf("Bellman Ford done after %d of %d iters. ", i, V);
	    //results << "Bellman-Ford done after " << i << " of " << V << " iters" << endl;
            break;
        }
    }

    // Find the largest and smallest label among all the vertices.
    // Initialize vertex agony.
    int max_label = 0, min_label = 100000;
    int agony[V+1];
    for(int i = 0; i < V; i++) {
	agony[i] = 0;
    	if(dist[i] > max_label) {
    	    max_label = dist[i];
    	}
	if(dist[i] < min_label) {
    	    min_label = dist[i];
    	}
    }

    for(int i = 0; i < V; i++) {
	dist[i] = max_label - dist[i];
	printf("%d %d %d\t", i, dist[i], agony[i]);
    }
    printf("\n\n");

    ofstream flabel;
    flabel.open(rankfile.c_str());
//
    flabel << "#NodeId Rank Agony" << endl;

    
    int primal_value = 0;
    for(int k = 0; k < original_edges.size(); k++) {
	printf("%d ,",max(0, dist[original_edges[k].u] - dist[original_edges[k].v] + 1));
	primal_value += max(0, dist[original_edges[k].u] - dist[original_edges[k].v] + 1);
	agony[original_edges[k].u] += max(0, dist[original_edges[k].u] - dist[original_edges[k].v] + 1);
    }
    
    // Set the new labels for each of the vertices
    printf("\nVertex labels\n");
    for(int i = 0; i < V; i++) {
	flabel << i << " " << dist[i] << " " << agony[i] << endl;
    }
    flabel << (float)LPvalue/(float)E << " " << LPvalue << " " << primal_value << endl;

    summary << "\t" 
	    << setprecision(4) << (float)LPvalue/(float)E << "\t"
	    << setw(6) << V << "\t"
	    << setw(6) << E << "\t"
	    << setw(6) << max_label - min_label << "\t"
	    << setw(6) << LPvalue << "\t"
	    << setw(6) << primal_value << "\t"
	    << endl;
    printf("\n%7.5f %6d %6d %6d %6d",LPvalue/(float)E, V, E, LPvalue, primal_value);
    flabel.close();
}

void init_graph(string fnodes, string fedges) {
	
    cout << "opening file:" << fnodes << endl;
    
    ifstream fin;
    char buf[50];

    fin.open(fnodes.c_str());
    for(V=0; !fin.eof(); V++){
	do {
	    fin.getline(&buf[0], 256);
	    cout << buf << endl;
	} while(buf[0] == '#');
    }
    V--;
    fin.close();
    
    cout << "opening file:" << fnodes << endl;

    fin.open(fedges.c_str());
    for(E=0; !fin.eof(); E++) {
	int u, v;
	do {
	    fin.getline(&buf[0], 256);
	    cout << buf << endl;
	} while(buf[0] == '#');
        sscanf(buf, "%d%d", &u, &v);

	assert(u>=0 && u<=V && v>=0 && v<=V);
	
        edges.push_back(edge(u, v));
	original_edges.push_back(edge(u, v));
	adj[u].insert(E); 
    }
    E--;
    fin.close();
    
    printf("Done reading file.\n");
    cycleCount = 0;

}

int main(int argc, char *argv[]) {
    results.open("results.txt");
    
    summary.open(argv[1],fstream::app);
    summary << argv[2] << " ";
    cout << " Here ? " << endl;
    
    string nodefile(argv[2]);
    nodefile.append(".nodes");
    cout << "nodefile" << endl;
    
    string edgefile(argv[2]);
    edgefile.append(".edges");
    cout <<"edgefile "<<endl;
    
    string rankfile(argv[2]);
    rankfile.append(".ranks");

    cout << "Basic reading done. " << endl;
    

    init_graph(nodefile, edgefile);
    cout << "Graph initialised" << endl;
    
    get_max_eulerian_subgraph();
    cout << "eulerian subgraph done" << endl;
    

    labeling(rankfile);
    cout <<"Labeling done" << endl;
    
    results.close();
    summary.close();
    
    return 0;
}
