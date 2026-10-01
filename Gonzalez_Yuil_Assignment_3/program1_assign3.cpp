/*
Program name: program1_assign3
Program description: an email organizer that impliments priority using a maxheap. written in c++, oop/class based
Inputs: txt file containing commands
Outputs: email information, number of unread emails 
Collaborators: N/A
Other sources: Gemini
Author: Ana Gonzalez Yuil
Creation Date: 9.30.26
Revision Date: 10.1.26
Revisions: N/A
*/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

/**
 * @struct Date
 * @brief Parses and represents a calendar date in MM-DD-YYYY format.
 */
struct Date {
    int month;
    int day;
    int year;

    /**
     * @brief Parses a date string into numerical components.
     * @param dateStr String in "MM-DD-YYYY" format.
     * @return Populated Date struct.
     */
    static Date parse(const std::string& dateStr) {
        Date d;
        // Extract month, day, and year substrings and convert to integers
        d.month = std::stoi(dateStr.substr(0, 2));
        d.day = std::stoi(dateStr.substr(3, 2));
        d.year = std::stoi(dateStr.substr(6, 4));
        return d;
    }

    /**
     * @brief Less-than operator to compare chronological date order.
     */
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }

    /**
     * @brief Greater-than operator (newer date is greater).
     */
    bool operator>(const Date& other) const {
        return other < *this;
    }
};

/**
 * @class Email
 * @brief Represents an individual email message with sender, subject, and date.
 */
class Email {
private:
    std::string sender;
    std::string subject;
    std::string dateStr;
    Date parsedDate;

    /**
     * @brief Assigns a numerical priority rank to sender categories.
     * @return Integer rank where higher value means higher priority.
     *         Boss (5) > Subordinate (4) > Peer (3) > ImportantPerson (2) > OtherPerson (1)
     */
    int getSenderRank() const {
        if (sender == "Boss") return 5;
        if (sender == "Subordinate") return 4;
        if (sender == "Peer") return 3;
        if (sender == "ImportantPerson") return 2;
        if (sender == "OtherPerson") return 1;
        return 0; // Default fallback for unknown senders
    }

public:
    /**
     * @brief Constructor for Email object.
     */
    Email(std::string s, std::string subj, std::string d)
        : sender(std::move(s)), subject(std::move(subj)), dateStr(std::move(d)) {
        parsedDate = Date::parse(dateStr);
    }

    // Accessor methods
    std::string getSender() const { return sender; }
    std::string getSubject() const { return subject; }
    std::string getDate() const { return dateStr; }

    /**
     * @brief Custom priority comparison operator for max heap.
     * @details Returns true if this email has higher priority than 'other'.
     *          Priority rules:
     *          1. Higher sender category rank takes precedence.
     *          2. If sender ranks are equal, the newer date takes precedence.
     */
    bool operator>(const Email& other) const {
        int thisRank = this->getSenderRank();
        int otherRank = other.getSenderRank();

        if (thisRank != otherRank) {
            return thisRank > otherRank;
        }
        // Tie-breaker: Newer date gets higher priority
        return this->parsedDate > other.parsedDate;
    }

    /**
     * @brief Less-than operator utilized by heap algorithms.
     */
    bool operator<(const Email& other) const {
        return other > *this;
    }
};

/**
 * @class EmailMaxHeap
 * @brief A list-based max heap implemented completely from scratch for priority queue management.
 */
class EmailMaxHeap {
private:
    // Dynamic array (list) backing the binary heap structure
    std::vector<Email> heap;

    // Helper: Get parent index of node i
    int parent(int i) { return (i - 1) / 2; }

    // Helper: Get left child index of node i
    int leftChild(int i) { return (2 * i) + 1; }

    // Helper: Get right child index of node i
    int rightChild(int i) { return (2 * i) + 2; }

    /**
     * @brief Percolates an element upward to restore the max-heap property.
     */
    void heapifyUp(int i) {
        while (i > 0 && heap[parent(i)] < heap[i]) {
            std::swap(heap[parent(i)], heap[i]);
            i = parent(i);
        }
    }

    /**
     * @brief Percolates an element downward to restore the max-heap property.
     */
    void heapifyDown(int i) {
        int maxIndex = i;
        int l = leftChild(i);
        int r = rightChild(i);
        int size = heap.size();

        // Compare with left child
        if (l < size && heap[maxIndex] < heap[l]) {
            maxIndex = l;
        }

        // Compare with right child
        if (r < size && heap[maxIndex] < heap[r]) {
            maxIndex = r;
        }

        // If a child has higher priority, swap and continue heapifying down
        if (maxIndex != i) {
            std::swap(heap[i], heap[maxIndex]);
            heapifyDown(maxIndex);
        }
    }

public:
    /**
     * @brief Inserts a new email into the max heap and reorders.
     */
    void insert(const Email& email) {
        heap.push_back(email);
        heapifyUp(heap.size() - 1);
    }

    /**
     * @brief Returns the highest priority email without removing it.
     */
    const Email& getMax() const {
        if (heap.empty()) {
            throw std::runtime_error("Heap is empty!");
        }
        return heap[0];
    }

    /**
     * @brief Removes the highest priority email (root) and reorders remaining elements.
     */
    void removeMax() {
        if (heap.empty()) {
            return;
        }
        // Move last element to root and remove back
        heap[0] = heap.back();
        heap.pop_back();

        // Restore max-heap property from top down
        if (!heap.empty()) {
            heapifyDown(0);
        }
    }

    /**
     * @brief Returns the total count of unread emails in the heap.
     */
    size_t size() const {
        return heap.size();
    }

    /**
     * @brief Checks if the heap contains any unread emails.
     */
    bool isEmpty() const {
        return heap.empty();
    }
};

/**
 * @brief Helper utility to trim whitespace from command substrings.
 */
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

int main() {
    // Optimize standard I/O operations for performance
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    EmailMaxHeap emailHeap;
    std::string line;

    // Read commands line-by-line from standard input (file redirected via cin)
    while (std::getline(std::cin, line)) {
        line = trim(line);
        if (line.empty()) continue;

        // 1. Handle EMAIL command: Adds an email to the priority queue
        if (line.rfind("EMAIL", 0) == 0) {
            std::string payload = line.substr(5); // Strip "EMAIL" keyword
            
            // Parse comma-separated fields: <sender category>, <subject line>, <date>
            size_t firstComma = payload.find(',');
            size_t secondComma = payload.find(',', firstComma + 1);

            if (firstComma != std::string::npos && secondComma != std::string::npos) {
                std::string sender = trim(payload.substr(0, firstComma));
                std::string subject = trim(payload.substr(firstComma + 1, secondComma - firstComma - 1));
                std::string date = trim(payload.substr(secondComma + 1));

                Email newEmail(sender, subject, date);
                emailHeap.insert(newEmail);
            }
        }
        // 2. Handle NEXT command: Displays the highest priority email
        else if (line == "NEXT") {
            if (!emailHeap.isEmpty()) {
                const Email& topEmail = emailHeap.getMax();
                std::cout << "Next Email:\n";
                std::cout << " Sender: " << topEmail.getSender() << "\n";
                std::cout << " Subject: " << topEmail.getSubject() << "\n";
                std::cout << " Date: " << topEmail.getDate() << "\n";
            } else {
                std::cout << "Next Email:\n No unread emails.\n";
            }
        }
        // 3. Handle READ command: Marks highest priority email as read (removes from heap)
        else if (line == "READ") {
            if (!emailHeap.isEmpty()) {
                emailHeap.removeMax();
            }
        }
        // 4. Handle COUNT command: Displays the current count of unread emails
        else if (line == "COUNT") {
            std::cout << emailHeap.size() << "\n";
        }
    }

    return 0;
}