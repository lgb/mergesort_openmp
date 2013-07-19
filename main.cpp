/*
 * Изначальный код см. в статье:
 * http://www.ibm.com/developerworks/ru/library/au-aix-openmp-framework/index.html
 */

#include <omp.h>

#include <iostream>
#include <vector>

std::vector<long> merge(const std::vector<long>& left, const std::vector<long>& right)
{
	std::vector<long> result;
	unsigned left_it = 0, right_it = 0;

	while (left_it < left.size() && right_it < right.size())
	{
		if (left[left_it] < right[right_it])
		{
			result.push_back(left[left_it]);
			left_it++;
		}
		else
		{
			result.push_back(right[right_it]);
			right_it++;
		}
	}

	// Занесение оставшихся данных из обоих векторов в результирующий
	while (left_it < left.size())
	{
		result.push_back(left[left_it]);
		left_it++;
	}

	while (right_it < right.size())
	{
		result.push_back(right[right_it]);
		right_it++;
	}

	return result;
}

std::vector<long> mergesort(std::vector<long>& vec, int threads)
{
	// Условие завершения: список полностью отсортирован,
	// если он содержит только один элемент.
	if (vec.size() == 1)
	{
		return vec;
	}

	// Определяем местоположение среднего элемента в векторе
	std::vector<long>::iterator middle = vec.begin() + (vec.size() / 2);

	std::vector<long> left(vec.begin(), middle);
	std::vector<long> right(middle, vec.end());

	// Выполнение сортировки слиянием над двумя меньшими векторами
	if (threads > 1)
	{
		#pragma omp parallel sections
		{
			#pragma omp section
			{
			  left = mergesort(left, threads / 2);
			}
			#pragma omp section
			{
			  right = mergesort(right, threads - threads / 2);
			}
		}
	}
	else
	{
		left = mergesort(left, 1);
		right = mergesort(right, 1);
	}

	return merge(left, right);
}

int main()
{
	const long VEC_SIZE = 1000000;
	std::vector<long> v(VEC_SIZE);	// ~4MB

	int num_threads;
	double wall_timer;

	for (num_threads = 1; num_threads <= 16; ++num_threads)
	{
		// перезаполнение вектора
		for (long i = 0; i < VEC_SIZE; ++i)
			v[i] = (i * i) % VEC_SIZE;

		// собственно сортировка
		wall_timer = omp_get_wtime();
		v = mergesort(v, num_threads);
		wall_timer = omp_get_wtime() - wall_timer;

		std::cout << "threads: " << num_threads << "\ttime on wall: " <<  wall_timer << std::endl;
	}
}
