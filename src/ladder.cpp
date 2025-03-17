#include "ladder.h"
#include <unistd.h>  // Add this for getcwd()

void error(string word1, string word2, string msg) {
    cerr << "Error: " << msg << " for words '" << word1 << "' and '" << word2 << "'" << endl;
}

// Calculate if edit distance between two strings is within a specified distance
bool edit_distance_within(const std::string& str1, const std::string& str2, int d) {
    // Early exit if the strings differ in length by more than d
    if (abs(int(str1.length()) - int(str2.length())) > d) {
        return false;
    }
    
    // Special case for insertions at beginning
    if (str2.length() == str1.length() + 1) {
        if (str2.substr(1) == str1) {
            return true;  // Insertion at beginning
        }
    }
    
    // Special case for deletions at beginning
    if (str1.length() == str2.length() + 1) {
        if (str1.substr(1) == str2) {
            return true;  // Deletion at beginning
        }
    }
    
    // Create a matrix for dynamic programming
    const int m = str1.length();
    const int n = str2.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));
    
    // Initialize first row and column
    for (int i = 0; i <= m; i++) {
        dp[i][0] = i;
    }
    for (int j = 0; j <= n; j++) {
        dp[0][j] = j;
    }
    
    // Fill the DP table
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (str1[i-1] == str2[j-1]) {
                dp[i][j] = dp[i-1][j-1];  // No operation required
            } else {
                dp[i][j] = 1 + min(dp[i-1][j-1],  // Replace
                               min(dp[i-1][j],    // Delete
                                   dp[i][j-1]));  // Insert
            }
            
            // Early exit if we can confirm edit distance exceeds d
            if (i == j && dp[i][j] > d) {
                return false;
            }
        }
    }
    
    return dp[m][n] <= d;
}

// Check if two words are adjacent (differ by exactly one edit operation)
bool is_adjacent(const string& word1, const string& word2) {
    // If lengths differ by more than 1, they can't be adjacent
    if (abs(int(word1.length()) - int(word2.length())) > 1) {
        return false;
    }
    
    // Case 1: Same length - must differ by exactly one character
    if (word1.length() == word2.length()) {
        int differences = 0;
        for (size_t i = 0; i < word1.length(); i++) {
            if (word1[i] != word2[i]) {
                differences++;
                if (differences > 1) {
                    return false;
                }
            }
        }
        return differences <= 1;  // Changed to allow identical words (0 differences)
    }
    
    // Case 2: word1 is longer - must be a deletion
    if (word1.length() == word2.length() + 1) {
        return is_deletion(word1, word2);
    }
    
    // Case 3: word2 is longer - must be an insertion
    if (word2.length() == word1.length() + 1) {
        return is_deletion(word2, word1);
    }
    
    return false;
}

// Helper function to check if longer_word becomes shorter_word after a single deletion
bool is_deletion(const string& longer_word, const string& shorter_word) {
    for (size_t i = 0; i < longer_word.length(); i++) {
        string test = longer_word.substr(0, i) + longer_word.substr(i + 1);
        if (test == shorter_word) {
            return true;
        }
    }
    return false;
}

// Load dictionary words from a file
void load_words(set<string>& word_list, const string& file_name) {
    // Print current directory for debugging

    
    cout << "Attempting to open: " << file_name << endl;
    ifstream file(file_name);
    if (!file) {
        cerr << "Error: Cannot open file " << file_name << endl;
        return;
    }
    
    string word;
    while (file >> word) {
        // Convert to lowercase
        for (char& c : word) {
            c = tolower(c);
        }
        word_list.insert(word);
    }
    
    file.close();
}

// Generate word ladder from begin_word to end_word
vector<string> generate_word_ladder(const string& begin_word, const string& end_word, const set<string>& word_list) {
    // Check if start and end words are the same
    if (begin_word == end_word) {
        return vector<string>();  // Return empty vector for same word
    }
    
    // Start BFS
    queue<vector<string>> ladder_queue;
    set<string> visited;
    
    // Initialize with begin_word
    vector<string> initial_ladder = {begin_word};
    ladder_queue.push(initial_ladder);
    visited.insert(begin_word);
    
    while (!ladder_queue.empty()) {
        vector<string> ladder = ladder_queue.front();
        ladder_queue.pop();
        
        string last_word = ladder.back();
        
        // Check all dictionary words
        for (const string& word : word_list) {
            if (is_adjacent(last_word, word) && visited.find(word) == visited.end()) {
                visited.insert(word);
                
                vector<string> new_ladder = ladder;
                new_ladder.push_back(word);
                
                // Check if we've reached the end word
                if (word == end_word) {
                    return new_ladder;
                }
                
                ladder_queue.push(new_ladder);
            }
        }
    }
    
    // No ladder found
    return vector<string>();
}

// Print a word ladder
void print_word_ladder(const vector<string>& ladder) {
    if (ladder.empty()) {
        cout << "No word ladder found." << endl;
        return;
    }
    
    cout << "Word ladder found: ";
    for (size_t i = 0; i < ladder.size(); i++) {
        cout << ladder[i];
        if (i < ladder.size() - 1) {
            cout << " ";
        }
    }
    cout << " " << endl;
}

// Verify word ladder implementation with test cases
void verify_word_ladder() {
    set<string> word_list;
    load_words(word_list, "words.txt");
    
    cout << "Testing word ladder generator..." << endl;
    
    auto test = [&](const string& start, const string& end, int expected_size) {
        vector<string> ladder = generate_word_ladder(start, end, word_list);
        bool passed = ladder.size() == expected_size;
        cout << start << " → " << end << ": " << (passed ? "PASSED" : "FAILED") 
             << " (expected " << expected_size << ", got " << ladder.size() << ")" << endl;
        if (!passed) {
            print_word_ladder(ladder);
        }
        return passed;
    };
    
    test("cat", "dog", 4);
    test("marty", "curls", 6);
    test("code", "data", 6);
    test("work", "play", 6);
    test("sleep", "awake", 8);
    test("car", "cheat", 4);
}
