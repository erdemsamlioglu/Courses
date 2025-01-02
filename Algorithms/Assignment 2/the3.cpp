#include "the3.h"



int recursive_sln(int i, char**& arr, int*& len, int &number_of_calls) { //direct recursive
    number_of_calls+=1;
    
    if(i<0)
    {
        return 0;
    }
    
    else if(number_of_calls==1)
    {
        
        int j=i-1;
        char now=arr[i][0];
        
        for(; j>-1;j--)
        {
            
            if(now=='S')
            {
                if(arr[j][1]=='S')
                { 
                    break;
                } 
            }
            
            else if(now=='O')
            {
                if(arr[j][1]=='I')
                {
                    break;
                }
                
            }

            else if(now=='I')
            {
                if(arr[j][1]=='O')
                {
                    break;
                }         
            }       
    }
        
        return std::max( recursive_sln(i-1, arr, len ,number_of_calls),   len[i]+ recursive_sln(j,arr,len ,number_of_calls));    
    }
    
    else
    {
        
        int j=i-1;
        
        char now=arr[i][1];
        for(; j>-1;j-- )
        {
            
            if(now=='S')
            {
                
                if(arr[j][1]=='S')
                {
                    
                    break;
                }
            
                
            }
            
            else if(now=='O')
            {
                if(arr[j][1]=='O')
                {
                    break;
                }
                
                
                
                
            }

            else if(now=='I')
            {
                if(arr[j][1]=='I')
                {
                    break;
                }
                
                
                
            }


            
            
            
        }
        
        
        int k=i-1;
        char now2=arr[i][0];
        for(; k>-1;k--)
        {
            
            if(now2=='S')
            {
                
                if(arr[k][1]=='S')
                {
                    
                    break;
                }
            
                
            }
            
            else if(now2=='O')
            {
                if(arr[k][1]=='I')
                {
                    break;
                }
                
                
                
                
            }

            else if(now2=='I')
            {
                if(arr[k][1]=='O')
                {
                    break;
                }
                            
                
            }

                    
        } 
        
        return std::max( recursive_sln(j, arr, len, number_of_calls),   len[i]+  recursive_sln(k,arr,len ,number_of_calls));
        
    }
    
}

    

int memoization_sln(int i, char**& arr, int*& len, int**& mem) { //memoization

    int actual_length;
    int temp1;

    if (i == 0)
    {
        if (arr[0][1] == 'I')
        {
            mem[0][0] = len[0];
            mem[0][1] = 0;
            mem[0][2] = 0;
        }
        else if (arr[0][1] == 'O')
        {
            mem[0][1] = len[0];
            mem[0][0] = 0;
            mem[0][2] = 0;
        }
        else if (arr[0][1] == 'S')
        {
            mem[0][2] = len[0];
            mem[0][0] = 0;
            mem[0][1] = 0;
        }
        return len[0];
    }
    else
    {
        int returnWhich;
        if (arr[i][0] == 'I' && arr[i][1] == 'S')
        {
            if (mem[i - 1][1] == -1 || mem[i - 1][2] == -1)
               memoization_sln(i - 1, arr, len, mem);
               
            mem[i][0] = mem[i - 1][0];
            mem[i][2] = fmax(mem[i - 1][1] + len[i], mem[i - 1][2]);
            mem[i][1] = mem[i - 1][1];
            returnWhich = 2;
        }

        else if (arr[i][0] == 'I' && arr[i][1] == 'O')
        {
            if (mem[i - 1][1] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][1] = fmax(mem[i - 1][1] + len[i], mem[i - 1][1]);
            mem[i][2] = mem[i - 1][2];
            mem[i][0] = mem[i - 1][0];
            returnWhich = 1;
        }

        else if (arr[i][0] == 'I' && arr[i][1] == 'I')
        {
            if (mem[i - 1][1] == -1 || mem[i - 1][0] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][0] = fmax(mem[i - 1][1] + len[i], mem[i - 1][0]);
            mem[i][2] = mem[i - 1][2];
            mem[i][1] = mem[i - 1][1];
            returnWhich = 0;
        }
        //For I

        else if (arr[i][0] == 'O' && arr[i][1] == 'S')
        {
            if (mem[i - 1][0] == -1 || mem[i - 1][2] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][2] = fmax(mem[i - 1][0] + len[i], mem[i - 1][2]);
            mem[i][1] = mem[i - 1][1];
            mem[i][0] = mem[i - 1][0];
            returnWhich = 2;
        }

        else if (arr[i][0] == 'O' && arr[i][1] == 'O')
        {
            if (mem[i - 1][0] == -1 || mem[i - 1][1] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][1] = fmax(mem[i - 1][0] + len[i], mem[i - 1][1]);
            mem[i][2] = mem[i - 1][2];
            mem[i][0] = mem[i - 1][0];
            returnWhich = 1;
        }

        else if (arr[i][0] == 'O' && arr[i][1] == 'I')
        {
            if (mem[i - 1][0] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][0] = fmax(mem[i - 1][0] + len[i], mem[i - 1][0]);
            mem[i][2] = mem[i - 1][2];
            mem[i][1] = mem[i - 1][1];
            returnWhich = 0;
        }

        //For O

        else if (arr[i][0] == 'S' && arr[i][1] == 'S')
        {
            if (mem[i - 1][2] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][2] = fmax(mem[i - 1][2] + len[i], mem[i - 1][2]);
            mem[i][1] = mem[i - 1][1];
            mem[i][0] = mem[i - 1][0];
            returnWhich = 2;
        }

        else if (arr[i][0] == 'S' && arr[i][1] == 'O')
        {
            if (mem[i - 1][1] == -1 || mem[i - 1][2] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][1] = fmax(mem[i - 1][2] + len[i], mem[i - 1][1]);
            mem[i][2] = mem[i - 1][2];
            mem[i][0] = mem[i - 1][0];
            returnWhich = 1;
        }

        else if (arr[i][0] == 'S' && arr[i][1] == 'I')
        {
            if (mem[i - 1][0] == -1 || mem[i - 1][2] == -1)
                memoization_sln(i - 1, arr, len, mem);
            mem[i][0] = fmax(mem[i - 1][2] + len[i], mem[i - 1][0]);
            mem[i][1] = mem[i - 1][1];
            mem[i][2] = mem[i - 1][2];
            returnWhich = 0;
        }

        //return mem[i][returnWhich];
    }
    temp1=fmax(mem[i][1], mem[i][2]);
    actual_length=fmax(mem[i][0],temp1);
    return actual_length;


}



    
    
    
    
    
    
    
    
  




int dp_sln(int size, char**& arr, int*& len, int**& mem) { //dynamic programming
    
    int actual_length;
    int temp1;
    
    if (arr[0][1]=='I')
    {
        mem[0][0]=len[0];
        mem[0][1]=0;
        mem[0][2]=0;
        
        
    }
    else if (arr[0][1]=='O')
    {
        mem[0][1]=len[0];
        mem[0][0]=0;
        mem[0][2]=0;
        
        
    }
    else if (arr[0][1]=='S')
    {
        mem[0][2]=len[0];
        mem[0][0]=0;
        mem[0][1]=0;
        
        
    }
    for(int i=1; i< size; i++)
    {
       
       if(arr[i][0]=='I' && arr[i][1]=='S')
       {
           mem[i][2]=fmax(mem[i-1][1]+len[i], mem[i-1][2]);
           mem[i][1]=mem[i-1][1];
           mem[i][0]=mem[i-1][0];

           
       }
       
       else if(arr[i][0]=='I' && arr[i][1]=='O')
       {
           mem[i][1]=fmax(mem[i-1][1]+len[i], mem[i-1][1]);
           mem[i][2]=mem[i-1][2];
           mem[i][0]=mem[i-1][0];
           
       }
       
       else if(arr[i][0]=='I' && arr[i][1]=='I')
       {
           mem[i][0]=fmax(mem[i-1][1]+len[i], mem[i-1][0]);
           mem[i][2]=mem[i-1][2];
           mem[i][1]=mem[i-1][1];
           
       }
       
       //For I
       
       else if(arr[i][0]=='O' && arr[i][1]=='S')
       {
           mem[i][2]=fmax(mem[i-1][0]+len[i], mem[i-1][2]);
           mem[i][1]=mem[i-1][1];
           mem[i][0]=mem[i-1][0];
           
       }
       
       else if(arr[i][0]=='O' && arr[i][1]=='O')
       {
           mem[i][1]=fmax(mem[i-1][0]+len[i], mem[i-1][1]);
           mem[i][2]=mem[i-1][2];
           mem[i][0]=mem[i-1][0];
           
       }
       
       else if(arr[i][0]=='O' && arr[i][1]=='I')
       {
           mem[i][0]=fmax(mem[i-1][0]+len[i], mem[i-1][0]);
           mem[i][2]=mem[i-1][2];
           mem[i][1]=mem[i-1][1];
           
       }
       
       //For O
       
       else if(arr[i][0]=='S' && arr[i][1]=='S')
       {
           mem[i][2]=fmax(mem[i-1][2]+len[i], mem[i-1][2]);
           mem[i][1]=mem[i-1][1];
           mem[i][0]=mem[i-1][0];
           
       }
       
       else if(arr[i][0]=='S' && arr[i][1]=='O')
       {
           mem[i][1]=fmax(mem[i-1][2]+len[i], mem[i-1][1]);
           mem[i][2]=mem[i-1][2];
           mem[i][0]=mem[i-1][0];
           
       }
       
       else if(arr[i][0]=='S' && arr[i][1]=='I')
       {
           mem[i][0]=fmax(mem[i-1][2]+len[i], mem[i-1][0]);
           mem[i][1]=mem[i-1][1];
           mem[i][2]=mem[i-1][2];
           
       }
       
       
       
       
       



    }
    temp1=fmax(mem[size-1][1], mem[size-1][2]);
    actual_length=fmax(mem[size-1][0],temp1)

    return actual_length ;
}