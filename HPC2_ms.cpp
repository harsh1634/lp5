// T.C sequential = O(nlogn)
// T.C parallel per thread = O((nlogn)/P) + O(logn)

#include <iostream>
#include <omp.h>
#include <ctime>
#include <stdlib.h>

using namespace std;

void displayArray(int nums[], int length)
{
    cout << "Array: [ ";
    for (int i = 0; i < length; i++)
    {
        cout << nums[i] << " ";
    }
    cout << "]" << endl;
}

void merge(int nums[], int leftStart, int leftEnd, int rightStart, int rightEnd)
{
    int n = rightEnd - leftStart + 1;
    int *temp = new int[n];

    int t = 0;
    int l = leftStart;
    int r = rightStart;

    while (l <= leftEnd && r <= rightEnd)
    {
        if (nums[l] <= nums[r])
            temp[t++] = nums[l++];
        else
            temp[t++] = nums[r++];
    }

    while (l <= leftEnd)
        temp[t++] = nums[l++];

    while (r <= rightEnd)
        temp[t++] = nums[r++];

    for (int i = 0; i < n; i++)
        nums[leftStart + i] = temp[i];

    delete[] temp;
}

void seq_mergeSort(int nums[], int start, int end)
{
    if (start < end)
    {
        int mid = (start + end) / 2;
        seq_mergeSort(nums, start, mid);
        seq_mergeSort(nums, mid + 1, end);
        merge(nums, start, mid, mid + 1, end);
    }
}

void par_mergeSort(int nums[], int start, int end)
{
    if (start < end)
    {
        int mid = (start + end) / 2;
#pragma omp parallel sections num_threads(2)
        {
#pragma omp section
            par_mergeSort(nums, start, mid);
#pragma omp section
            par_mergeSort(nums, mid + 1, end);
        }
        merge(nums, start, mid, mid + 1, end);
    }
}

int main()
{
    int n;
    cout << "Enter the size of array:" << endl;
    cin >> n;

    int nums[n], seq[n], par[n];
    // cout << "Start entering the elements" << endl;
    // for (int i = 0; i < n; i++)
    // {
    //     cin >> nums[i];
    //     par[i] = seq[i] = nums[i];
    // }

    srand(time(0));

    for(int i = 0; i<n; i++){
        seq[i] = par[i] = nums[i] = rand()%1000;
    }

    cout << "Before Sorting: ";
    displayArray(nums, n);

    double seq_s = omp_get_wtime();
    seq_mergeSort(seq, 0, n - 1);
    double seq_e = omp_get_wtime();
    
    cout << "After Sorting with sequential mergesort: "<<endl;
    displayArray(seq, n);

    double par_s = omp_get_wtime();
    par_mergeSort(par, 0, n - 1);
    double par_e = omp_get_wtime();

    cout << "After Sorting with parallel mergesort: "<<endl;
    displayArray(par, n);

    cout<<endl<<"Time taken by sequential mergesort: "<<seq_e-seq_s<<endl;

    cout<<endl<<"Time taken by parallel mergesort: "<<par_e-par_s<<endl;

    return 0;
}
