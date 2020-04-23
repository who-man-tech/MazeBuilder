#include <iostream>
#include <cstdlib>

#include "src/maze_gen.h"

int main(int, char**) {
    
    Field field(6, 6, 3, 2);

    // Инициализация генератора случайных чисел
    std::srand(10);

    // Создаем основной путь

    // Идем по бесконечному циклу, пока не достигнем точки финиша
    // Получаем ячейку из пути по индексу
    // Получаем массив стен этой ячейки
    // Идем по циклу от 0, до 3
    // Проверяем верхнюю ячейку, получаем из field ячейку, у которой y текущей ячейки больше на 1
    // Проверяем правую ячейку, получаем из field ячейку, у которой x текущей ячейки больше на 1
    // Проверяем нужнюю ячейку, получаем из field ячейку, у которой y текущей ячейки меньше на 1
    // Проверяем левую ячейку, получаем из field ячейку, у которой x текущей ячейки меньше на 1

    // Получаем случайное направление
    // Провераяем данное направление

    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 6; x++) {
            Cell cell = field.get_cell(x, y);
            switch (cell.type()) {
                case Cell::ctNormal: 
                    std::cout << "N ";
                    break;

                case Cell::ctStart: 
                    std::cout << "S ";
                    break;

                case Cell::ctFinish: 
                    std::cout << "F ";
                    break;
            }
        }
        std::cout << '\n';
    }

    field.trace_route();

    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 6; x++) {
            Cell cell = field.get_cell(x, y);
                if (cell.path() != nullptr) // TODO: Измить условие, чтобы бьла возможность определять к какому пути принадлежит ячейка
                    std::cout << "1 ";
                else
                    std::cout << "0 ";
        }
        std::cout << '\n';
    }


    return 0;
}
