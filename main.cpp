#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>

const double DOUBLE_MAX = std::numeric_limits<double>::max();

class FloydWarshall {
public:
    FloydWarshall(const std::string& file_name) {
        InitEdges("input/" + file_name);
        InitDistanceMatrix();
    }

    void GenerateDistanceMatrix() {
        for_each_vertex([this](const std::string& k) {
            for_each_vertex_pair([this, k](const std::string& i, const std::string& j) {
                double i_k = dist_matrix_[{i, k}];
                double k_j = dist_matrix_[{k, j}];
                double i_j = dist_matrix_[{i, j}];

                dist_matrix_[{i, j}] = std::min(i_k + k_j, i_j);
                });
            });
    }

    void PrintDistances(const std::string& file_name) {
        std::ofstream file_out("output/" + file_name);
        for_each_vertex_pair([this, &file_out](const std::string& lhs, const std::string& rhs) {
            double dist = dist_matrix_[{lhs, rhs}];
            if (lhs != rhs) {
                file_out << "from: " << lhs << " to: " << rhs;
                if (dist == DOUBLE_MAX) {
                    file_out << " - INF\n";
                }
                else {
                    file_out << " - " << dist << "\n";
                }
            }
            });
        file_out.close();
    }

private:
    std::map<std::pair<std::string, std::string>, double> dist_matrix_;
    std::vector<std::string> vertices_;

    void InitDistanceMatrix() {
        for_each_vertex_pair([this](const std::string& lhs, const std::string& rhs) {
            if (lhs == rhs) {
                dist_matrix_[{lhs, lhs}] = 0;
            }
            else if (!dist_matrix_.count({ lhs, rhs })) {
                dist_matrix_[{lhs, rhs}] = DOUBLE_MAX;
            }
            });
    }

    void InitEdges(const std::string& file_name) {
        std::ifstream input_file(file_name);
        std::string lhs, rhs;
        double w;

        while (input_file >> lhs >> rhs >> w) {
            dist_matrix_[{lhs, rhs}] = w;
            if (std::find(vertices_.begin(), vertices_.end(), lhs) == vertices_.end()) {
                vertices_.push_back(lhs);
            }
            if (std::find(vertices_.begin(), vertices_.end(), rhs) == vertices_.end()) {
                vertices_.push_back(rhs);
            }
        }

        input_file.close();
    }

    template<typename Func>
    void for_each_vertex_pair(Func func) const {
        for (const auto& lhs : vertices_) {
            for (const auto& rhs : vertices_) {
                func(lhs, rhs);
            }
        }
    }

    template<typename Func>
    void for_each_vertex(Func func) const {
        for (const auto& v : vertices_) {
            func(v);
        }
    }
};

void TestFile(std::ofstream& out, const std::string& file_name) {
    auto t_start = std::chrono::high_resolution_clock::now();
    FloydWarshall FW(file_name);
    FW.GenerateDistanceMatrix();
    auto t_stop = std::chrono::high_resolution_clock::now();
    auto t_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_stop - t_start);
    out << "Time duration: " << t_duration.count() / 1000.0 << " ms\n";
}

void RunTimeTest() {
    std::ofstream log_file("log.txt");

    TestFile(log_file, "test1.txt");
    TestFile(log_file, "test2.txt");
    TestFile(log_file, "test3.txt");

    log_file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: <FileNameToLoad> <FileNameToSave>" << std::endl;
        return 1;
    }
    FloydWarshall FW(argv[1]);
    FW.GenerateDistanceMatrix();
    FW.PrintDistances(argv[2]);

    RunTimeTest();
    
    std::cout << "Succes!\n";
    return 0;
}