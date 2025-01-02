#include "the3.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


vp* returntraversedinarray(vp graph2)
{
	 int i;
    int u;
    int num_elements = 200;
    int front = 0;
    int it =0;
    int qit =1;
    int num =0;

    vp cur;

    vp* vpqueue = (vp*) malloc(num_elements* sizeof(vp));

    TAG(graph2) = 'a';
    vpqueue[0] = graph2;


    while(vpqueue[front]!='\0')
    {

        cur = vpqueue[front];
        it =0;

        while (EDGE(cur)[it]!='\0')  {


            if (TAG(EDGE(cur)[it])!= 'a')

            {

                TAG(EDGE(cur)[it])= 'a';
                vpqueue[qit] = EDGE(cur)[it] ;

                qit++;

                vpqueue[qit] = '\0';
            }

            it++;
        }

        front++;

    }

    for (u =0; vpqueue[u] != '\0'; u++ ) {

        TAG(vpqueue[u]) = 'u';
    }

    return vpqueue;
}


vp merge(vp graph1, vp graph2)
{
     int i;
     int a;
     int b;
     int c;
     int d ;
     int e;
     int f;
     int g;
     int h ;
    int u;
    int num_elements = 200;
    int front = 0;
    int it =0;
    int qit =1;
    int solda = 0;
    int soldaExists = 0;
    int num =0;
    vp cur;
    vp ptr;
    vp ptr2;
    vp temp;

    vp* vpqueue = (vp*) malloc(num_elements* sizeof(vp));
    vp* vpqueueCopy = (vp*) malloc(num_elements* sizeof(vp));

    TAG(graph1) = 'p';
    vpqueue[0] = graph1;


    while(vpqueue[front]!='\0')
    {

        cur = vpqueue[front];
        it =0;


        while (EDGE(cur)[it]!='\0')  {



            if (TAG(EDGE(cur)[it])!= 'p')

            {

                TAG(EDGE(cur)[it])= 'p';


                vpqueue[qit] = EDGE(cur)[it] ;

                qit++;

                vpqueue[qit] = '\0';
            }

            it++;
        }

        front++;

    }

    for (u =0; vpqueue[u] != '\0'; u++ ) {

        TAG(vpqueue[u]) = 'u';
    }


     for (u =0; vpqueue[u] != '\0'; u++ ) {

       ptr = (vp) malloc(sizeof(vertex) + 50*(sizeof(vp)));
       LABEL(ptr) = strdup(LABEL(vpqueue[u])) ;
       vpqueueCopy[u] = ptr ;
       num ++;
    }
     vpqueueCopy[u] = '\0' ; /////

     for (u =0; vpqueue[u] != '\0'; u++ ) {

       for (a=0; EDGE(vpqueue[u])[a] != '\0' ; a++) {
            for (b=0; vpqueueCopy[b] != '\0' ; b++) {
                if(strcmp(LABEL(vpqueueCopy[b]), (LABEL(EDGE(vpqueue[u])[a])) )== 0)
                EDGE(vpqueueCopy[u])[a] = vpqueueCopy[b];
            }

     }
     EDGE(vpqueueCopy[u])[a] = '\0';
    }



    vp* secondQueue = returntraversedinarray(graph2) ;
     for (u =0; vpqueueCopy[u] != '\0'; u++ ) {
            for (a =0; secondQueue[a] != '\0'; a++ ) {
              if(strcmp(LABEL(vpqueueCopy[u]), LABEL(secondQueue[a]) )== 0) {
                   for (b=0; EDGE(secondQueue[a])[b] != '\0' ; b++) {
                       solda = 0;
                       for (c=0; EDGE(vpqueueCopy[u])[c] != '\0' ; c++) {
                            if(strcmp(LABEL(EDGE(secondQueue[a])[b]), LABEL(EDGE(vpqueueCopy[u])[c]))==0)
                               solda = 1;
                        }
                     if (solda == 0) {
                            soldaExists =0;
                            ptr2=NULL;
                            for(d=0; vpqueueCopy[d]!='\0' ; d++){
                                if(strcmp(LABEL(vpqueueCopy[d]), LABEL(EDGE(secondQueue[a])[b]))== 0)
                                {
                                    soldaExists=1;
                                    ptr2=vpqueueCopy[d];
                                }
                            }
                            if(soldaExists==1)
                            {
                                EDGE(vpqueueCopy[u])[c] = ptr2;
                                EDGE(vpqueueCopy[u])[c+1] = '\0';
                            }
                            else
                            {
                        ptr = (vp) malloc(sizeof(vertex) + 50*(sizeof(vp)));
                        LABEL(ptr) = strdup(LABEL(EDGE(secondQueue[a])[b])) ;
                        vpqueueCopy[num] = ptr ;
                        num ++;
                        EDGE(vpqueueCopy[u])[c] = ptr;
                        EDGE(vpqueueCopy[u])[c+1] = '\0';
                            }

                       }


                   }
              }
     }

     }

    for(e=0; vpqueueCopy[e]!= '\0'; e++)
    {
        for(f=0; EDGE(vpqueueCopy[e])[f]!= '\0'; f++)
        {
           for(g=f+1; EDGE(vpqueueCopy[e])[g] != '\0'; g++)
           {
               for(h=0; h<strlen(LABEL(EDGE(vpqueueCopy[e])[f])) && h<strlen(LABEL(EDGE(vpqueueCopy[e])[g])) ; h++ )
               {
                   if(LABEL(EDGE(vpqueueCopy[e])[f])[h] > LABEL(EDGE(vpqueueCopy[e])[g])[h])
                   {
                      temp = EDGE(vpqueueCopy[e])[f];
                      EDGE(vpqueueCopy[e])[f] = EDGE(vpqueueCopy[e])[g];
                      EDGE(vpqueueCopy[e])[g] = temp;
                      break;
                   }
                   else if(LABEL(EDGE(vpqueueCopy[e])[f])[h] < LABEL(EDGE(vpqueueCopy[e])[g])[h])
                    break;

               }
           }

        }
    }



     return vpqueueCopy[0]  ;



}


