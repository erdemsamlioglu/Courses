
#include <stdio.h>
#include <math.h>

int main()
{
    double deltat, v;
    int n;
    int count;
    int i;
    double tmp;
    double arrX[500];
    double arrCur[500];
    double preco;
    double co;
    double sformul;
    double vson;
    double totalx;
    double totaltime;
    double R;

    count = 0;
    i=0;
    preco=0;
    totalx=0;
    totaltime =0;

    scanf("%lf %lf %d", &deltat, &v, &n);


    while(count<=n+1)
    {
      scanf("%lf", &tmp);
      arrX[count]=tmp;
      scanf("%lf", &tmp);
      arrCur[count]=tmp;
      count ++;
    }


    while(i<=n)
    {



      i++;
    }


    R=arrX[n];
    i=0;




    while(totalx<R)
    {
      while(totalx<arrX[i+1])
      {
         co=preco*(-1)+2*((arrCur[i+1]-arrCur[i])/(arrX[i+1]-arrX[i]));
      sformul=arrCur[i]+preco*(totalx-arrX[i])+((co-preco)/(2*(arrX[i+1]-arrX[i])))*(totalx-arrX[i])*(totalx-arrX[i]);
      vson=sqrt((v*v)-(sformul*sformul));
      totalx+=vson*deltat;
      totaltime+=deltat;

      }
      preco=co;
      i=i+1;
    }

    printf("%lf", totaltime);















    return 0;

}
