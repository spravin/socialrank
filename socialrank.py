from subprocess import call
import sys

def print_usage():
    """Print the input format for the program"""
    print "Usage : \n"
    print "python framework.py output list_of_input_files"
    print "output\t : File where the summary output statistics will be written"
    print "list_of_input_files : The files on which to apply the socialrank algorithm."
    print "\t\tThese should be .edges and .nodes files. "
    print ""
    
def get_filelist():
    filelist = []

    for s in sys.argv[1:]:
        if not s.endswith(".nodes"):
            continue
        t = s[:-6]
        if(t + ".edges" in sys.argv[1:]):
            filelist.append(t)
        else:
            print >>sys.stderr,"Error :",s," does not have edges file"
    filelist.sort()
    return filelist

def main():
    if(len (sys.argv) < 2 or sys.argv[1].endswith(".nodes") or sys.argv[1].endswith(".edges")):
        print_usage()
        return
    
    filelist = get_filelist()
    for f in filelist:
        call(["./socialrank", sys.argv[1], f])

main()
