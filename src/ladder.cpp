#include "ladder.h"
#include <unistd.h>  // Add this for getcwd()
#include <unordered_set>  // Add this for unordered_set

void error(string word1, string word2, string msg) {
    cerr << "Error: " << msg << " for words '" << word1 << "' and '" << word2 << "'" << endl;
}

// Forward declarations
bool one_deletion_away(const string& longer_word, const string& shorter_word);

// Calculate if edit distance between two strings is within a specified distance
bool edit_distance_within(const std::string& str1, const std::string& str2, int d) {
    // Early exit if the strings differ in length by more than d
    if (abs(int(str1.length()) - int(str2.length())) > d) {
        return false;
    }
    
    // For d=1 (which is the case for is_adjacent), we can use faster checks
    if (d == 1) {
        // Case 1: Same length - check for one character difference
        if (str1.length() == str2.length()) {
            int differences = 0;
            for (size_t i = 0; i < str1.length(); i++) {
                if (str1[i] != str2[i]) {
                    differences++;
                    if (differences > 1) {
                        return false;
                    }
                }
            }
            return true;  // 0 or 1 differences is within edit distance 1
        }
        
        // Case 2: Strings differ by 1 in length - check for insertion/deletion
        if (str1.length() + 1 == str2.length()) {
            // str2 is longer - check if str1 can be derived by removing one char from str2
            return one_deletion_away(str2, str1);
        }
        
        if (str1.length() == str2.length() + 1) {
            // str1 is longer - check if str2 can be derived by removing one char from str1
            return one_deletion_away(str1, str2);
        }
        
        return false;
    }
    
    // For other values of d, use dynamic programming with space optimization
    // We only need two rows of the DP table
    const int m = str1.length();
    const int n = str2.length();
    
    // Use only two rows of dp table - prev and current
    vector<int> prev(n + 1);
    vector<int> curr(n + 1);
    
    // Initialize the first row
    for (int j = 0; j <= n; j++) {
        prev[j] = j;
    }
    
    // Fill the DP table row by row
    for (int i = 1; i <= m; i++) {
        curr[0] = i;  // First column of current row
        
        for (int j = 1; j <= n; j++) {
            if (str1[i-1] == str2[j-1]) {
                curr[j] = prev[j-1];  // No operation required
            } else {
                curr[j] = 1 + min(prev[j-1],    // Replace
                              min(prev[j],      // Delete
                                  curr[j-1]));  // Insert
            }
        }
        
        // Check if we can early exit (minimum edit distance already exceeds d)
        if (i <= n && curr[i] > d) {
            // On the diagonal - if edit distance exceeds d, we can't get <= d
            return false;
        }
        
        // Swap rows for next iteration
        prev.swap(curr);
    }
    
    return prev[n] <= d;
}

// Helper function to check if removing one character from longer_word gives shorter_word
bool one_deletion_away(const string& longer_word, const string& shorter_word) {
    for (size_t i = 0; i < longer_word.length(); i++) {
        // Check if string matches when skipping this position
        bool match = true;
        for (size_t s = 0, l = 0; s < shorter_word.length(); s++, l++) {
            if (l == i) l++;  // Skip the current position in longer_word
            if (l >= longer_word.length() || shorter_word[s] != longer_word[l]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

// Check if two words are adjacent (differ by exactly one edit operation)
bool is_adjacent(const string& word1, const string& word2) {
    // Use edit_distance_within for simpler, faster implementation
    return edit_distance_within(word1, word2, 1);
}

// Helper function to check if longer_word becomes shorter_word after a single deletion
// Optimized version without string concatenation
bool is_deletion(const string& longer_word, const string& shorter_word) {
    const size_t long_len = longer_word.length();
    const size_t short_len = shorter_word.length();
    
    // If lengths don't differ by exactly 1, early exit
    if (long_len != short_len + 1) return false;
    
    for (size_t i = 0; i < long_len; i++) {
        // Check if removing character at position i in longer_word gives shorter_word
        bool match = true;
        for (size_t j = 0, k = 0; j < short_len; j++, k++) {
            if (k == i) k++; // Skip the character at position i
            if (k >= long_len || shorter_word[j] != longer_word[k]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

// Load dictionary words from a file
void load_words(set<string>& word_list, const string& file_name) {
    cout << "Attempting to open: " << file_name << endl;
    ifstream file(file_name);
    if (!file) {
        cerr << "Error: Cannot open file " << file_name << endl;
        return;
    }
    
    string word;
    word_list.clear(); // Clear existing words
    
    // Reserve some capacity for common word list sizes
    const int estimated_words = 100000;
    
    // Read words in batches for better performance
    vector<string> word_batch;
    word_batch.reserve(1000);
    
    while (file >> word) {
        // Convert to lowercase in-place
        transform(word.begin(), word.end(), word.begin(), 
                 [](unsigned char c){ return tolower(c); });
        
        word_batch.push_back(word);
        
        // Insert batch when it reaches a certain size
        if (word_batch.size() >= 1000) {
            word_list.insert(word_batch.begin(), word_batch.end());
            word_batch.clear();
        }
    }
    
    // Insert any remaining words
    if (!word_batch.empty()) {
        word_list.insert(word_batch.begin(), word_batch.end());
    }
    
    file.close();
}

// Generate word ladder from begin_word to end_word
vector<string> generate_word_ladder(const string& begin_word, const string& end_word, const set<string>& word_list) {
    // Check if start and end words are the same
    if (begin_word == end_word) {
        return vector<string>();  // Return empty vector for same word
    }
    
    // Convert set to unordered_set for faster lookups
    unordered_set<string> dict(word_list.begin(), word_list.end());
    
    // Start BFS
    queue<vector<string>> ladder_queue;
    unordered_set<string> visited;
    
    // Initialize with begin_word
    vector<string> initial_ladder = {begin_word};
    ladder_queue.push(initial_ladder);
    visited.insert(begin_word);
    
    // For matching the specific paths the tests expect
    // Sort words to get consistent traversal order
    vector<string> sorted_words(word_list.begin(), word_list.end());
    sort(sorted_words.begin(), sorted_words.end());
    
    while (!ladder_queue.empty()) {
        vector<string> current_ladder = ladder_queue.front();
        ladder_queue.pop();
        
        string last_word = current_ladder.back();
        
        // Use sorted dictionary to get consistent paths
        for (const string& word : sorted_words) {
            if (visited.count(word) == 0 && is_adjacent(last_word, word)) {
                visited.insert(word);
                
                vector<string> new_ladder = current_ladder;
                new_ladder.push_back(word);
                
                // Check if we've reached the target
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
