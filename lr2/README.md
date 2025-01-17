## Задание лабораторной работы

1.  Написать символьный драйвер, который будет генерировать с заданной частотой внешние воздействия и принимать реакцию на них.
2.  Измерить в драйвере время реакции (найти среднее и максимальное).
3.  Построить гистограмму времени реакции.
4.  Оценить влияние:
    *   Приоритета процесса;
    *   Дисциплины диспетчеризации;
    *   Внешней загрузки (HDD, сеть).

## Выполнение

В ходе работы были разработаны:

1.  **Модуль ядра (символьный драйвер):** Генерирует внешние воздействия каждые 100 мс и измеряет время реакции.
2.  **Приложение:** Взаимодействует с драйвером каждые 10 мс.

### Результаты тестов
Тестирование проводилось на операционной системе Ubuntu 22.04.

**1. Обычный режим:**
   * Среднее время отклика: 2.00597 мс
   * Максимальное время отклика: 41.16306 мс

![Скриншот Обычный режим](/lr2/Tests/Обычный%20запуск.png)

**2. Высокий приоритет:**
   * Среднее время отклика: 1.28338 мс
   * Максимальное время отклика: 9.79093 мс

![Скриншот Высокий приоритет](/lr2/Tests/Высокий%20приоритет.png)

**3. Низкий приоритет:**
   * Среднее время отклика: 1.31712 мс
   * Максимальное время отклика: 23.15756 мс

![Скриншот Низкий приоритет](/lr2/Tests/Низкий%20приоритет.png)

**4. Дисциплина диспетчеризации SCHED_BATCH:**
   * Среднее время отклика: 1.63377 мс
   * Максимальное время отклика: 20.26886 мс

![Скриншот Дисциплина диспетчеризации SCHED_BATCH](/lr2/Tests/Планировщик%20SCHED_BATCH.png)

**5. Дисциплина диспетчеризации SCHED_FIFO:**
   * Среднее время отклика: 0.95466 мс
   * Максимальное время отклика: 16.02450 мс

![Скриншот Обычный режим](/lr2/Tests/Планировщик%20SCHED_FIFO.png)

**6. Дисциплина диспетчеризации SCHED_RR:**
   * Среднее время отклика: 1.87247 мс
   * Максимальное время отклика: 18.74743 мс

![Скриншот Дисциплина диспетчеризации SCHED_RR](/lr2/Tests/Планировщик%20SCHED_RR.png)

**7. Дисциплина диспетчеризации SCHED_IDLE:**
   * Среднее время отклика: 1.20158 мс
   * Максимальное время отклика: 9.36210 мс

![Скриншот Дисциплина диспетчеризации SCHED_IDLE](/lr2/Tests/Планировщик%20SCHED_IDLE.png)

**8. Дисциплина диспетчеризации SCHED_NORMAL/OTHER:**
   * Среднее время отклика: 2.37530 мс
   * Максимальное время отклика: 69.36455 мс

![Скриншот Дисциплина диспетчеризации SCHED_NORMAL/OTHER](/lr2/Tests/Планировщик%20SCHED_NORMAL-OTHER.png)

**9. Под нагрузкой HDD:**
   * Среднее время отклика: 1.42230 мс
   * Максимальное время отклика: 35.05833 мс

![Скриншот Под нагрузкой HDD](/lr2/Tests/Нагрузка%20на%20HDD.png)

**10. Под нагрузкой сети:**
   * Среднее время отклика: 2.17121 мс
   * Максимальное время отклика: 13.16306 мс
  
![Скриншот Под нагрузкой сети](/lr2/Tests/Нагрузка%20на%20сеть.png)
