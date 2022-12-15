# ИДЗ 4 | Мовшин Максим Антонович | Вариант 34 | БПИ213
****
## Условие
34. Задача для агронома. Председатель дачного кооператива Сидоров В.И.
получил указание, что в связи с составлением единого земельного кадастра,
необходимо представить справку о площади занимаемых земель. Известно,
что территория с запада и востока параллельна меридианам, на севере ограничены параллелью, а с юга выходят к реке, описываемой функцией f(x).
Требуется создать многопоточное приложение, вычисляющее площадь
угодий методом адаптивной квадратуры. При решении использовать парадигму рекурсивного параллелизма. Замечание: кривизну Земли из-за малой
занимаемой площади не учитывать.
## Входные данные
- t - максимальное количество потоков **параметр командной строки**. `1 <= t <= 99`. Значение по умолчанию: `4`
- a, b, c - коэффициенты интегрируемого многочлена
- left, right - границы интегрирования
- precision - точность вычислений

входные данные подаются программе следующим образом:
```
./main_compiled <t>
<a> <b> <c>
<left> <right> <precision>
```
## Решение
Задачу рекуривно решает класс `CalculationThread`. Вначале происходит попытка рекурсивно запустить в отдельных потоках задачу рассчета интеграла левой и правой частей интервала. Если на данный момент количество потоков максимально, эта задача откладывается на будущее. Затем рассчитывается приближенное значение интеграла на всем интервале и его половинах, и считается ошибка между приближениями. Если она удовлетворяет заданной точности, класс сразу завершает работу и возвращает ответ. Иначе, для он либо запускает рекурсивный рассчет задачи для левой и правой частей интервала, либо ожидает результатов из уже запущенных потоков (в зависимости от действий вначале) и возвращает сумму результатов рекурсивных вызовов на обоих половинах интервала.
### Модель параллельных вычислений
Рекурсивный параллелизм - подходит для реализации программ с рекурсивными вычислениями. Каждый рекурсивный вызов порождает внутри себя новые потоки, которые независимо работают над решением задачи. Часто используется для решения задач методом "разделяй и властвуй". В данной программе действительно используется модель параллельных вычислений. Внутри основной логической функции класса потока происходит создание и запуск новых потоков, которые в свою очередь порождают новые и так далее. Все потоки независимо работают над решением задачи. Задача решается методом "разделяй и влавствуй".
### Файлы
- **main.cpp** - базовая программа на С, состоящая из **нескольких функций**.
- **main_compiled** - базовая программа на C, откомпилированная без модификации ассемблерного кода.
### Замечания
1. Программа осуществляет проверку входных данных, но всё же не защищена полностью от некорректного ввода. С учетом специфики курса, автору показалось нецелесообразным нагружать логику проверками ручного ввода при выполнении задания, направленной на развитие навыков работы с многопоточностью.
2. Выбор подынтегральных функций сужен по тем же причинам. Однако все еще настраиваем для возможности тестирования программы на различных функциях.
3. Программа имеет некоторые встроенные ограничения на вводимые параметры, однако всё же может работать с отклонениями на некоторых их вариациях. С учетом допущений пунктов 1. и 2. автору показалось более целесообразным пожертвовать однозначностью работы в угоду гибкости (поскольку программа скорее исследовательская, нежели прикладная).
### Тестовое покрытие
#### Тест 1
Входные данные:
```
-2.28 1.46789
2 4
```
Ожидаемые выходные данные:
```
83.5134 +- 0.00005
```
Результаты тестов:

![](/screenshots/test_one_main.png)
![](/screenshots/test_one_assembler.png)
#### Тест 2
Входные данные:
```
2.45 -1.46789
0 8
```
Ожидаемые выходные данные:
```
-1483.51936 +- 0.00005
```
![](/screenshots/test_two_main.png)
![](/screenshots/test_two_assembler.png)
#### Тест 3
Входные данные:
```
10 -3
-3 4
```
Ожидаемые выходные данные:
```
-61.25 +- 0.00005
```
![](/screenshots/test_three_main.png)
![](/screenshots/test_three_assembler.png)
#### Тест 4
Входные данные:
```
2 0
-3 4
```
Ожидаемые выходные данные:
```
14 +- 0.00005
```
![](/screenshots/test_four_main.png)
![](/screenshots/test_four_assembler.png)
#### Тест 5
Входные данные:
```
3 2.5
5 5
```
Ожидаемые выходные данные:
```
0
```
![](/screenshots/test_five_main.png)
![](/screenshots/test_five_assembler.png)
