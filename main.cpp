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
	std::vector<long> v(1000000);
	for (long i = 0; i < 1000000; ++i)
		v[i] = (i * i) % 1000000;
	v = mergesort(v, 1);
	for (long i = 0; i < 1000000; ++i)
		std::cout << v[i] << "\n";
}
