#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h> 
long *input1, *input2, *output, *sum;
int threads;
 struct infoStruct {
     int threadCount;
	 int arrSize;
	 int threadPart;
 };

void* addition(void* input) {
	int threadNum = ((struct infoStruct*)input)->threadPart;
	int threadCount = ((struct infoStruct*)input)->threadCount;
	int arrSize = ((struct infoStruct*)input)->arrSize;
    for(int i = (threadNum * (arrSize/threadCount)); i < ((threadNum + 1)*(arrSize/threadCount)); ++i) {
            output[i] = input1[i] + input2[i];
    }
    pthread_exit(0);
}

void* sum_array(void* input)
{
    int threadNum = ((struct infoStruct*)input)->threadPart;
    for(int i = (threadNum* 5); i < (threadNum+1)*5; ++i) {
        sum[threadNum] += output[i];
	}
}

void merge(int low, int mid, int high)
{
	int left[mid - low + 1];
	int right[high - mid];
	int n1 = mid - low + 1, n2 = high - mid, i, j;
	for (i = 0; i < n1; i++)
		left[i] = output[i + low];
	for (i = 0; i < n2; i++)
		right[i] = output[i + mid + 1];
	int k = low;
	i = j = 0;
	while (i < n1 && j < n2) {
		if (left[i] <= right[j])
			output[k++] = left[i++];
		else
			output[k++] = right[j++];
	}
	while (i < n1) {
		output[k++] = left[i++];
	}
	while (j < n2) {
		output[k++] = right[j++];
	}
}

void merge_sort1(int low, int high)
{
	int mid = low + (high - low) / 2;
	if (low < high) {
		merge_sort1(low, mid);
		merge_sort1(mid + 1, high);
		merge(low, mid, high);
	}
}

void* merge_sort(void* input)
{
	int thread_part = ((struct infoStruct*)input)->threadPart;
	int arr_size = ((struct infoStruct*)input)->arrSize;
	int thread_count = ((struct infoStruct*)input)->threadCount;
	int low = thread_part * (arr_size / thread_count);
	int high = (thread_part + 1) * (arr_size / thread_count) - 1;
	int mid = low + (high - low) / 2;
	if (low < high) {
		merge_sort1(low, mid);
		merge_sort1(mid + 1, high);
		merge(low, mid, high);
	}
}

void parallel_sum_array(int num_threads, long *a, long *b, long *c, ssize_t size) {
	pthread_t thread_id[num_threads];
    int shared_datoutput[num_threads];
	struct infoStruct *info;
    void* status;
    for(int i = 0; i < num_threads; i++)
        shared_datoutput[i] = i;
    for(int i = 0; i < num_threads; ++i) {
		info = (struct infoStruct *)malloc(sizeof(struct infoStruct));
		info -> threadPart = shared_datoutput[i];
		info -> threadCount = num_threads;
		info -> arrSize = size;
		int result = pthread_create(&thread_id[i], NULL, addition, (void*) info);
		if(result)
			printf("Thread creation failed\n");
    }
    for(int i = 0; i < num_threads; ++i) {
            int result = pthread_join(thread_id[i], &status);
            if(result)
                printf("Join failed %d\n", i);
    }
}

long parallel_sum(int num_threads, long *ary, ssize_t array_size) {
    pthread_t thread_id[num_threads];
    int shared_datoutput[num_threads];
    struct infoStruct *info;
    void* status;
    for(int i = 0; i < num_threads; i++)
        shared_datoutput[i] = i;
    for (int i = 0; i < num_threads; i++){
        info = (struct infoStruct *)malloc(sizeof(struct infoStruct));
		info -> threadPart = shared_datoutput[i];
		info -> threadCount = num_threads;
		info -> arrSize = array_size;
        int result = pthread_create(&thread_id[i], NULL, sum_array, (void*) info);
            if(result)
                printf("Thread creation failed\n");
    }
    for(int i = 0; i < num_threads; ++i) {
            int result = pthread_join(thread_id[i], &status);
            if(result)
                printf("Join failed %d\n", i);
    }
    long total_sum = 0;
    for (int i = 0; i < num_threads; i++) {
        total_sum += sum[i];
    }
    return total_sum;
}

void parallel_sort(int num_threads, long *ary, ssize_t array_size) {
    pthread_t thread_id[num_threads];
    int shared_datoutput[num_threads];
    struct infoStruct *info;
    void* status;
    for(int i = 0; i < num_threads; i++)
        shared_datoutput[i] = i;
    for (int i = 0; i < num_threads; i++){
        info = (struct infoStruct *)malloc(sizeof(struct infoStruct));
		info -> threadPart = shared_datoutput[i];
		info -> threadCount = num_threads;
		info -> arrSize = array_size;
        int result = pthread_create(&thread_id[i], NULL, merge_sort, (void*) info);
            if(result)
                printf("Thread creation failed\n");
    }
    for(int i = 0; i < num_threads; ++i) {
            int result = pthread_join(thread_id[i], &status);
            if(result)
                printf("Join failed %d\n", i);
    }
    merge(0, (array_size / 2 - 1) / 2, array_size / 2 - 1);
	merge(array_size / 2, array_size/2 + (array_size-1-array_size/2)/2, array_size - 1);
	merge(0, (array_size - 1)/2, array_size - 1);
}


int main() {
    int arr_size;
	printf("Enter the number of threads : ");
    scanf("%d", &threads);
	printf("Enter the number of array elements : ");
    scanf("%d", &arr_size);
    input1 = (long*)malloc(arr_size * sizeof(long));
    input2 = (long*)malloc(arr_size * sizeof(long));
    output = (long*)malloc(arr_size * sizeof(long));
    sum = (long*)malloc(threads * sizeof(long));
    printf("Enter elements for Array 1: ");
    for(int i = 0; i < arr_size; i++)
        scanf("%ld", &input1[i]);
    printf("Enter elements for Array 2: ");
    for(int i = 0; i < arr_size; i++)
        scanf("%ld", &input2[i]);
    for(int i = 0; i < arr_size; i++)
        output[i] = 0;
    parallel_sum_array(threads, input1, input2, output, arr_size);
	for(int i = 0; i < arr_size; i++)
        printf("%ld + %ld = %ld\n", input1[i], input2[i], output[i]);
    printf("Sum = %ld\n", parallel_sum(threads, output, arr_size));
    parallel_sort(threads, output, arr_size);
    printf("Sorted array: ");
	for (int i = 0; i < arr_size; i++)
		printf("%ld ", output[i]);
    
}