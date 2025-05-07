#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <omp.h>

using namespace std;

void seq_bubblesort(int *a, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (a[j] > a[j + 1])
                swap(a[j], a[j + 1]);
        }
    }
}

void par_bubblesort(int *a, int n)
{
    for (int i = 0; i < n; i++)
    {
        int first = i % 2;
#pragma opm parallel for shared(a, first)
        for (int j = first; j < n - 1; j += 2)
        {
            // int tid = omp_get_thread_num();
            // #pragma opm critical
            // {
            //     cout<<"Phase "<<i<<" , comparing indices ("<<j<<","<<j+1<<") "<<"on thread "<<tid<<endl;
            // }
            if (a[j] > a[j + 1])
                swap(a[j], a[j + 1]);
        }
    }
}

void swap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

int main()
{
    int n;
    cout << "Enter size of array:" << endl;
    cin >> n;

    int *seq_arr = new int[n];
    int *par_arr = new int[n];
    // cout<<"Start entering the elements:"<<endl;
    // for(int i = 0; i<n; i++){
    //     cin>>seq_arr[i];
    //     par_arr[i] = seq_arr[i];
    // }

    srand(time(0));
    for (int i = 0; i < n; i++)
    {
        seq_arr[i] = par_arr[i] = rand() % 10000;
    }
    // omp_set_num_threads(5);

#pragma omp parallel
    {
#pragma omp single
        cout << "Number of threads: " << omp_get_num_threads() << endl;
    }

    double seq_s = omp_get_wtime();
    seq_bubblesort(seq_arr, n);
    double seq_e = omp_get_wtime();

    double par_s = omp_get_wtime();
    par_bubblesort(par_arr, n);
    double par_e = omp_get_wtime();

    cout << endl
         << "Output of sequential bubblesort" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << seq_arr[i] << " ";
    }

    cout << endl
         << "Output of parallel bubblesort:" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << par_arr[i] << " ";
    }

    cout << endl
         << endl
         << "Time taken by sequential bubblesort: " << seq_e - seq_s << endl;

    cout << endl
         << "Time taken by parallel bubblesort: " << par_e - par_s << endl;
}