#include <iostream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <numeric>
#include <iomanip>
#include <string>
#include <functional>

using namespace std;

// ANSI escape codes for color in Code::Blocks
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

// Class to represent a Subject
class Subject {
public:
    string name;
    int difficulty;
    int hoursAllocated;
    int minutesAllocated;
    int deadline;
    int examDay; // Added to store the exam day

    Subject(string n, int d, int dl, int ed) : name(n), difficulty(d), hoursAllocated(0), minutesAllocated(0), deadline(dl), examDay(ed) {}
};

// Function to get user input for subjects with validation
vector<Subject> inputSubjects(int n) {
    vector<Subject> subjects;
    cout << YELLOW << "\nEnter subject details:\n" << RESET;
    for (int i = 0; i < n; i++) {
        string name;
        int difficulty, deadline;

        cout << "Subject " << (i + 1) << ": ";
        cin >> name;

        // Input validation for difficulty
        do {
            cout << "Difficulty (1-5): ";
            cin >> difficulty;
            if (difficulty < 1 || difficulty > 5) {
                cout << RED << "Invalid input! Please enter a difficulty level between 1 and 5.\n" << RESET;
            }
        } while (difficulty < 1 || difficulty > 5);

        // Input for deadline
        cout << "Deadline (in days until the exam): ";
        cin >> deadline;

        // Calculate exam day as deadline + 1
        int examDay = deadline + 1;

        subjects.emplace_back(name, difficulty, deadline, examDay);
    }
    return subjects;
}

// Function to prioritize subjects based on difficulty (Greedy Algorithm)
void prioritizeSubjects(vector<Subject>& subjects) {
    sort(subjects.begin(), subjects.end(), [](Subject& a, Subject& b) {
        return a.difficulty > b.difficulty; // Sort by difficulty
    });
}

// Function to assign study hours and minutes ensuring each subject gets at least one hour
void assignStudyHoursAndMinutes(vector<Subject>& subjects, int totalHours) {
    int totalMinutes = totalHours * 60; // Convert hours to minutes
    int totalDifficulty = accumulate(subjects.begin(), subjects.end(), 0, [](int sum, const Subject& s) {
        return sum + s.difficulty;
    });

    // Calculate base minutes for each subject
    for (auto& subject : subjects) {
        subject.minutesAllocated = (subject.difficulty * totalMinutes) / totalDifficulty;
        subject.hoursAllocated = subject.minutesAllocated / 60; // Convert back to hours
        subject.minutesAllocated = subject.minutesAllocated % 60; // Remaining minutes
    }

    // Ensure all total allocated time is used
    int totalAllocated = 0;
    for (const auto& subject : subjects) {
        totalAllocated += subject.hoursAllocated * 60 + subject.minutesAllocated;
    }

    // Adjust to use all available minutes
    if (totalAllocated < totalMinutes) {
        for (auto& subject : subjects) {
            if (totalAllocated < totalMinutes) {
                subject.minutesAllocated++;
                totalAllocated++;
            }
        }
    }
}

// Function to implement the Knapsack algorithm
int knapsackAllocate(const vector<int>& difficulty, const vector<int>& hoursPerSubject, int totalHours) {
    int n = difficulty.size();
    vector<vector<int>> dp(n + 1, vector<int>(totalHours + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= totalHours; w++) {
            if (hoursPerSubject[i - 1] <= w) {
                dp[i][w] = max(dp[i - 1][w ], dp[i - 1][w - hoursPerSubject[i - 1]] + difficulty[i - 1]);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }
    return dp[n][totalHours];
}

// Function to display menu
void displayMenu() {
    cout << CYAN << "\n------------------------------------------------------------- " << BOLD << "Personalized Study Plan Optimization" << RESET << CYAN << " ---------------------------------------------------------\n";
    cout << "1. Enter Subjects\n";
    cout << "2. Optimize Study Plan\n";
    cout << "3. Use Hungarian Algorithm for Study Optimization\n";
    cout << "4. Use Knapsack Algorithm for Optimal Study Allocation\n";
    cout << "5. Exit\n";
    cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    cout << "Select an option: " << RESET;
}

// Function for Hungarian Algorithm for assignment problem
vector<int> hungarianAlgorithm(vector<vector<int>>& cost) {
    int n = cost.size();
    vector<int> u(n), v(n), p(n), way(n);
    for (int i = 1; i < n; ++i) {
        p[0] = i;
        vector<int> minv(n, INT_MAX);
        vector<bool> used(n, false);
        int j0 = 0;
        do {
            used[j0] = true;
            int i0 = p[j0], delta = INT_MAX, j1;
            for (int j = 1; j < n; ++j) {
                if (!used[j]) {
                    int cur = cost[i0][j] - u[i0] - v[j];
                    if (cur < minv[j]) {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta) {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            }
            for (int j = 0; j < n; ++j) {
                if (used[j]) {
                    u[p[j]] += delta;
                    v[j] -= delta;
                } else {
                    minv[j] -= delta;
                }
            }
            j0 = j1;
        } while (p[j0] != 0);
        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }
    return p;
}

// Function to generalize task order scheduling
void generalizeTaskOrder(vector<Subject>& subjects) {
    // Sort subjects based on deadlines (earliest first)
    sort(subjects.begin(), subjects.end(), [](const Subject& a, const Subject& b) {
        return a.deadline < b.deadline; // Sort by deadline
    });

    cout << "\n" << BOLD << "Scheduled Tasks based on Deadlines:\n" << RESET;
    cout << left << setw(25) << "Subject" << setw(20) << "Deadline" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
    for (const auto& subject : subjects) {
        cout << left << setw(25) << subject.name << setw(20) << subject.deadline << " days" << endl;
    }
}

// Function to create a cost matrix based on the difficulty-deadline ratio
void createCostMatrix(vector<vector<int>>& costMatrix, const vector<Subject>& subjects) {
    int n = subjects.size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (subjects[j].deadline == 0) {
                costMatrix[i][j] = INT_MAX;  // Prevent division by zero
            } else {
                costMatrix[i][j] = subjects[i].difficulty / subjects[j].deadline; // Use difficulty-deadline ratio
            }
        }
    }
}

// Function to generate a daily study plan
void generateDailyStudyPlan(vector<Subject>& subjects, int totalHours) {
    int currentDay = 0;

    while (true) {
        bool allExamsDone = true;
        cout << "\n" << BOLD << "Daily Study Plan for Day " << currentDay << ":\n" << RESET;
        cout << left << setw(25) << "Subject" << setw(15) << "Allocated Hours" << setw(15) << "Allocated Minutes" << endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------\n";

        // Declare allocatedHours and allocatedMinutes outside the loop
        int totalAllocatedHours = 0; // Track total allocated hours for the day

        for (auto& subject : subjects) {
            if (subject.examDay > currentDay) {
                allExamsDone = false;

                // Calculate allocated hours and minutes based on the subject's difficulty
                int allocatedHours = subject.hoursAllocated;
                int allocatedMinutes = subject.minutesAllocated;

                // Adjust allocated hours on exam day
                if (subject.examDay == currentDay + 1) {
                    allocatedHours = max(1, allocatedHours - 1); // Ensure at least 1 hour
                }

                // Allocate more time to subjects with upcoming exams
                if (subject.examDay - currentDay <= 3) {
                    allocatedHours += 1; // Add an extra hour
                }

                // Ensure allocated hours do not exceed total available hours
                if (totalAllocatedHours + allocatedHours > totalHours) {
                    allocatedHours = totalHours - totalAllocatedHours; // Adjust to fit the remaining time
                }

                // Update total allocated hours for the day
                totalAllocatedHours += allocatedHours;

                cout << left << setw(25) << subject.name
                     << right << setw(2) << allocatedHours << " hrs "
                     << setw(2) << allocatedMinutes << " mins" << endl;
            }
        }

        cout << endl;

        // If all exams are done, exit the loop
        if (allExamsDone) {
            break;
        }

        currentDay++;
    }
}

// Main function
int main() {
    vector<Subject> subjects;
    int n = 0;
    int totalHours = 0; // Total study hours input

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                cout << YELLOW << "Enter number of subjects: " << RESET;
                cin >> n;
                subjects = inputSubjects(n);
                prioritizeSubjects(subjects);
                cout << GREEN << "Subjects entered successfully!\n " << RESET;

                cout << YELLOW << "Enter available study hours per day: " << RESET;
                cin >> totalHours; // Ask for total hours per day
                while (totalHours <= 0) {
                    cout << RED << "Invalid input! Please enter a positive number of available study hours.\n" << RESET;
                    cin >> totalHours;
                }
                break;
            }
            case 2: {
                // Assign study hours based on difficulty
                assignStudyHoursAndMinutes(subjects, totalHours);

                cout << "\n" << BOLD << "Assigned Study Hours:\n" << RESET;
                cout << left << setw(25) << "Subject" << setw(15) << "Allocated Hours" << setw(15) << "Allocated Minutes" << endl;
                cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
                for (const auto& subject : subjects) {
                    cout << left << setw(25) << subject.name
                         << right << setw(2) << subject.hoursAllocated << " hrs "
                         << setw(2) << subject.minutesAllocated << " mins" << endl;
                }
                break;
            }
            case 3: {
                cout << "\n" << BOLD << "Optimizing study allocation using the Hungarian Algorithm...\n" << RESET;

                int m = subjects.size(); // Number of subjects
                vector<vector<int>> cost(m + 1, vector<int>(m + 1, 0));

                // Calculate costs based on difficulty and deadlines
                createCostMatrix(cost, subjects);

                vector<int> assignment = hungarianAlgorithm(cost);
                cout << "\n" << BOLD << "Optimal assignment based on costs:\n" << RESET;
                for (int i = 1; i <= m; i++) {
                    cout << GREEN << "Subject " << subjects[i - 1].name << " -> Task " << assignment[i] << RESET << endl;
                }
                break;
            }
            case 4: {
                cout << YELLOW << "Using Knapsack Algorithm for optimal study allocation...\n" << RESET;

                vector<int> difficulty;
                vector<int> hoursPerSubject;

                for (const auto& subject : subjects) {
                    difficulty.push_back(subject.difficulty);
                    hoursPerSubject.push_back(subject.hoursAllocated + (subject.minutesAllocated > 0 ? 1 : 0)); // Count each hour, round up for minutes
                }

                int maxDifficulty = knapsackAllocate(difficulty, hoursPerSubject, totalHours);
                cout << GREEN << "Maximum achievable difficulty with the available hours: " << maxDifficulty << RESET << endl;
                break;
            }
            case 5:
                cout << GREEN << "Exiting...\n" << RESET;
                return 0;
            default:
                cout << RED << "Invalid choice. Please try again.\n" << RESET;
        }

        // Generate daily study plan
        generateDailyStudyPlan(subjects, totalHours);
    }

    return 0;
}
