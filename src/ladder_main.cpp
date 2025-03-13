#include "ladder.h"
#include <iostream>
#include <string>

int main() {
    // Load the dictionary
    set<string> word_list;
    load_words(word_list, "words.txt");
    
    // Uncomment to run verification tests
    verify_word_ladder();
    return 0;
    
    string start_word, end_word;
    cout << "Enter start word: ";
    cin >> start_word;
    
    cout << "Enter end word: ";
    cin >> end_word;
    
    // Convert words to lowercase
    for (char& c : start_word) c = tolower(c);
    for (char& c : end_word) c = tolower(c);
    
    // Check if start and end words are the same
    if (start_word == end_word) {
        error(start_word, end_word, "Start and end words cannot be the same");
        return 1;
    }
    
    // Check if end word is in the dictionary
    if (word_list.find(end_word) == word_list.end()) {
        error(start_word, end_word, "End word must be in the dictionary");
        return 1;
    }
    
    // Generate the word ladder
    vector<string> ladder = generate_word_ladder(start_word, end_word, word_list);
    
    // Print the word ladder
    print_word_ladder(ladder);
    
    return 0;
}
