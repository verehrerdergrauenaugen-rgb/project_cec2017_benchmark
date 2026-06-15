/*
  CEC17 Test Function Suite for Single Objective Optimization
  Noor Awad (email: noor0029@ntu.edu.sg)
  Sep. 10th 2016
*/

#define _CRT_SECURE_NO_WARNINGS

#include <WINDOWS.H>    
#include <stdio.h>
#include <math.h>
#include <malloc.h>


void cec17_test_func(double*, double*, int, int, int);

double* OShift, * M, * y, * z, * x_bound;
int ini_flag = 0, n_flag, func_flag, * SS;


int main()
{
    const int n = 20;                       // Размерность точки: 2, 10, 20, 30, 50 или 100
    const int m = 5;                        // Одна точка
    const long long repeat_count = 1000000; // Количество повторных вычислений

    double* x = (double*)malloc(n * sizeof(double));
    double* f = (double*)malloc(m * sizeof(double));

    if (x == NULL || f == NULL)
    {
        printf("Error: not enough memory\n");
        return 1;
    }

    // Одна фиксированная точка для всех функций: x = (0, 0, ..., 0)
    for (int i = 0; i < n; i++)
    {
        x[i] = 0.0;
    }

    FILE* results = fopen("results.txt", "w");
    if (results == NULL)
    {
        printf("Error: cannot create results.txt\n");
        free(x);
        free(f);
        return 1;
    }

    fprintf(results, "CEC17 benchmark results\n");
    fprintf(results, "dimension = %d\n", n);
    fprintf(results, "repeat_count = %lld\n", repeat_count);

    fprintf(results, "point = [");
    for (int i = 0; i < n; i++)
    {
        fprintf(results, "%.17g", x[i]);
        if (i + 1 < n)
            fprintf(results, ", ");
    }
    fprintf(results, "]\n\n");

    fprintf(results, "function\tvalue\ttotal_time_sec\taverage_time_sec\n");

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    for (int func_num = 1; func_num <= 30; func_num++)
    {
        // В CEC2017 функция F2 удалена
        if (func_num == 2)
        {
            fprintf(results, "F%d\tSKIPPED: deleted function\n", func_num);
            printf("F%d skipped: deleted function\n", func_num);
            continue;
        }

        // Проверяем, что нужные input_data-файлы существуют.
        // Иначе cec17_test_func может упасть при fscanf из NULL.
        int input_ok = 1;
        char file_name[256];
        char missing_file[256];

        sprintf_s(file_name, sizeof(file_name), "input_data/M_%d_D%d.txt", func_num, n);
        FILE* test_file = fopen(file_name, "r");
        if (test_file == NULL)
        {
            input_ok = 0;
            sprintf_s(missing_file, sizeof(missing_file), "%s", file_name);
        }
        else
        {
            fclose(test_file);
        }

        if (input_ok)
        {
            sprintf_s(file_name, sizeof(file_name), "input_data/shift_data_%d.txt", func_num);
            test_file = fopen(file_name, "r");
            if (test_file == NULL)
            {
                input_ok = 0;
                sprintf_s(missing_file, sizeof(missing_file), "%s", file_name);
            }
            else
            {
                fclose(test_file);
            }
        }

        if (input_ok && ((func_num >= 11 && func_num <= 20) || func_num == 29 || func_num == 30))
        {
            sprintf_s(file_name, sizeof(file_name), "input_data/shuffle_data_%d_D%d.txt", func_num, n);
            test_file = fopen(file_name, "r");
            if (test_file == NULL)
            {
                input_ok = 0;
                sprintf_s(missing_file, sizeof(missing_file), "%s", file_name);
            }
            else
            {
                fclose(test_file);
            }
        }

        if (!input_ok)
        {
            fprintf(results, "F%d\tSKIPPED: missing %s\n", func_num, missing_file);
            printf("F%d skipped: missing %s\n", func_num, missing_file);
            continue;
        }

        // Первый запуск нужен для инициализации данных функции.
        // Его не включаем в замер времени.
        cec17_test_func(x, f, n, m, func_num);

        LARGE_INTEGER start_time;
        LARGE_INTEGER end_time;

        QueryPerformanceCounter(&start_time);

        for (long long iteration = 0; iteration < repeat_count; iteration++)
        {
            cec17_test_func(x, f, n, m, func_num);
        }

        QueryPerformanceCounter(&end_time);

        double total_time_sec =
            (double)(end_time.QuadPart - start_time.QuadPart) /
            (double)frequency.QuadPart;

        double average_time_sec = total_time_sec / (double)repeat_count;

        fprintf(
            results,
            "F%d\t%.17g\t%.9f\t%.12g\n",
            func_num,
            f[0],
            total_time_sec,
            average_time_sec
        );

        printf(
            "F%d: value = %.17g, total time = %.9f sec, average = %.12g sec\n",
            func_num,
            f[0],
            total_time_sec,
            average_time_sec
        );
    }

    fclose(results);

    free(x);
    free(f);

    free(y);
    free(z);
    free(M);
    free(OShift);
    free(x_bound);
    free(SS);

    printf("\nResults saved to results.txt\n");

    return 0;
}


