#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>

const double DOUBLE_MAX = std::numeric_limits<double>::max();

class FloydWarshall {
public:
    FloydWarshall(const std::string& file_name) {
        InitEdges("input/" + file_name);
        InitDistanceMatrix();
    }

    void GenerateDistanceMatrix() {
        for_each_vertex([this](int k) {
            for_each_vertex_pair([this, k](int i, int j) {
                double i_k = dist_matrix_[{i, k}];
                double k_j = dist_matrix_[{k, j}];
                double i_j = dist_matrix_[{i, j}];
                
                dist_matrix_[{i, j}] = std::min(i_k + k_j, i_j);
                });
            });
    }

    void PrintDistances(const std::string& file_name) {
        std::ofstream file_out("output/" + file_name);
        for_each_vertex_pair([this, &file_out](int lhs, int rhs) {
            double dist = dist_matrix_[{lhs, rhs}];
            if (lhs != rhs) {
                if (dist == DOUBLE_MAX) {
                    file_out << "from: " << lhs << " to: " << rhs << " - " << "INF" << "\n";
                }
                else {
                    file_out << "from: " << lhs << " to: " << rhs << " - " << dist << "\n";
                }
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
            else if (!dist_matrix_.count({lhs, rhs})) {
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

void RunTimeTest() {
    std::ofstream log_file("log.txt");

    FloydWarshall FW1("test1.txt");
    auto t1_start = std::chrono::high_resolution_clock::now();
    FW1.GenerateDistanceMatrix();
    auto t1_stop = std::chrono::high_resolution_clock::now();
    FW1.PrintDistances("test1-ouput.txt");
    auto t1_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1_stop - t1_start);
    log_file << "Time duration 10 edges: " << t1_duration.count() << "ms\n";


    FloydWarshall FW2("test2.txt");
    auto t2_start = std::chrono::high_resolution_clock::now();
    FW2.GenerateDistanceMatrix();
    auto t2_stop = std::chrono::high_resolution_clock::now();
    FW2.PrintDistances("test2-ouput.txt");
    auto t2_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2_stop - t2_start);
    log_file << "Time duration 100 edges: " << t2_duration.count() << "ms\n";

    FloydWarshall FW3("test3.txt");
    auto t3_start = std::chrono::high_resolution_clock::now();
    FW3.GenerateDistanceMatrix();
    auto t3_stop = std::chrono::high_resolution_clock::now();
    FW3.PrintDistances("test3-ouput.txt");
    auto t3_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t3_stop - t3_start);
    log_file << "Time duration 1000 edges: " << t3_duration.count() << "ms\n";

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
    
    return 0;
}