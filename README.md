socialrank
==========

Implementation of SocialRank algorithm.

SocialRank is an algorithm for finding the amount of hierarchy 
in a directed social network, and ranking nodes based on their
level in this hierarchy.

SocialRank is based on the premise that the existence of a 
link indicates a social rank recommendation; a link u -> v 
(i.e. u is a follower of v) indicates a social recommendation 
of v from u. If there is no reverse link from v to u, it might 
indicate that v is higher up in the hierarchy than u. We 
assume that in social networks, when people connect to other 
people who are lower in the hierarchy, this causes them social 
agony. To infer the ranks of the nodes in the network, we find 
the best possible ranking, i.e. the ranking that gives the 
least social agony.

Based on publication:
Finding Hierarchy in Directed Online Social Networks
Mangesh Gupte, Pravin Shankar, Jing Li, S Muthukrishnan, Liviu Iftode. 
In the Proceedings of the 20th International World Wide Web Conference (WWW 2011), March 2011.

URL:
http://paul.rutgers.edu/~spravin/www11_socialhierarchy.pdf

If you find this code useful, and use it in a publication, please 
cite the above paper. Here is the bibtex entry:

@inproceedings{gupte2011hierarchy,

   author =       "Mangesh Gupte and Pravin Shankar and Jing Li and S Muthukrishnan and Liviu Iftode",
   
   title =        "{Finding Hierarchy in Directed Online Social Networks}",
   
   booktitle =    {20th International World Wide Web Conference (WWW 2011)},
   
   year =         {2011},
   
   month =        {March}

}
