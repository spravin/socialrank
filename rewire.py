# Given a (directed) graph, rewire it.
# For each edge, we change its direction with probability 1/2. 
import sys
import random
from subprocess import call

def main():
    p = 0.5
    random.seed(86533254)
    for filename in sys.argv[1:]:
        # Check if this is an edges file
        if(filename.endswith(".edges")):
            filename = filename[:-6]
        else:
            continue

        # Check if the edges file has a nodes file 
        if(filename + ".nodes" not in sys.argv[1:]):
            print >>sys.stderr, "Error: No nodes file for ",filename
            continue
        
        print filename
        fin = open(filename + ".edges")
        lines = fin.readlines()
        fin.close()
        for trial in range(5):
            # Make a copy of the nodes file
            nodefilename = "rewired/"+filename + "_t"+str(trial)+".nodes"
            call(["cp", filename +".nodes", nodefilename])
            # Rewire the edges
            edgefilename = "rewired/"+filename + "_t"+str(trial)+".edges"           
            fout = open(edgefilename,"w")
            rewire_graph(lines, fout, p)
            fout.close()

def rewire_graph(fin, fout, p):
    outedges = set()
    for line in fin:
        (u,v) = line.strip().split()
        if( random.random() <= p ):
            outedges.add(u + " " + v)
        else:
            outedges.add(v + " " + u)
            
    for l in outedges:
        print >>fout, l
main()
