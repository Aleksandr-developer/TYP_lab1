#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>
#include <fstream>
#include <string>
#include <condition_variable>

using namespace std;

mutex mtx;

void new_matrix_null(int** A, int n, int m)
{
	for (int i = 0; i < n; i++)
	{
		A[i] = new int[n];
		for (int j = 0; j < m; j++)
		{
			A[i][j] = 0;
		}
	}
}

void new_matrix_random(int** A, int n, int m)
{
	for (int i = 0; i < n; i++)
	{
		A[i] = new int[n];
		for (int j = 0; j < m; j++)
		{
			A[i][j] = rand() % 1000;
		}
	}
}

timed_mutex t_mtx;
condition_variable_any wait_other_threads;

int num = 1000;
int i = 0;
int j = 0;
int k = 0;
int numThreads = 0;

void mul_matrix2(int** mat1, int** mat2, int** mat3, int n, int quantity_experiments)
{
	if (numThreads < (quantity_experiments - 1))
	{
		numThreads++;
		mtx.lock();
		wait_other_threads.wait(mtx);
		mtx.unlock();
	}
	else
		wait_other_threads.notify_all();

	while (i < n)
	{
		this_thread::get_id();
		//lock_guard<mutex> firstGuard(mtx);
		mtx.lock();
		if (i >= n)
		{
			mtx.unlock();
			break;
		}
		int i_local = i, j_local = j, k_local = k;
		k++;

		if (k >= n)
		{
			k = 0; j++;
		}

		if (j >= n)
		{
			j = 0; i++;
		}

		mtx.unlock();

		mat3[i_local][j_local] += mat1[i_local][k_local] * mat2[k_local][j_local];

	}
	numThreads = 0;

}

void show_matrix(int** A, int n, int m)
{
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < m; ++j)
		{
			cout << A[i][j] << " ";
		}
		cout << endl;
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	ofstream fout;
	cout << "Введите количество потоков: ";
	cin >> num;

	int n = 10;
	int** A = new int* [n];
	new_matrix_random(A, n, n);
	int** B = new int* [n];
	new_matrix_random(B, n, n);
	int** C = new int* [n];
	new_matrix_null(C, n, n);
	int num_of_thread = _Thrd_hardware_concurrency();
	fout.open("out.xls");

	if (fout.is_open())
	{
		for (int y = 1; y <= num; y += 1)
		{
			vector <thread> th_vec;
			auto begin = std::chrono::steady_clock::now();
			for (int z = 0; z < y; z++)
			{
				th_vec.push_back(thread(mul_matrix2, A, B, C, n, k));
			}

			for (int z = 0; z < y; z++)
			{
				th_vec[z].join();
			}

			auto end = std::chrono::steady_clock::now();
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
			fout << y << "\t" << elapsed_ms.count() << "\n";
			cout << y << elapsed_ms.count() << " "
			cout << y << "-th experiment complited\n";
		}
	}
	show_matrix(C, n, n);
	return 0;
}