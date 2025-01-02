#include "the2.h"
#include <cmath>
// You may write your own helper functions here

int maxvarofArr(int *arr, int size)
{
    int mymax = arr[0];
    for (int i = 1; i < size; i++)
        if (arr[i] > mymax)
            mymax = arr[i];
    return mymax;
}
 

void countSort(int *arr, int n, int exp,int groupSize,long &numberOfIterations)
{
    int output[n]; 
    int i, count[int(pow(10,groupSize))] = { 0 };
    
    
    
    
    for (i = 0; i < n; i++)
        {count[(arr[i] / exp) % int(pow(10,groupSize))]++;
        numberOfIterations++;}
 
    
    for (i = 1; i < int(pow(10,groupSize)); i++)
        {count[i] += count[i - 1];
        numberOfIterations++;}
 
    
    for (i = n - 1; i >= 0; i--) {
        output[count[(arr[i] / exp) % int(pow(10,groupSize))] - 1] = arr[i];
        count[(arr[i] / exp) % int(pow(10,groupSize))]--;
        numberOfIterations++;
        }
 
  
    for (i = 0; i < n; i++)
        {arr[i] = output[i];
        numberOfIterations++;}
        

}
 

void radixsort(int *arr, int n, int groupSize,int maxdigitLength, long &numberOfIterations)
{
    
    int max = maxvarofArr(arr, n);
 
    if (maxdigitLength%groupSize==0)
    {   for (int exp = 1; max / exp > 0; exp *= pow(10,groupSize))
            countSort(arr, n, exp, groupSize,numberOfIterations);}
    else
    {
        int division=maxdigitLength/groupSize;
        int plus=maxdigitLength%groupSize;
        int exp = 1;
        for (; division>0; exp *= pow(10,groupSize), division--)
            countSort(arr, n, exp, groupSize,numberOfIterations);
        countSort(arr, n, exp, plus,numberOfIterations);
        
        
    }
}

void reverseArray(int *arr, int var1, int var2)
{
    while (var1 < var2)
    {
        int temp = arr[var1];
        arr[var1] = arr[var2];
        arr[var2] = temp;
        var1++;
        var2--;
    }
}    


















long multiDigitRadixSort(int* arr, bool ascending, int arraySize, int groupSize, int maxDigitLength){
    long numberOfIterations = 0;
    
    
    if (ascending) //sort array with ascending order
    {

       radixsort(arr,arraySize,groupSize,maxDigitLength,numberOfIterations);
        
    }
    
    
    
    
    else //sort array with descending order
    {
        
    radixsort(arr,arraySize,groupSize,maxDigitLength,numberOfIterations);
    reverseArray(arr,0,arraySize-1);   
        
        
        
        
        
        
        
        
    }
    
    return numberOfIterations;
    
}