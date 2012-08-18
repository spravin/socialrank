# Read a file from which take the username. Take these usernames and generate
# the induced subgraph from these vertices from the full graph.

import sys
import math
import string

def generate_subgraph(num, in_nodes, in_edges, out_nodefilename, out_edgefilename):
    nodes = dict()
    foutnodes = open(out_nodefilename, "w")
    for x in range(num):
        n = in_nodes[x]
#        print n
        arr = n.strip().split()
        nodes[arr[0]] = x
        print >>foutnodes, x, n,
    foutnodes.close()
    
    foutedges = open(out_edgefilename, "w")
    for e in in_edges:
        edge = e.strip().split()
        if(edge[0] in nodes and edge[1] in nodes):
            print >>foutedges, nodes[edge[0]], nodes[edge[1]]
    foutedges.close()
    
    return
    
def main():

    initial    = 10     # Size of the smallest graph
    final      = 10**5  # Size of the largest graph
    resolution = 10     # Number of graphs per 10 fold increase in vertex size 
    mult       = math.pow(initial, 1.0/resolution) # The exponent

    for filename in sys.argv[1:]:
        n = initial
        if(filename.endswith(".edges")):
            fname = filename[:-6]
            print fname
        else:
            continue
        
        # Read all the nodes
        nodefile = open(fname + ".nodes")
        in_nodes = nodefile.readlines()
        nodefile.close()

        # Read all the edges
        edgefile = open(fname + ".edges")
        in_edges = edgefile.readlines()
        edgefile.close()
    
        for count in range(1 + resolution * int(math.log(final/initial, resolution))):
            if( len(in_nodes)  < int(round(n))):
                print >>sys.stderr, "Not enough nodes", len(in_nodes), "<" ,int(round(n)) 
                break
            out_nodefile = "subgraph/"+fname+string.zfill(count, 2)+".nodes"
            out_edgefile = "subgraph/"+fname+string.zfill(count, 2)+".edges"
            print string.zfill(int(n), 6)
            print out_nodefile, out_edgefile
            generate_subgraph(int(round(n)), in_nodes, in_edges, out_nodefile, out_edgefile)
            n *= mult

main()
