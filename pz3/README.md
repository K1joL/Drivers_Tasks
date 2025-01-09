## Задание
1. Подключиться к Raspberry PI с помощью minicom и узнать версию ядра.
2. Собрать сборку ядра с cyclictest при помощи buildroot
3. Запустить cyclictest
4. Наложить rt-патч на ядро
5. Запустить cyclictest

## Выполнение задания
1. Вывод cyclictest для ядра без RT патча

Без RT патча:
    Min: 5
    Avg: 12
    Max: 172

![Скриншот вывода cyclictest для ядра без RT патча](/pz3/Default.jpg)

2. Вывод cyclictest для ядра с RT патчем

С RT патчем:
    Min: 8
    Avg: 15
    Max: 76
    
![Скриншот вывода cyclictest для ядра с RT патчем](/pz3/RT-patch.jpg)
