/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aryamamo <aryamamo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/11 13:19:05 by aryamamo          #+#    #+#             */
/*   Updated: 2025/02/11 13:19:30 by aryamamo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memset
#include <sys/time.h>
#include <unistd.h> // usleep, write

/* 使用可能な関数のみ：
memset, printf, malloc, free, write, usleep, gettimeofday,
pthread_create, pthread_detach, pthread_join,
pthread_mutex_init, pthread_mutex_destroy, pthread_mutex_lock,
	pthread_mutex_unlock
*/

// 自作の文字列長計算関数 (strlenの代替)
size_t	my_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len] != '\0')
	{
		len++;
	}
	return (len);
}

// スレッドに渡すデータ構造
typedef struct
{
	int id;                       // 人を識別するID
	pthread_mutex_t *print_mutex; // 出力時の排他制御用ミューテックスへのポインタ
}		person_data_t;

// スレッドで実行する関数
void	*person_thread(void *arg)
{
	person_data_t	*pdata;
	int				id;
	struct timeval	tv;
	const char		*msg = "Person thread: Work done.\n";

	pdata = (person_data_t *)arg;
	id = pdata->id;
	// ミューテックスで保護してメッセージ出力
	pthread_mutex_lock(pdata->print_mutex);
	printf("Person %d: Thread start.\n", id);
	pthread_mutex_unlock(pdata->print_mutex);
	// 現在時刻を取得
	gettimeofday(&tv, NULL);
	pthread_mutex_lock(pdata->print_mutex);
	printf("Person %d: Current time = %ld sec, %ld microsec\n", id, tv.tv_sec,
		tv.tv_usec);
	pthread_mutex_unlock(pdata->print_mutex);
	// usleepで処理の模擬（0.5秒待機）
	usleep(500000);
	// writeを用いてメッセージを出力
	write(STDOUT_FILENO, msg, my_strlen(msg));
	pthread_mutex_lock(pdata->print_mutex);
	printf("Person %d: Thread end.\n", id);
	pthread_mutex_unlock(pdata->print_mutex);
	return (NULL);
}

int	main(void)
{
	pthread_mutex_t	print_mutex;
	person_data_t	*pdata1;
	pthread_t		thread1;
	person_data_t	*pdata2;
	pthread_t		thread2;

	if (pthread_mutex_init(&print_mutex, NULL) != 0)
	{
		printf("Mutex initialization failed.\n");
		return (1);
	}
	/* --- join可能なスレッド (Person 1) の作成 --- */
	pdata1 = (person_data_t *)malloc(sizeof(person_data_t));
	if (pdata1 == NULL)
	{
		printf("Memory allocation failed.\n");
		pthread_mutex_destroy(&print_mutex);
		return (1);
	}
	memset(pdata1, 0, sizeof(person_data_t));
	pdata1->id = 1;
	pdata1->print_mutex = &print_mutex;
	if (pthread_create(&thread1, NULL, person_thread, pdata1) != 0)
	{
		printf("Failed to create thread 1.\n");
		free(pdata1);
		pthread_mutex_destroy(&print_mutex);
		return (1);
	}
	/* --- detachして実行するスレッド (Person 2) の作成 --- */
	pdata2 = (person_data_t *)malloc(sizeof(person_data_t));
	if (pdata2 == NULL)
	{
		printf("Memory allocation failed.\n");
		pthread_join(thread1, NULL);
		free(pdata1);
		pthread_mutex_destroy(&print_mutex);
		return (1);
	}
	memset(pdata2, 0, sizeof(person_data_t));
	pdata2->id = 2;
	pdata2->print_mutex = &print_mutex;
	if (pthread_create(&thread2, NULL, person_thread, pdata2) != 0)
	{
		printf("Failed to create thread 2.\n");
		free(pdata2);
		pthread_join(thread1, NULL);
		free(pdata1);
		pthread_mutex_destroy(&print_mutex);
		return (1);
	}
	if (pthread_detach(thread2) != 0)
	{
		printf("Failed to detach thread 2.\n");
		// detachに失敗しても続行
	}
	// join可能なスレッドの終了待ち
	if (pthread_join(thread1, NULL) != 0)
	{
		printf("Failed to join thread 1.\n");
	}
	// detach済みのスレッドはjoinできないため、十分な待ち時間を設ける
	usleep(1000000); // 1秒待機
	free(pdata1);
	free(pdata2);
	pthread_mutex_destroy(&print_mutex);
	printf("Main thread: All threads have finished.\n");
	return (0);
}
