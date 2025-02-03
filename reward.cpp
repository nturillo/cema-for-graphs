#include <cstdint>
#include <queue>
#include <vector>
#include <iostream>
#include <string>

int** compositions = nullptr;
int* num_compositions = nullptr;

extern "C" {
    double edge_reward(int n, int8_t* A) {
        double reward = 0;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                reward += A[i * n + j];
            }
        }
        return reward;
    }

    inline int choose(int n, int k) {
        if (k == 0) return 1;
        if (n == 0) return 0;
        return choose(n-1, k-1) + choose(n-1, k);
    }

    inline void generate_weak_compositions(int n, int k, std::vector<int> dividers, int ptr, int* compositions, int& num_compositions) {
        int max = n + k - 1;
        if (ptr == k - 1) {
            compositions[num_compositions * k + 0] = dividers[0];
            for (int i = 1; i < k-1; i++) {
                compositions[num_compositions * k + i] = dividers[i] - dividers[i-1] - 1;
            }
            compositions[num_compositions * k + k-1] = max - dividers[k-2] - 1;
            num_compositions++;
            return;
        }

        int ptrs_left = k - ptr - 2;
        for (int i = ptr == 0 ? 0 : dividers[ptr-1] + 1; i < max - ptrs_left; i++) {
            dividers[ptr] = i;
            generate_weak_compositions(n, k, dividers, ptr+1, compositions, num_compositions);
        }
    }

    void delete_compositions(int num_vertices) {
        for (int i = 0; i < num_vertices-1; i++) {
            delete[] compositions[i];
        }
        delete[] compositions;
        delete[] num_compositions;
    }

    void populate_compositions(int num_vertices) {
        // populate compositions and num_compositions
        compositions = new int*[num_vertices-1];
        num_compositions = new int[num_vertices-1];

        for (int i = 0; i < num_vertices-1; i++) {
            // num of weak compositions of i+1 into num_vertices parts
            // is (i+1) + (num_vertices) - 1 choose (num_vertices) - 1
            num_compositions[i] = choose(i+1+num_vertices-1, num_vertices-1);
            compositions[i] = new int[num_compositions[i] * (num_vertices)];
            std::vector<int> dividers(num_vertices-1);
            int nc = 0;
            generate_weak_compositions(i+1, num_vertices, dividers, 0, compositions[i], nc);
        }
    }

    void print_compositions(int num_vertices) {
        for (int i = 0; i < num_vertices-1; i++) {
            std::cout << "Compositions of " << i+1 << " into " << num_vertices << " parts:\n";
            for (int j = 0; j < num_compositions[i]; j++) {
                std::cout << "[";
                for (int k = 0; k < num_vertices; k++) {
                    std::cout << compositions[i][j * (num_vertices) + k] << " ";
                }
                std::cout << "]\n";
            }
            std::cout << "\n";
        }
    }

    inline bool check_divisor(int n, int8_t* A, int divisor[], int q) {
        // run dhars burning algorithm
        int temp_divisor[n];
        for (int i = 0; i < n; i++) {
            temp_divisor[i] = divisor[i];
        }

        while (true) {
            int burning[n];
            for (int i = 0; i < n; i++) {
                burning[i] = 0;
            }
            burning[q] = 1;
            
            // begin a fire
            bool changed = true;
            while(changed) {
                changed = false;
                for (int i = 0; i < n; i++) {
                    if (burning[i] == 1) continue;
                    int sum = 0;
                    for (int j = 0; j < n; j++) {
                        sum += A[i * n + j] * burning[j];
                    }
                    if (sum > temp_divisor[i]) {
                        burning[i] = 1;
                        changed = true;
                    }
                }
            }

            // if the whole graph is burning, player two wins
            bool everything_burning = true;
            for (int i = 0; i < n; i++) {
                if (burning[i] == 0) {
                    everything_burning = false;
                    break;
                }
            }
            if (everything_burning) return true;

            // update the divisor
            for (int i = 0; i < n; i++) {
                if (burning[i]) continue;
                for (int j = 0; j < n; j++) {
                    int fire = A[i * n + j] * burning[j];
                    temp_divisor[j] += fire;
                    temp_divisor[i] -= fire;
                } 
            }

            // if q is now out of debt, player one wins
            if (temp_divisor[q] >= 0) return false;
        }

    }

    inline bool check_gonality(int n, int8_t* A, int gon, int num_compositions, int* compositions) {
        // checks if the gonality is less than or equal to gon
        // if there is a divisor of degree gon such that player one wins no matter where player two places their -1, return true
        int divisor[n];

        for (int i = 0; i < num_compositions; i++) {

            for (int j = 0; j < n; j++) {
                divisor[j] = compositions[i * n + j];
            }

            bool player_two_wins = false;
            for (int j = 0; j < n; j++) {
                if (divisor[j] != 0) continue;
                divisor[j]--;
                if (check_divisor(n, A, divisor, j)) {
                    player_two_wins = true;
                    break;
                }
                divisor[j]++;
            }
            if (!player_two_wins) return true;
        }
        return false;
    }

    int gonality(int n, int8_t* A) {
        // run binary search to find the gonality
        int left = 1;
        int right = n-1;
        int works[n-1];
        for (int i = 0; i < n-1; i++) {
            works[i] = 0;
        }

        while (right - left > 1) {
            int mid = (left + right) / 2;
            if (check_gonality(n, A, mid, num_compositions[mid-1], compositions[mid-1])) {
                works[mid] = 1;
                right = mid;
            } else {
                works[mid] = 2;
                left = mid;
            }
        }
        if (works[left] == 1) return left;
        if (works[left] == 2) return right;
        if (check_gonality(n, A, left, num_compositions[left-1], compositions[left-1])) return left;
        return right;
    }

    inline int genus(int n, int8_t* A) {
        // count the number of edges
        int num_edges = 0;
        for (int i = 0; i < n; i++) {
            for (int j = i+1; j < n; j++) {
                num_edges += A[i * n + j];
            }
        }

        return num_edges - n + 1;
    }

    bool connected(int n, int8_t* A) {
        int visited[n];
        for (int i = 0; i < n; i++) {
            visited[i] = 0;
        }

        std::queue<int> q;
        q.push(0);
        while (!q.empty()) {
            int node = q.front();
            q.pop();
            visited[node] = 1;
            for (int i = 0; i < n; i++) {
                if (A[node * n + i] && !visited[i]) {
                    q.push(i);
                }
            }
        }

        for (int i = 0; i < n; i++) {
            if (!visited[i]) return false;
        }
        return true;
    }

    double genus_minus_gonality_reward(int n, int8_t* A) {
        if (!connected(n, A)) return -100.0;
        int g = genus(n, A);
        if (g <= 12) return -50.0;
        return gonality(n, A) - ( (double) g + 3) / 2;
    }
}

//
//int main(int argc, char** argv) {
//    int n = 6;
//    int8_t A[n * n] = {0, 0, 0, 0, 1, 1,
//                        0, 0, 0, 0, 1, 0,
//                        0, 0, 0, 0, 0, 1,
//                        0, 0, 0, 0, 1, 1,
//                        1, 1, 0, 1, 0, 1,
//                        1, 0, 1, 1, 1, 0};
//
//    populate_compositions(n);
//    //print_compositions(n);
//
//    std::cout << "Gonality of graph on " << n << " vertices: " << gonality(n, A) << "\n";
//}
//