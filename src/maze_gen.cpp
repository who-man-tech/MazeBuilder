#include <cassert>
#include <algorithm>
#include <sstream>

#include "maze_gen.h"


Field::Field(int w, int h, int start, int finish)
    : start_(start)
    , finish_(finish)
{
    if (w < 3 && h < 3){
        w_ = 3;
        h_ = 3;
    }
    else {
        w_ = w;
        h_ = h;
    }
    
    bool walls[4];
    for (int y = 0; y < h_; y++)
        for (int x = 0; x < w_; x++) {
            walls[0] = (y == 0);
            walls[1] = (x == w_ - 1);
            walls[2] = (y == h_ - 1);
            walls[3] = (x == 0);
            Cell::CellType c_type;
            if (x == 0 && y == start_) 
                c_type = Cell::ctStart;
            else if (x == w_ - 1 && y == finish_)
                c_type = Cell::ctFinish;
            else
                c_type = Cell::ctNormal;

            Cell cell(x, y, c_type, walls);
            cells_.push_back(cell);
        }

    path_ = std::make_unique<Path>(this, Path::ptMain, &cells_[w_ * start]);
}


Cell& Field::get_cell(int x, int y) {
    assert(x >= 0 && y >= 0 && x < w_ && y < h_);

    return cells_[y * w_ + x];
}


bool Field::trace_route() {
    // Создать путь
    if (!path_->create()) {
        std::cout << "Path could not be created.\n";
        return false;
    }


    // Создать ветвления
    std::vector<Cell*> free_cells;
    int deltas[4][2] = {
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0}
    };
    int invert_dir[4] = {2, 3, 0, 1};

    // 1. Выбираем все ячейки пути из которых можно создать ветвление.
    // Проходимся по всем ячейкам пути
    for (int i = 0; i < path_->get_cells_size(); i++) {
        Cell* cell = path_->get_cell(i);
        // Проходимся по каждой стороне ячейки
        for (int w = 0; w < 4; w++) {
            // Если стенка то переходим к следующей стороне
            if (cell->walls(static_cast<Cell::CellDirection>(w)))
                continue;

            // Иначе добавляем данную ячейку в список доступных ячеек
            free_cells.push_back(cell);
            break;
        }
    }
    
    // 2. Случайно выбираем часть ячеек из списка доступных для ветвления для начала нового ветвления. Создаем из них список новых ветвлений.
    for (int r = 0; r < free_cells.size(); r++) {
        // Получаем случайную ячейку из списка доступных ячеек
        Cell* path_cell = free_cells[rand() % free_cells.size()];
        // Проверяем каждую из сторон
        for (int w = 0; w < 4; w++) {   
            if (path_cell->walls(static_cast<Cell::CellDirection>(w)))
                continue;

            Cell* next_cell = &(get_cell(path_cell->x() + deltas[w][0], path_cell->y() + deltas[w][1]));
            // Если нашли доступное направление, то сдвигаемся на ячейку по этому направлению и создаем ветвление со стартом в этой ячейке,
            if (next_cell->path() == nullptr && !next_cell->walls(static_cast<Cell::CellDirection>(invert_dir[w]))) {
                // Связываем ячейку пути, с которого стартовало ветвление с самим ветвлением
                Fork fork(this, Path::ptFork, Fork::ForkPair(path_cell, next_cell));
                // добавляем его в список путей доступных для ветвления,
                forks_.push_back(std::move(fork));
                break;                
            }
        }
    }

    int forks_count = forks_.size();
    // Пока количесво доступных ветвлений больше 0
    while (forks_count > 0) {
        forks_count = 0;
        // Строим каждое ветвление по однуму шагу
        for (auto &fork : forks_) {
            // Если смогли построить ветвление, то увеличиваем количество доступных ветвлений
            if (fork.step())
                forks_count++;
        }
    }
    
    // Зачистить непривязанные ячейки
    // clear();

    return true;
}


// void Field::clear() {
//     for (int y = 0; y < h_; y++)
//         for (int x = 0; x < w_; x++) {
//             Cell& cell = get_cell(x, y);

//             if (cell.path() == nullptr) {
//                 cell.set_wall(Cell::cdTop, true);
//                 cell.set_wall(Cell::cdRight, true);
//                 cell.set_wall(Cell::cdBottom, true);
//                 cell.set_wall(Cell::cdLeft, true);
//             }
//         }
// }


int Field::get_cell_pos(int x, int y) { // Получение позиции в ячейки в пути
    assert(x >= 0 && y >= 0 && x < w_ && y < h_);

    Cell& cell = cells_[y * w_ + x];
    // Получить путь, которому принадлежит ячейка
    Path* path = cell.path();
    if (path == nullptr)
        return -1;

    return path_->get_cell_id(&cell);
}


// Field::operator std::string() {
//     std::ostringstream ss;

//     ss << w_ << " " << h_ << "\n";

//     for (int y = 0; y < h_; y++) {
//         for (int x = 0; x < w_; x++) {
//             Cell& cell = cells_[y * w_ + x];
//             switch (cell.type()) {
//                 case Cell::ctNormal:
//                     if (cell.path() == nullptr)
//                         ss << "N ";
//                     else if (cell.path() == &(*path_))
//                         // ss << pathes_[0].get_cell_id(&cell) % 10 << ' ';
//                         ss << get_cell_pos(x, y) % 10 << ' ';
//                         // ss << "* ";
//                     else
//                         ss << "F ";
//                     break;
                
//                 case Cell::ctStart:
//                     ss << "S ";
//                     break;
                
//                 case Cell::ctFinish:
//                     ss << "F ";
//                     break;
//             }
//         }
//         ss << "\n";
//     }

//     ss << std::string(*path_) << "\n";

//     for (auto& f: forks_)
//         ss << std::string(f) << "\n";

//     return ss.str();
// }


bool Path::create() {
    assert(field_ != nullptr);

    int deltas[4][2] = {
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0}
    };
    std::vector<int> dirs;
    std::vector<int> free_dirs;
    // Создаем путь
    // 1. Начинаем со стартовой ячейки, устанавливаем её первым элементом пути.
    Cell* curr = cells_[0];

    while (true) {
        free_dirs.clear();

        // 3. Проверяем все направления ячейки:
        //   1. Если направление указывает на ячейку, которая принадлежит пути, то ставим стену между ячейками и устанавливаем в обоих ячейках данное направление, как не доступное.
        // Проверяем верхнюю ячейку, получаем из field ячейку, у которой y текущей ячейки больше на 1
        for (int w = 0; w < 4; w++) {
            Cell::CellDirection cell_dir = static_cast<Cell::CellDirection>(w);
            if (curr->walls(cell_dir))
                continue;

            Cell& cell = field_->get_cell(curr->x() + deltas[w][0], curr->y() + deltas[w][1]);
            if (cell.path() != nullptr) {
                // Находим позицию текущей ячейки в векторе ячеек пути
                auto self = std::find(cells_.begin(), cells_.end(), curr);
                // Если мы не первая ячейка и проверяемая ячейка не является предудыщей, то ставим стенку
                if (self != cells_.begin() && self - 1 != std::find(cells_.begin(), cells_.end(), &cell)) {
                    curr->set_wall(cell_dir, true);
                }
            }
            else
                free_dirs.push_back(w);
        }

        //   2. Если достигнута ячейка выхода, завершаем построение пути.
        if (curr->type() == Cell::ctFinish)
            break; 

        //   3. Если нет доступных направлений, то убираем у ячейки принадлежность к пути, возвращаемся на предыдущую ячейку и указываем данное направление как не доступное.
        if (0 == free_dirs.size()) {
            unbind(curr);
            curr = cells_.back();
            curr->set_wall(static_cast<Cell::CellDirection>(dirs.back()), true);
            dirs.pop_back();
            continue;
        }

        // 4. Случайно выбираем направление следующего шага из доступных.
        dirs.push_back(free_dirs[rand() % free_dirs.size()]);
        curr = &(field_->get_cell(curr->x() + deltas[dirs.back()][0], curr->y() + deltas[dirs.back()][1]));
        bind(curr);


        //   2. Иначе переходим к 3.4 шагу.
    }

    return true;
}


int Path::get_cell_id(Cell* cell) {
    // Получаем итератор ячейки в пути
    auto cpos = std::find(cells_.begin(), cells_.end(), cell);
    // Если ячейка не найдена
    if (cpos == cells_.end()) {
        std::cout << "cell not found\n";
        return -1;
    }
    
    // Возвращаем разность между началом списка ячеек и интератором ячейки в пути
    return std::distance(cells_.begin(), cpos);
}


bool Fork::step() {
    std::vector<int> free_dirs;
    int deltas[4][2] = {
        {0, -1},
        {1, 0},
        {0, 1},
        {-1, 0}
    };
    int invert_dir[4] = {2, 3, 0, 1};
    Cell* curr = cells_.back();
    free_dirs.clear();
    
    // Проверяем все направления ячейки:
    //   1. Если направление указывает на ячейку, которая принадлежит пути, то ставим стену между ячейками.
    // Проверяем верхнюю ячейку, получаем из field ячейку, у которой y текущей ячейки больше на 1
    for (int w = 0; w < 4; w++) {
        Cell::CellDirection cell_dir = static_cast<Cell::CellDirection>(w);
        if (curr->walls(cell_dir))
            continue;

        Cell& cell = field_->get_cell(curr->x() + deltas[w][0], curr->y() + deltas[w][1]);
        if (cell.path() != nullptr) {
            // Находим позицию текущей ячейки в векторе ячеек пути
            auto self = std::find(cells_.begin(), cells_.end(), curr);
            // Если мы не первая ячейка и проверяемая ячейка не является предудыщей, то ставим стенку
            // Но мы первая ячейка первая, и проверяемая ячейка не является точкой пути
            // if (self != forks[p].cells_.begin() && self - 1 != std::find(forks[p].cells_.begin(), forks[p].cells_.end(), &cell) && 
            //     std::find_if(fork_starts.begin(), fork_starts.end(), [&frk = forks[p], &cl = cell](ForkStart& p)->bool {return p.first == &cl && p.second == &frk;}) == fork_starts.end())
            if ((self != cells_.begin() && self - 1 != std::find(cells_.begin(), cells_.end(), &cell))
                || (self == cells_.begin() && path_cell_ != &cell))
                // && std::find_if(fork_starts.begin(), fork_starts.end(), [&frk = this, &cl = cell](ForkStart& p)->bool {return path_cell == &cl && cells_[0] == &frk;}) == fork_starts.end()))
                curr->set_wall(cell_dir, true);
        }
        else if (!cell.walls(static_cast<Cell::CellDirection>(invert_dir[w])))
            free_dirs.push_back(w);
    }

    //   2. Если нет доступных направлений, то говорим, что данное вентвление уже построенно
    if (0 == free_dirs.size()) {           
        return false;
    }

    // 1. Берем случайное доступное направление для текущей ячейки.
    //  Указываем ячейке по выбранному направлению,
    //  что она принадлежит ответвлению и переходим в неё.
    int dir = free_dirs[rand() % free_dirs.size()];
    curr = &(field_->get_cell(curr->x() + deltas[dir][0], curr->y() + deltas[dir][1]));
    bind(curr);

    return true;
}


void Path::bind(Cell* cell) {
    assert(std::find(cells_.begin(), cells_.end(), cell) == cells_.end());
    assert(cell != nullptr);

    cells_.push_back(cell);
    cell->set_path(this);
}


void Path::unbind(Cell* cell) {
    auto c = std::find(cells_.begin(), cells_.end(), cell);
    assert(c != cells_.end());

    cells_.erase(c);
    cell->set_path();
}


// Path::operator std::string() {
//     std::ostringstream ss;

//     for (int i=0; i<cells_.size(); i++) {
//         ss << i << " @ (";
//         ss << cells_[i]->x() << ',' << cells_[i]->y() << ")\n";
//     }

//     return ss.str();
// }

