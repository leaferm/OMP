#include <iostream>
#include <omp.h>
#include <random>
#include <ctime>
using namespace std;
void Task1(){
    int n;
    #pragma omp parallel num_threads(8) private(n)
    {
        n = omp_get_thread_num();
        printf("Hello world printed by thread %d out of %d\n", n, omp_get_num_threads());
    }
}
void Task2(){
    int n = 3;
    omp_set_num_threads(n);
    #pragma omp parallel if(n > 2)
    {
        if (omp_in_parallel()) {
            printf("thread %d out of %d\n", omp_get_thread_num(), omp_get_num_threads());
        }
    }
    n = 1;
    omp_set_num_threads(n);
    #pragma omp parallel if(n > 2)
    {
        if (omp_in_parallel()) {
            printf("thread %d out of %d\n", omp_get_thread_num(), omp_get_num_threads());
        }
    }
}
void Task3(){
    int a=1, b=2;
    printf("Before parallel a = %d b = %d\n", a, b);
    #pragma omp parallel num_threads(2) private(a) firstprivate(b)
    {
        a+=omp_get_thread_num();
        b+=omp_get_thread_num();
        printf("In parallel a = %d b = %d thread = %d\n",a, b, omp_get_thread_num());
    }
    printf("After parallel a = %d b = %d\n\n", a, b);

    printf("Before parallel a = %d b = %d\n", a, b);
    #pragma omp parallel num_threads(4) shared(a) private(b)
    {
        a+=omp_get_thread_num();
        b+=omp_get_thread_num();
        printf("In parallel a = %d b = %d thread = %d\n",a, b, omp_get_thread_num());
    }
    printf("After parallel a = %d b = %d\n", a, b);
}
void Task4(){
    const size_t size = 10;
    int a[] = {1,2,3,4,5,6,7,5,3,1};
    int b[] = {4,5,4,3,2,1,4,6,9,0};
    int min = a[0];
    int max = b[0];
    #pragma omp parallel num_threads(2)
    {
        #pragma parallel for
        for(int i=0;i<size;i++){
            #pragma omp master
            {
                if(min > a[i]) min=a[i];
            }
            if(omp_get_thread_num() != 0){
                if(max < b[i]) max = b[i];
            }
        }
    }
    printf("min a[] = %d max b[] = %d",min,max);
}
void Task5(){
    std::mt19937 gen(time(0));
    int d[6][8];
    for(int i=0;i<6;i++){
        for(int j=0;j<8;j++){
            d[i][j]=(int)gen()%10;
        }
    }
    int sum = 0;
    int min = d[0][0];
    int max = d[0][0];
    int counter = 0;
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                #pragma omp parallel for num_threads(6) reduction(+: sum)
                for(int i=0;i<6;i++){
                    #pragma omp parallel for
                    for(int j=0;j<8;j++){
                        sum+=d[i][j];
                    }
                }

            }
            #pragma omp section
            {
                #pragma omp parallel for num_threads(6)
                for(int i=0;i<6;i++){
                    #pragma omp parallel for
                    for(int j=0;j<8;j++){
                        if(min > d[i][j]){
                            #pragma omp critical
                            min = d[i][j];
                        }
                        if(max < d[i][j]){
                            #pragma omp critical
                            max = d[i][j];
                        }
                    }
                }
            }
            #pragma omp section
            {
                #pragma omp parallel for num_threads(6) reduction(+: counter)
                for(int i=0;i<6;i++){
                    #pragma omp parallel for
                    for(int j=0;j<8;j++){
                        if(d[i][j]%3 == 0)
                            counter++;
                    }
                }
            }
        }
    }
    printf("average = %d\n", sum/48);
    printf("min = %d max = %d\n", min,max);
    printf("quantity of numbers which are divided by 3  = %d\n", counter);
    for(auto &line:d){
        for(auto item:line){
            cout<<item<<' ';
        }
        cout<<endl;
    }
}
void Task6(){
    std::mt19937 gen(time(0));
    int a[100];
    for(int i=0;i<100;i++){
        a[i] = (int)gen()%100;
    }
    int sum = 0;
    #pragma omp parallel for
    for(int i=0;i<100;i++){
        sum += a[i];
    }
    printf("average without reduction = %d\n",sum/100);
    sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < 100; i++)
        sum += a[i];
    printf("average with reduction = %d\n",sum/100);
}
void Task7() {
    int a[12], b[12], c[12];
    omp_set_num_threads(3);
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("Quantity of threads in a first zone = %d\n", omp_get_num_threads());
        }
        #pragma omp for
        for (int i = 0; i < 12; i++) {
            a[i] = i;
            printf("thread %d put in a[] value %d\n", omp_get_thread_num(), a[i]);
            b[i] = 2 * i;
            printf("thread %d put in b[] value %d\n", omp_get_thread_num(), b[i]);
        }
    }
    omp_set_num_threads(4);
    #pragma omp parallel
    {
        #pragma omp single
        {
            printf("Quantity of threads in second zone = %d\n", omp_get_num_threads());
        }
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < 12; i++) {
            c[i] = a[i] + b[i];
            printf("thread %d put in c[] value %d\n", omp_get_thread_num(),c[i] );
        }
    }
}
void Task8(){
    size_t size = 16000;
    double start, finish;
    double a[size], b[size];
    for(int i=0;i<size;i++){
        a[i]=i;
    }
    b[0] = 0;
    b[size - 1] = size;
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (int i = 1; i < size - 1; i++) {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    finish = omp_get_wtime();
    printf("static zone was completed in %f\n",finish - start);

    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic, 100)
        for (int i = 1; i < size - 1; i++) {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    finish = omp_get_wtime();
    printf("dynamic zone was completed in %f\n",finish - start);

    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for schedule(guided, 100)
        for (int i = 1; i < size - 1; i++) {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    finish = omp_get_wtime();
    printf("guided zone was completed in %f\n",finish - start);

    start = omp_get_wtime();
    #pragma omp parallel num_threads(8)
    {
        #pragma omp for schedule(auto)
        for (int i = 1; i < size - 1; i++) {
            b[i] = (a[i - 1] + a[i] + a[i + 1]) / 3.0;
        }
    }
    finish = omp_get_wtime();
    printf("static zone was completed in %f\n",finish - start);
}
void Task9(const size_t size){
    std::mt19937 gen(time(0));
    double start, finish;
    int matrix[size][size];
    int vector[size];
    int result[size];
    for(auto &line: matrix){
        for(auto &item: line){
            item = (int)gen()%10;
        }
    }
    for(auto &item: vector){
        item = (int)gen()%10;
    }
    int sum = 0;

    start = omp_get_wtime();
    for(int i=0;i<size;i++){
        sum=0;
        for(int j=0;j<size;j++){
            sum += matrix[i][j]*vector[j];
        }
        result[i]=sum;
    }
    finish = omp_get_wtime();
    for(auto item: result){
        cout<<item<<" ";
    }
    cout<<endl;
    printf("Not parallel multiplication take %f seconds\n", (finish - start));

    start = omp_get_wtime();
    #pragma omp parallel for schedule(static)
    for(int i=0;i<size;i++){
        int sum = 0;
        for(int j=0;j<size;j++){
            sum += matrix[i][j]*vector[j];
        }
        //#pragma omp critical
        result[i] = sum;
    }
    finish = omp_get_wtime();
    for(auto item: result){
        cout<<item<<" ";
    }
    cout<<endl;
    printf("Parallel multiplication take %f seconds\n", (finish - start));
}
void Task10(){
    std::mt19937 gen(time(0));
    int d[6][8];
    for(auto &line: d){
        for(auto &item: line){
            item = (int)gen()%10;
        }
    }
    for(auto &line: d){
        for(auto item: line){
            cout << item <<" ";
        }
        cout<<endl;
    }
    int min = d[0][0], max = d[0][0];
    #pragma omp parallel
    {
        #pragma omp for
        for(int i=0;i<6;i++){
            int lmin = d[i][0];
            int lmax = d[i][0];
            for(int j=0;j<8;j++){
                if(d[i][j]>lmax) lmax = d[i][j];
                if(d[i][j]<lmin) lmin = d[i][j];
            }
            #pragma omp critical
            {
                if(lmin < min) min = lmin;
                if(lmax > max) max = lmax;
            }
        }
    }
    printf("max = %d, min = %d",max, min);
}
int main(){
    Task10();
    return 0;
}