#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <string>
#include <utility>

const double DOUBLE_MAX = std::numeric_limits<double>::max();

class FloydWarshall {
public:
    FloydWarshall(const std::string& file_name) {
        InitEdges(file_name);
        InitDistanceMatrix();
    }

    void GenerateDistanceMatrix() {
        for_each_vertex([this](int k) {
            for_each_vertex_pair([this, k](int i, int j) {
                double ik = dist_matrix_.count({ i, k }) ? dist_matrix_[{i, k}] : DOUBLE_MAX;
                double kj = dist_matrix_.count({ k, j }) ? dist_matrix_[{k, j}] : DOUBLE_MAX;
                double ij = dist_matrix_.count({ i, j }) ? dist_matrix_[{i, j}] : DOUBLE_MAX;

                if (ik != DOUBLE_MAX && kj != DOUBLE_MAX && ik + kj < ij) {
                    dist_matrix_[{i, j}] = ik + kj;
                }
                });
            });
    }

    void PrintDistances(const std::string& file_name) {
        std::ofstream file_out(file_name);
        for_each_vertex_pair([this, &file_out](int lhs, int rhs) {
            double dist = dist_matrix_[{lhs, rhs}];
            if (dist == DOUBLE_MAX) {
                file_out << "from: " << lhs << " to: " << rhs << " - " << "INF" << "\n";
            }
            else {
                file_out << "from: " << lhs << " to: " << rhs << " - " << dist << "\n";
            }
            });
        file_out.close();
    }

private:
    std::map<std::pair<int, int>, double> dist_matrix_;
    int max_vertex_ = 0;

    void InitDistanceMatrix() {
        for_each_vertex_pair([this](int lhs, int rhs) {
            if (lhs == rhs) {
                dist_matrix_[{lhs, lhs}] = 0;
            }
            else if (dist_matrix_.find({lhs, rhs}) == dist_matrix_.end()) {
                dist_matrix_[{lhs, rhs}] = DOUBLE_MAX;
            }
        });
    }

    void InitEdges(const std::string& file_name) {
        std::ifstream input_file(file_name);

        for_each_edge(input_file, [this](int lhs, int rhs, double w) {
            dist_matrix_[{lhs, rhs}] = w;
            max_vertex_ = std::max({ max_vertex_, lhs, rhs });
            });
    }

    template<typename Func>
    void for_each_vertex_pair(Func func) const {
        for (int lhs = 0; lhs <= max_vertex_; ++lhs) {
            for (int rhs = 0; rhs <= max_vertex_; ++rhs) {
                func(lhs, rhs);
            }
        }
    }

    template<typename Func>
    void for_each_vertex(Func func) const {
        for (int v = 0; v <= max_vertex_; ++v) {
            func(v);
        }
    }

    template<typename Func>
    void for_each_edge(std::ifstream& input_file, Func func) {
        int n;
        input_file >> n;

        for (int i = 0; i < n; ++i) {
            int lhs, rhs;
            double w;
            input_file >> lhs >> rhs >> w;
            func(lhs, rhs, w);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <FileNameToLoad> <FileNameToSave>" << std::endl;
        return 1;
    }
    FloydWarshall FW(argv[1]);
    FW.GenerateDistanceMatrix();
    FW.PrintDistances(argv[2]);
    return 0;
}

/*
Написать программу, которая будет применять указанный в варианте алгоритм к входным графам, заданным в файлах.
То, как описаны графы в файлах, остаётся на выбор студента.
Формат записи результата работы алгоритма в файл не регламентирован и устанавливается студентом.
В отчёте для каждого варианта требуется:
* указать сложность алгоритма и доказать, что она именно такая;
* сравнение работы алгоритма на различных допустимых входных данных:
на каких графах алгоритм работает лучше, на каких – хуже, на каких – вообще не работает;
* объяснить почему был выбран тот или иной способ представления графов в программе.
 
На вход программе подаётся взвешенный орграф.
*/