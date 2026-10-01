/*
Program name: program2_assign3
Program description: an email organizer that impliments priority using a maxheap. written in c++, oop/class based
Inputs: txt file containing commands
Outputs: email information, number of unread emails 
Collaborators: N/A
Other sources: CoPilot
Author: Ana Gonzalez Yuil
Creation Date: 9.30.26
Revision Date: 10.1.26
Revisions: N/A
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

/*
-----------------------------------------------------------
Email Class
-----------------------------------------------------------
Represents a single email with:
- sender category
- subject line
- date

The heap will store Email objects.
-----------------------------------------------------------
*/
class Email
{
public:
    string senderCategory;
    string subject;
    string date;

    Email()
    {
        senderCategory = "";
        subject = "";
        date = "";
    }

    Email(string sender, string subj, string emailDate)
    {
        senderCategory = sender;
        subject = subj;
        date = emailDate;
    }
};

/*
-----------------------------------------------------------
MaxHeap Class
-----------------------------------------------------------
Uses a vector as the underlying list structure.

The highest priority email is always stored at index 0.

Priority Rules:
1. Sender category determines primary priority.
2. If sender categories match, the newer date wins.
-----------------------------------------------------------
*/
class MaxHeap
{
private:

    // Internal list used to represent the heap
    vector<Email> heap;

    /*
    -------------------------------------------------------
    senderPriority()

    Converts sender category strings into numerical values.

    Higher number = higher priority.

    Boss             -> 5
    Subordinate      -> 4
    Peer             -> 3
    ImportantPerson  -> 2
    OtherPerson      -> 1
    -------------------------------------------------------
    */
    int senderPriority(const string& sender) const
    {
        if (sender == "Boss")
            return 5;
        else if (sender == "Subordinate")
            return 4;
        else if (sender == "Peer")
            return 3;
        else if (sender == "ImportantPerson")
            return 2;
        else
            return 1;
    }

    /*
    -------------------------------------------------------
    dateValue()

    Converts a date from MM-DD-YYYY into an integer
    YYYYMMDD format.

    Example:
    03-15-2025 becomes 20250315

    Larger value means newer date.
    -------------------------------------------------------
    */
    int dateValue(const string& date) const
    {
        string month = date.substr(0, 2);
        string day   = date.substr(3, 2);
        string year  = date.substr(6, 4);

        return stoi(year + month + day);
    }

    /*
    -------------------------------------------------------
    higherPriority()

    Compares two emails.

    Returns true if email A should appear higher
    in the max heap than email B.

    Comparison Order:
    1. Sender priority
    2. Newer date
    -------------------------------------------------------
    */
    bool higherPriority(const Email& a, const Email& b) const
    {
        int senderA = senderPriority(a.senderCategory);
        int senderB = senderPriority(b.senderCategory);

        // Compare sender categories first
        if (senderA != senderB)
        {
            return senderA > senderB;
        }

        // If same sender category,
        // newer date gets priority
        return dateValue(a.date) > dateValue(b.date);
    }

    /*
    -------------------------------------------------------
    heapifyUp()

    Used after insertion.

    Moves a newly inserted email upward until
    max heap order is restored.
    -------------------------------------------------------
    */
    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parent = (index - 1) / 2;

            if (higherPriority(heap[index], heap[parent]))
            {
                swap(heap[index], heap[parent]);
                index = parent;
            }
            else
            {
                break;
            }
        }
    }

    /*
    -------------------------------------------------------
    heapifyDown()

    Used after removing the root.

    Moves an email downward until max heap
    ordering is restored.
    -------------------------------------------------------
    */
    void heapifyDown(int index)
    {
        int size = heap.size();

        while (true)
        {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int largest = index;

            if (leftChild < size &&
                higherPriority(heap[leftChild],
                               heap[largest]))
            {
                largest = leftChild;
            }

            if (rightChild < size &&
                higherPriority(heap[rightChild],
                               heap[largest]))
            {
                largest = rightChild;
            }

            if (largest != index)
            {
                swap(heap[index], heap[largest]);
                index = largest;
            }
            else
            {
                break;
            }
        }
    }

public:

    /*
    -------------------------------------------------------
    insert()

    Adds a new email and restores heap order.
    -------------------------------------------------------
    */
    void insert(const Email& email)
    {
        heap.push_back(email);
        heapifyUp(heap.size() - 1);
    }

    /*
    -------------------------------------------------------
    empty()

    Returns true if heap contains no emails.
    -------------------------------------------------------
    */
    bool empty() const
    {
        return heap.empty();
    }

    /*
    -------------------------------------------------------
    size()

    Returns number of unread emails.
    -------------------------------------------------------
    */
    int size() const
    {
        return heap.size();
    }

    /*
    -------------------------------------------------------
    getMax()

    Returns the highest priority email.

    Assumes heap is not empty.
    -------------------------------------------------------
    */
    Email getMax() const
    {
        return heap[0];
    }

    /*
    -------------------------------------------------------
    removeMax()

    Removes highest priority email (READ command).

    Steps:
    1. Replace root with last element.
    2. Delete last element.
    3. Heapify down.
    -------------------------------------------------------
    */
    void removeMax()
    {
        if (heap.empty())
            return;

        heap[0] = heap.back();
        heap.pop_back();

        if (!heap.empty())
        {
            heapifyDown(0);
        }
    }
};

/*
-----------------------------------------------------------
trim()

Removes leading and trailing spaces from strings.

Useful when parsing command fields.
-----------------------------------------------------------
*/
string trim(string str)
{
    while (!str.empty() && str.front() == ' ')
        str.erase(str.begin());

    while (!str.empty() && str.back() == ' ')
        str.pop_back();

    return str;
}

/*
-----------------------------------------------------------
processEmailCommand()

Parses:

EMAIL sender, subject, date

and inserts email into heap.
-----------------------------------------------------------
*/
void processEmailCommand(const string& line,
                         MaxHeap& inbox)
{
    string content = line.substr(6);

    stringstream ss(content);

    string sender;
    string subject;
    string date;

    getline(ss, sender, ',');
    getline(ss, subject, ',');
    getline(ss, date);

    sender = trim(sender);
    subject = trim(subject);
    date = trim(date);

    Email newEmail(sender, subject, date);

    inbox.insert(newEmail);
}

/*
-----------------------------------------------------------
Main Function
-----------------------------------------------------------

1. Ask user for input filename.
2. Open file.
3. Execute commands sequentially.
4. Display requested output.
-----------------------------------------------------------
*/
int main()
{
    MaxHeap inbox;

    string filename;

    cout << "Enter command file name: ";
    cin >> filename;

    ifstream inputFile(filename);

    if (!inputFile)
    {
        cout << "Error opening file." << endl;
        return 1;
    }

    string line;

    while (getline(inputFile, line))
    {
        /*
        ---------------------------------------------------
        EMAIL COMMAND
        ---------------------------------------------------
        */
        if (line.rfind("EMAIL", 0) == 0)
        {
            processEmailCommand(line, inbox);
        }

        /*
        ---------------------------------------------------
        NEXT COMMAND

        Display highest priority unread email.
        ---------------------------------------------------
        */
        else if (line == "NEXT")
        {
            if (inbox.empty())
            {
                cout << "No unread emails." << endl;
            }
            else
            {
                Email nextEmail = inbox.getMax();

                cout << "Next Email:" << endl;
                cout << "Sender: "
                     << nextEmail.senderCategory
                     << endl;

                cout << "Subject: "
                     << nextEmail.subject
                     << endl;

                cout << "Date: "
                     << nextEmail.date
                     << endl;
            }
        }

        /*
        ---------------------------------------------------
        READ COMMAND

        Remove highest priority email.
        ---------------------------------------------------
        */
        else if (line == "READ")
        {
            if (ibriefnbox.empty())
            {
                cout << "No unread emails." << endl;
            }
            else
            {
                inbox.removeMax();
            }
        }

        /*
        ---------------------------------------------------
        COUNT COMMAND

        Display unread email count.
        ---------------------------------------------------
        */
        else if (line == "COUNT")
        {
            cout << "Unread Emails: "
                 << inbox.size()
                 << endl;
        }
    }

    inputFile.close();

    return 0;
}