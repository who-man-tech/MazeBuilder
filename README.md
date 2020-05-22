# MazeBuilder
Random maze builder

Задание из книги Чарльза Уэзерелла "Этюды для программистов"

Базовые условия построения лабиринта:

  1. Лабиринт имеет прямоугольную форму m x n ячеек
  2. Лабиринт должен иметь один вход и один выход.
  3. Вход и выход распологаются на противоположных сторонах лабиринта.
  4. Правильным может быть только один путь.
  5. Путь не может быть прямым.
  6. Ответвления должны заканчиваться тупиками.
  

## Объекты и их свойства:

  1. Поле - cодержит в себе m * n(высота) ячеек.

  2. Путь - содержит ссылки на все ячейки, которые в него входят.

  3. Ячейка определяется положением на поле(x, y). Содержит информацию о принадлежности к пути(если ничему не принадлежит, то -1). Массив стенок окружающих её(0-верхняя, 1-правая, 2-нижняя, 3-левая). Тип(вход, выход, обычная).  Массив направлений окружающих её(0-верхнее, 1-правое, 2-нижнее, 3-левое).

## Алгоритм построения:
Входные данные: ширина и высота лабиринта(0, 0 - сверху-слева), начало последовательности случайных чисел.

  1. Определяем положения входа на левой и выхода на правой сторонах.

  2. Создаем поле из ячеек. Указываем тип для ячеек входа и выхода. Для ячеек, которые стоят по сторонам поля, проставляем стенки.

  3. Поиск пути:

    1. Начинаем со стартовой ячейки.

    2. Указываем, что текущая ячейка принадлежит пути.

    3. Проверяем все направления ячейки:

      1. Если направление указывает на ячейку, которая принадлежит пути, то ставим стену между ячейками и устанавливаем в обоих ячейках данное направление, как не доступное.

      2. Если достигнута ячейка выхода, указываем, что эта ячейка принадлежит пути и завершаем построение пути.

      3. Если нет доступных направлений, то убираем у ячейки принадлежность к пути, возвращаемся на предыдущую ячейку и указываем данное направление как не доступное.

    4. Случайно выбираем направление следующего шага из доступных.

    5. Проверяем направление ячейки на доступность:

      1. Если направление доступно, то переходим к ячейке по выбранному направлению и повторяем шаг 3.2.

      2. Иначе переходим к 3.4 шагу.

  4. Создание ветвлений:

    1. Выбираем все ячейки пути из которых можно создать ветвление. Если таких ячеек не осталось, то переходим пп. 5.
    
    2. Случайно выбираем часть ячеек из списка доступных для ветвления для начала нового ветвления. Создаем из них список новых ветвлений.

    3. Параллельно идем по каждому из новых ветвлений до тех пор пока список новых ветвлений не пуст:
    
      1. Берем случайное доступное направление для текущей ячейки. Указываем ячейке по выбранному направлению,
         что она принадлежит ответвлению и переходим в неё. Переходим к пп. 4.3.1. Если доступных направлений нет,
         то завершаем создание ветвления и удаляем его из списка новых ветвлений.
  
  5. Зачистка всего поля:
    
    1. Ищем ячейки, которые не принадлежат пути или ветвлению. Если такие имеются, то устанавливаем для них стены со всех сторон.


## Использование:
  - **Аргументы при вызове функции:**
    - help -- Вывод инструкции
    - w -- ширина поля
    - h -- высота поля
    - s или start -- позиция ячейки старта на левой строне
    - f или finish -- позиция ячейки финиша на правой строне
    - d или debug -- включение вывода внутренней информации
    - r или randon -- значение для рандомной последовательности