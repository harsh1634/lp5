#include <iostream>
#include <omp.h>

using namespace std;

void displayArray(int nums[], int length)
{
    cout << "Array: [";
    for (int i = 0; i < length; i++)
    {
        cout << nums[i] << " ";
    }
    cout << "]" << endl;
}

void minOperationSequential(int nums[], int length)
{
    int minValue = nums[0];
    for (int i = 0; i < length; i++)
    {
        if (minValue > nums[i])
            minValue = nums[i];
    }
    cout << "Min value: " << minValue << endl;
    ;
}

void minOperationParallel(int nums[], int length)
{
    int minValue = nums[0];
#pragma omp parallel for reduction(min : minValue)
    for (int i = 0; i < length; i++)
    {
        if (minValue > nums[i])
            minValue = nums[i];
    }
    cout << "Min value using parallel reduction: " << minValue << endl;
    ;
}

void maxOperationSequential(int nums[], int length)
{
    int maxValue = nums[0];
    for (int i = 0; i < length; i++)
    {
        if (maxValue < nums[i])
            maxValue = nums[i];
    }
    cout << "Max value: " << maxValue << endl;
    ;
}

void maxOperationParallel(int nums[], int length)
{
    int maxValue = nums[0];
#pragma omp parallel for reduction(max : maxValue)
    for (int i = 0; i < length; i++)
    {
        if (maxValue < nums[i])
            maxValue = nums[i];
    }
    cout << "Max value using parallel reduction: " << maxValue << endl;
    ;
}

void sumOperationSequential(int nums[], int length)
{
    int sumValue = 0;
    for (int i = 0; i < length; i++)
    {
        sumValue += nums[i];
    }
    cout << "Sum value: " << sumValue << endl;
    ;
}

void sumOperationParallel(int nums[], int length)
{
    int sumValue = 0;
#pragma omp parallel for reduction(+ : sumValue)
    for (int i = 0; i < length; i++)
    {
        sumValue += nums[i];
    }
    cout << "Sum value using parallel reduction: " << sumValue << endl;
    ;
}

void avgOperationSequential(int nums[], int length)
{
    int sumValue = 0;
    for (int i = 0; i < length; i++)
    {
        sumValue += nums[i];
    }
    cout << "Average value: " << (sumValue / length) << endl;
    ;
}

void avgOperationParallel(int nums[], int length)
{
    int sumValue = 0;
#pragma omp parallel for reduction(+ : sumValue)
    for (int i = 0; i < length; i++)
    {
        sumValue += nums[i];
    }
    cout << "Average value using parallel reduction: " << (sumValue / length) << endl;
    ;
}

int main()
{
#pragma omp parallel
    {
#pragma omp single
        cout << "Number of threads: " << omp_get_num_threads() << endl;
    }
    int length = 500000;
    // cout << "Enter the size of array: " << endl;
    // cin >> length;

    int nums[length];
    // cout << "Start entering the elements:" << endl;
    // for (int i = 0; i < length; i++)
    // {
    //     cin >> nums[i];
    // }

    srand(time(0));

    for (int i = 0; i < length; i++)
    {
        nums[i] = i;
    }

    // displayArray(nums, length);

    cout << "\nSequential operations" << endl;
    double seq_s = omp_get_wtime();
    minOperationSequential(nums, length);
    maxOperationSequential(nums, length);
    sumOperationSequential(nums, length);
    avgOperationSequential(nums, length);
    double seq_e = omp_get_wtime();

    cout << "\nParallel operations:" << endl;
    double par_s = omp_get_wtime();
    minOperationParallel(nums, length);
    maxOperationParallel(nums, length);
    sumOperationParallel(nums, length);
    avgOperationParallel(nums, length);
    double par_e = omp_get_wtime();

    cout << "\nTime taken for execution of sequential operations: " << (seq_e - seq_s) << endl;
    cout << "Time taken for execution of parallel operations: " << (par_e - par_s) << endl;
}