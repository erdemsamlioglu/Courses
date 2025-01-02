typedef
   struct vertex 
      { char tag;
        char *label;
        struct vertex *edge[1]; }
   vertex, *vp;
   
#define TAG(vp)   ((vp)->tag)
#define LABEL(vp) ((vp)->label)  
#define EDGE(vp)  ((vp)->edge)       

vp merge(vp graph1, vp graph2);
