#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>
#include <iomanip> // Include for formatting

using namespace std;

// Function to preprocess text and compute word frequencies
unordered_map<string, int> preprocessText(const string& filename) {
    unordered_map<string, int> wordCount;
    ifstream file(filename);
    string word;

    set<string> commonWords = {"A", "AND", "AN", "OF", "IN", "THE"};

    while (file >> word) {
        // Normalize word: Remove non-alphanumeric characters and convert to uppercase
        string normalized = "";
        for (char c : word) {
            if (isalnum(c)) {
                normalized += toupper(c);
            }
        }

        // Skip common words
        if (!normalized.empty() && commonWords.find(normalized) == commonWords.end()) {
            wordCount[normalized]++;
        }
    }

    file.close();
    return wordCount;
}

// Function to get the top 100 frequent words
vector<pair<string, double>> getTopFrequentWords(const unordered_map<string, int>& wordCount) {
    vector<pair<string, int>> freqVec(wordCount.begin(), wordCount.end());

    // Sort by frequency in descending order
    sort(freqVec.begin(), freqVec.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
        return a.second > b.second;
    });

    // Normalize frequencies
    int totalWords = 0;
    for (const auto& pair : freqVec) {
        totalWords += pair.second;
    }

    vector<pair<string, double>> normalizedFreq;
    for (size_t i = 0; i < min(size_t(100), freqVec.size()); i++) {
        normalizedFreq.emplace_back(freqVec[i].first, freqVec[i].second / double(totalWords));
    }

    return normalizedFreq;
}

// Function to compute similarity index
double computeSimilarity(const vector<pair<string, double>>& file1,
                         const vector<pair<string, double>>& file2) {
    unordered_map<string, double> freqMap;

    for (const auto& pair : file1) {
        freqMap[pair.first] = pair.second;
    }

    double similarity = 0.0;
    for (const auto& pair : file2) {
        if (freqMap.find(pair.first) != freqMap.end()) {
            similarity += freqMap[pair.first] * pair.second;
        }
    }

    return similarity;
}

int main() {
    const vector<string> fileNames = {
        "Cats by Moncrif.txt", "Foxes Book of Martyrs Part 1.txt", "Foxes Book of Martyrs Part 2.txt", 
        "Foxes Book of Martyrs Part 3.txt", "Foxes Book of Martyrs Part 4.txt", "Foxes Book of Martyrs Part 5.txt", 
        "Foxes Book of Martyrs Part 6.txt", "Gerards Herbal Vol. 1.txt", "Gerards Herbal Vol. 2.txt", 
        "Gerards Herbal Vol. 3.txt", "Gerards Herbal Vol.4.txt", "Gil Blas.txt", "Gossip in a Library.txt", 
        "Hudibras.txt", "King of the Beggars.txt", "Knocknagow.txt", "Les Chats par Moncrif.txt", 
        "Lives and Anecdotes of Misers.txt", "Love and Madness - Herbert Croft.txt", 
        "Memoirs of Laetitia Pilkington V 1.txt", "Memoirs of Laetitia Pilkington V 2.txt", 
        "Memoirs of Laetitia Pilkington V 3.txt", "Memoirs of Mrs Margaret Leeson - Peg Plunkett.txt", 
        "Monro his Expedition.txt", "Mrs Beetons Book of Household Management.txt", 
        "Out of the Hurly-Burly.txt", "Percys Reliques.txt", "Pompey The Little.txt", 
        "Radical Pamphlets from the English Civil War.txt", "Scepsis Scientifica.txt", 
        "The Anatomy of Melancholy Part 1.txt", "The Anatomy of Melancholy Part 2.txt", 
        "The Anatomy of Melancholy Part 3.txt", "The Complete Cony-catching.txt", 
        "The Consolation of Philosophy.txt", "The Devil on Two Sticks.txt", 
        "The Diary of a Lover of Literature.txt", "The History Of Ireland - Geoffrey Keating.txt", 
        "The History of the Human Heart.txt", "The Ingoldsby Legends.txt", 
        "The Life of Beau Nash.txt", "The Life of John Buncle by Thomas Amory.txt", 
        "The Life of King Richard III.txt", "The Life of Pico della Mirandola.txt", 
        "The Martyrdom of Man.txt", "The Masterpiece of Aristotle.txt", 
        "The Memoirs of Count Boruwlaski.txt", "The Metamorphosis of Ajax.txt", 
        "The Newgate Calendar - Supplement 3.txt", "The Newgate Calendar Supplement.txt", 
        "The Newgate Calendar V 1.txt", "The Newgate Calendar V 2.txt", 
        "The Newgate Calendar V 3.txt", "The Newgate Calendar V 4.txt", 
        "The Newgate Calendar V 5.txt", "The Newgate Calendar V 6.txt", 
        "The Poems of Ossian.txt", "The Poetical Works of John Skelton.txt", 
        "The Protestant Reformation.txt", "The Real Story of John Carteret Pilkington.txt", 
        "The Rowley Poems.txt", "The Silver Fox.txt"
    };

    const int numBooks = fileNames.size();
    vector<vector<pair<string, double>>> bookData(numBooks);

    // Process each book and get top 100 words
    for (int i = 0; i < numBooks; ++i) {
        auto wordCount = preprocessText(fileNames[i]);
        bookData[i] = getTopFrequentWords(wordCount);
    }

    // Build similarity matrix
    vector<vector<double>> similarityMatrix(numBooks, vector<double>(numBooks, 0.0));
    for (int i = 0; i < numBooks; ++i) {
        for (int j = i + 1; j < numBooks; ++j) {
            double similarity = computeSimilarity(bookData[i], bookData[j]);
            similarityMatrix[i][j] = similarity;
            similarityMatrix[j][i] = similarity; // Symmetric matrix
        }
    }

    // Find top 10 similar pairs
    vector<tuple<double, int, int>> similarityPairs;
    for (int i = 0; i < numBooks; ++i) {
        for (int j = i + 1; j < numBooks; ++j) {
            similarityPairs.emplace_back(similarityMatrix[i][j], i, j);
        }
    }

    // Sort pairs by similarity in descending order
    sort(similarityPairs.begin(), similarityPairs.end(), [](const tuple<double, int, int>& a, const tuple<double, int, int>& b) {
        return get<0>(a) > get<0>(b);
    });

    // Output top 10 similar pairs in a tabular format
    cout << "\n======================== Top 10 Similar Pairs of Books ========================\n";
    cout << "| Rank | Book 1                                      | Book 2                                      | Similarity |\n";
    cout << "-------------------------------------------------------------------------------\n";

    for (int i = 0; i < 10 && i < similarityPairs.size(); ++i) {
        double similarity;
        int book1, book2;
        tie(similarity, book1, book2) = similarityPairs[i];

        // Format output in a table
        cout << "| " << setw(4) << i + 1 << " | " 
             << setw(45) << left << fileNames[book1] << " | "
             << setw(45) << left << fileNames[book2] << " | "
             << setw(10) << fixed << setprecision(4) << similarity << " |\n";
    }

    cout << "===============================================================================\n";

    return 0;
}
