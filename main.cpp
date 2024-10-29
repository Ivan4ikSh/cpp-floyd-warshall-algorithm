#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <string>
#include <vector>
#include <exception>

const double DOUBLE_MAX = std::numeric_limits<double>::max();

class CycleDetectedException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Cycle detected in the graph!";
    }
};

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

                if (i_k < DOUBLE_MAX && k_j < DOUBLE_MAX && i_k + k_j < i_j) {
                    dist_matrix_[{i, j}] = i_k + k_j;
                    next_vertex_[{i, j}] = next_vertex_[{i, k}];
                }
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
                    file_out << " - " << dist << " via path: " << GetPath(lhs, rhs) << "\n";
                }
            }
        });
        file_out.close();
    }

private:
    std::map<std::pair<std::string, std::string>, double> dist_matrix_;
    std::map<std::pair<std::string, std::string>, std::string> next_vertex_;
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
            next_vertex_[{lhs, rhs}] = rhs;
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

    std::string GetPath(const std::string& start, const std::string& end) const {
        if (dist_matrix_.at({ start, end }) == DOUBLE_MAX) {
            return "No path";
        }

        std::string path = start;
        std::string current = start;

        while (current != end) {
            current = next_vertex_.at({ current, end });
            if (current == end) {
                path += "-" + current;
                break;
            }
            if (path.find(current) != std::string::npos) {
                throw CycleDetectedException();
            }
            path += "-" + current;
        }

        return path;
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

    try {
        FloydWarshall FW(argv[1]);
        FW.GenerateDistanceMatrix();
        FW.PrintDistances(argv[2]);
    }
    catch (const CycleDetectedException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    RunTimeTest();
    std::cout << "Success!\n";
    return 0;
}
