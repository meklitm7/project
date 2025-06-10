#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>  // for system()
#include <unistd.h> // for sleep() on Unix-like systems
#include <cctype>   // for tolower()

using namespace std;

// Trim function to remove leading and trailing spaces from input strings
string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Structure to represent a bank account with account number, customer name, balance, and interest rate
struct Account {
    int accountNumber;      // Unique account number assigned by user
    string customerName;    // Name of the account holder
    double balance;         // Current balance in the account
    double interestRate;    // Annual interest rate in percentage
};

// Structure to represent a loan with loan ID, customer name, loan amount, interest rate, duration, and remaining balance
struct Loan {
    int loanID;             // Unique loan identifier generated automatically
    string customerName;    // Name of the loan customer
    double loanAmount;      // Original loan amount
    double interestRate;    // Interest rate for the loan in percentage
    int duration;           // Duration of the loan in months
    double remainingBalance;// Remaining balance to be repaid
};

// Global vectors to store all accounts and loans in memory
vector<Account> accounts;
vector<Loan> loanBook;

// File names used to persist account and loan data between program runs
const string accountsFile = "accounts.txt";
const string loanBookFile = "loanbook.txt";

// Function declarations for account management operations
void loadAccounts();            // Load accounts from file into memory
void saveAccounts();            // Save accounts from memory to file
bool accountNumberExists(int);  // Check if an account number already exists
int findAccountIndexByNumber(int);// Find index of account by account number
void createAccount();           // Create a new account with user input
void depositFunds();            // Deposit funds into an existing account
void withdrawFunds();           // Withdraw funds from an existing account
void transferFunds();           // Transfer funds between two accounts
void viewCurrentBalance();      // View balance of a specific account
void calculateAndAddInterest(); // Calculate and add interest to an account balance
void closeAccount();            // Close an account and remove it from records
void listAllAccounts();         // List all existing accounts with details
void deleteAllAccounts();       // Delete all accounts from records

// Function declarations for loan management operations
void loadLoanBook();            // Load loans from file into memory
void saveLoanBook();            // Save loans from memory to file
int generateUniqueLoanID();     // Generate a unique loan ID for new loans
Loan* findLoanByID(int);        // Find a loan by its ID
void createLoanAgreement();     // Create a new loan agreement with user input
void makeMonthlyRepayment();    // Make a monthly repayment on a loan
void displayLoanBook();         // Display all loans with details

int main() {
    // Load existing accounts and loans from files at program start
    loadAccounts();
    loadLoanBook();

    int choice;
    do {
        // Display the main menu with all banking and loan options
        cout << "\nBanking System Menu:\n"
             << "0. Exit\n"
             << "1. Create Account\n"
             << "2. Deposit Funds\n"
             << "3. Withdraw Funds\n"
             << "4. Transfer Funds\n"
             << "5. View Current Balance\n"
             << "6. Calculate and Add Interest\n"
             << "7. Close Account\n"
             << "8. List All Accounts\n"
             << "9. Delete All Accounts\n"
             << "10. Create Loan Book (Load from file)\n"
             << "11. Create Loan Agreement\n"
             << "12. Make Monthly Repayment\n"
             << "13. Display Loan Book\n"
             << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Clear newline character from input buffer

        // Execute the selected menu option by calling the corresponding function
        switch(choice) {
            case 0:
                saveAccounts();     // Save accounts before exiting
                saveLoanBook();     // Save loans before exiting
                cout << "Exiting program. Data saved.\n";
                break;
            case 1:
                createAccount();
                break;
            case 2:
                depositFunds();
                break;
            case 3:
                withdrawFunds();
                break;
            case 4:
                transferFunds();
                break;
            case 5:
                viewCurrentBalance();
                break;
            case 6:
                calculateAndAddInterest();
                break;
            case 7:
                closeAccount();
                break;
            case 8:
                listAllAccounts();
                break;
            case 9:
                deleteAllAccounts();
                break;
            case 10:
                loadLoanBook();
                cout << "Loan book loaded from file.\n";
                break;
            case 11:
                createLoanAgreement();
                break;
            case 12:
                makeMonthlyRepayment();
                break;
            case 13:
                displayLoanBook();
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 0);

    return 0;
}

// Load accounts from the accounts file into the accounts vector
void loadAccounts() {
    accounts.clear();
    ifstream inFile(accountsFile);
    if (!inFile) {
        // File does not exist yet, so no accounts to load
        return;
    }
    string line;
    while (getline(inFile, line)) {
        istringstream iss(line);
        Account acc;
        string name;
        // Read account number, then ignore the space, then read name until '|'
        if (iss >> acc.accountNumber) {
            iss.ignore(); // Ignore space before name
            getline(iss, name, '|'); // Read customer name until delimiter '|'
            acc.customerName = trim(name);
            iss >> acc.balance >> acc.interestRate;
            accounts.push_back(acc);
        }
    }
    inFile.close();
}

// Save all accounts from the accounts vector to the accounts file
void saveAccounts() {
    ofstream outFile(accountsFile);
    if (!outFile) {
        cerr << "Error: Unable to open accounts file for saving.\n"; // cerr for errors
        return;
    }
    for (int i = 0; i < (int)accounts.size(); ++i) {
        outFile << accounts[i].accountNumber << " " << accounts[i].customerName << "|"
                << accounts[i].balance << " " << accounts[i].interestRate << "\n";
    }
    outFile.close();
}

// Check if an account number already exists in the accounts vector
bool accountNumberExists(int accountNumber) {
    for (int i = 0; i < (int)accounts.size(); ++i) {
        if (accounts[i].accountNumber == accountNumber) {
            return true;
        }
    }
    return false;
}

// Find the index of an account in the accounts vector by account number
// Returns -1 if not found
int findAccountIndexByNumber(int accountNumber) {
    for (int i = 0; i < (int)accounts.size(); ++i) {
        if (accounts[i].accountNumber == accountNumber) {
            return i;
        }
    }
    return -1;
}

// Create a new account by taking user input for account number, name, initial deposit, and interest rate
void createAccount() {
    Account newAcc;
    int accNum;
    do {
        cout << "Enter account number: ";
        cin >> accNum;
        cin.ignore();
        if (accountNumberExists(accNum)) {
            cout << "Account number already exists. Please enter a different number.\n";
        }
    } while (accountNumberExists(accNum));
    newAcc.accountNumber = accNum;

    cout << "Enter customer name: ";
    string name;
    getline(cin, name);
    newAcc.customerName = trim(name);

    cout << "Enter initial deposit amount: ";
    cin >> newAcc.balance;
    // Basic validation, but without <limits> cannot robustly handle non-numeric input.
    // User must enter a valid number for this to work as intended.
    if (newAcc.balance < 0) {
        cout << "Initial deposit cannot be negative. Setting to 0.\n";
        newAcc.balance = 0;
    }


    cout << "Enter annual interest rate (percent): ";
    cin >> newAcc.interestRate;
    // Basic validation
    if (newAcc.interestRate < 0) {
        cout << "Interest rate cannot be negative. Setting to 0.\n";
        newAcc.interestRate = 0;
    }
    cin.ignore();

    accounts.push_back(newAcc);
    saveAccounts();

    cout << "Account created successfully.\n";
    cout << "Account Number: " << newAcc.accountNumber << "\n"
         << "Customer Name: " << newAcc.customerName << "\n"
         << "Balance: " << newAcc.balance << "\n"
         << "Interest Rate: " << newAcc.interestRate << "%\n";
}

// Deposit funds into an existing account by account number
void depositFunds() {
    cout << "Enter account number to deposit into: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    cout << "Enter deposit amount: ";
    double amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid deposit amount. Must be positive.\n";
        return;
    }

    accounts[idx].balance += amount;
    saveAccounts();

    cout << "Deposit successful. New balance: " << accounts[idx].balance << "\n";
}

// Withdraw funds from an existing account by account number
void withdrawFunds() {
    cout << "Enter account number to withdraw from: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    cout << "Enter withdrawal amount: ";
    double amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid withdrawal amount. Must be positive.\n";
        return;
    }

    if (amount > accounts[idx].balance) {
        cout << "Insufficient funds.\n";
        return;
    }

    accounts[idx].balance -= amount;
    saveAccounts();

    cout << "Withdrawal successful. New balance: " << accounts[idx].balance << "\n";
}

// Transfer funds between two accounts by their account numbers
void transferFunds() {
    cout << "Enter source account number: ";
    int srcAccNum;
    cin >> srcAccNum;
    cin.ignore();

    int srcIdx = findAccountIndexByNumber(srcAccNum);
    if (srcIdx == -1) {
        cout << "Source account not found.\n";
        return;
    }

    cout << "Enter destination account number: ";
    int destAccNum;
    cin >> destAccNum;
    cin.ignore();

    int destIdx = findAccountIndexByNumber(destAccNum);
    if (destIdx == -1) {
        cout << "Destination account not found.\n";
        return;
    }

    if (srcAccNum == destAccNum) {
        cout << "Source and destination accounts cannot be the same.\n";
        return;
    }

    cout << "Enter transfer amount: ";
    double amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid transfer amount. Must be positive.\n";
        return;
    }

    if (amount > accounts[srcIdx].balance) {
        cout << "Insufficient funds in source account.\n";
        return;
    }

    accounts[srcIdx].balance -= amount;
    accounts[destIdx].balance += amount;
    saveAccounts();

    cout << "Transfer successful.\n";
    cout << "Source account new balance: " << accounts[srcIdx].balance << "\n";
    cout << "Destination account new balance: " << accounts[destIdx].balance << "\n";
}

// View the current balance of an account by account number
void viewCurrentBalance() {
    cout << "Enter account number: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    cout << "Current balance: " << accounts[idx].balance << "\n";
}

// Calculate and add simple interest to an account's balance
void calculateAndAddInterest() {
    cout << "Enter account number to calculate interest: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    // Simple interest calculation for one year
    double interest = accounts[idx].balance * (accounts[idx].interestRate / 100.0);
    accounts[idx].balance += interest;
    saveAccounts();

    cout << "Interest added. New balance: " << accounts[idx].balance << "\n";
}

// Close an account by removing it from the accounts vector
void closeAccount() {
    cout << "Enter account number to close: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    for (int i = 0; i < (int)accounts.size(); ++i) {
        if (accounts[i].accountNumber == accNum) {
            accounts.erase(accounts.begin() + i);
            saveAccounts();
            cout << "Account closed successfully.\n";
            return;
        }
    }
    cout << "Account not found.\n";
}

// List all accounts with their details
void listAllAccounts() {
    if (accounts.empty()) {
        cout << "No accounts found.\n";
        return;
    }

    system("clear"); // Clear screen for Unix-like systems

    cout << "Accounts List:\n";
    for (auto a = accounts.cbegin(); a != accounts.cend(); ++a) {
        cout << "Account Number: " << a->accountNumber << "\n"
             << "Customer Name: " << a->customerName << "\n"
             << "Balance: " << a->balance << "\n"
             << "Interest Rate: " << a->interestRate << "%\n"
             << "-------------------------\n";
    }
    sleep(5); // Pause for 5 seconds
}

// Delete all accounts from the system
void deleteAllAccounts() {
    char confirm;
    cout << "Are you sure you want to delete ALL accounts? (y/n): ";
    cin >> confirm;
    cin.ignore();
    if (tolower(confirm) == 'y') { // `tolower` requires <cctype>
        accounts.clear();
        saveAccounts();
        cout << "All accounts deleted.\n";
    } else {
        cout << "Account deletion cancelled.\n";
    }
}

// Load loan records from the loan book file into memory
void loadLoanBook() {
    loanBook.clear();
    ifstream inFile(loanBookFile);
    if (!inFile) {
        // File does not exist yet, so no loans to load
        return;
    }
    string line;
    while (getline(inFile, line)) {
        istringstream iss(line);
        Loan loan;
        string name;
        // Read loan ID, then ignore the space, then read name until '|'
        if (iss >> loan.loanID) {
            iss.ignore(); // Ignore space before name
            getline(iss, name, '|'); // Read customer name until delimiter '|'
            loan.customerName = trim(name);
            iss >> loan.loanAmount >> loan.interestRate >> loan.duration >> loan.remainingBalance;
            loanBook.push_back(loan);
        }
    }
    inFile.close();
}

// Save all loan records from memory to the loan book file
void saveLoanBook() {
    ofstream outFile(loanBookFile);
    if (!outFile) {
        cerr << "Error: Unable to open loan book file for saving.\n"; // cerr for errors
        return;
    }
    for (auto a = loanBook.cbegin(); a != loanBook.cend(); ++a) {
        outFile << a->loanID << " " << a->customerName << "|"
                << a->loanAmount << " " << a->interestRate << " "
                << a->duration << " " << a->remainingBalance << "\n";
    }
    outFile.close();
}

// Generate a unique loan ID by finding the maximum existing ID and adding 1
int generateUniqueLoanID() {
    int maxID = 0;
    for (int i = 0; i < (int)loanBook.size(); ++i) {
        if (loanBook[i].loanID > maxID) {
            maxID = loanBook[i].loanID;
        }
    }
    return maxID + 1;
}

// Find a loan record by its loan ID, returns pointer or nullptr if not found
Loan* findLoanByID(int id) {
    for (int i = 0; i < (int)loanBook.size(); ++i) {
        if (loanBook[i].loanID == id) {
            return &loanBook[i];
        }
    }
    return nullptr;
}

// Create a new loan agreement by collecting details from the user
void createLoanAgreement() {
    Loan newLoan;
    newLoan.loanID = generateUniqueLoanID();

    cout << "Enter customer name: ";
    string name;
    getline(cin, name);
    newLoan.customerName = trim(name);

    cout << "Enter loan amount: ";
    cin >> newLoan.loanAmount;
    if (newLoan.loanAmount <= 0) {
        cout << "Loan amount must be positive. Setting to 0.\n";
        newLoan.loanAmount = 0;
    }
    
    cout << "Enter interest rate (percent): ";
    cin >> newLoan.interestRate;
    if (newLoan.interestRate < 0) {
        cout << "Interest rate cannot be negative. Setting to 0.\n";
        newLoan.interestRate = 0;
    }

    cout << "Enter duration (months): ";
    cin >> newLoan.duration;
    if (newLoan.duration <= 0) {
        cout << "Duration must be positive. Setting to 1 month.\n";
        newLoan.duration = 1;
    }
    cin.ignore(); // Consume the newline left by cin

    newLoan.remainingBalance = newLoan.loanAmount;

    loanBook.push_back(newLoan);
    saveLoanBook();

    system("clear"); // Clear screen for Unix-like systems

    cout << "Loan agreement created successfully.\n";
    cout << "Loan ID: " << newLoan.loanID << "\n"
         << "Customer Name: " << newLoan.customerName << "\n"
         << "Loan Amount: " << newLoan.loanAmount << "\n"
         << "Interest Rate: " << newLoan.interestRate << "%\n"
         << "Duration: " << newLoan.duration << " months\n"
         << "Remaining Balance: " << newLoan.remainingBalance << "\n"
         << "-------------------------\n";

    sleep(5); // Pause for 5 seconds
}

// Make a monthly repayment on a loan by loan ID
void makeMonthlyRepayment() {
    cout << "Enter loan ID for repayment: ";
    int id;
    cin >> id;
    cin.ignore();

    Loan* loan = findLoanByID(id);
    if (loan == nullptr) {
        cout << "Loan ID not found.\n";
        return;
    }

    cout << "Current remaining balance: " << loan->remainingBalance << "\n";
    cout << "Enter repayment amount: ";
    double repayment;
    cin >> repayment;

    if (repayment <= 0) {
        cout << "Repayment amount must be positive. Transaction cancelled.\n";
        return;
    }

    if (repayment > loan->remainingBalance) {
        cout << "Repayment amount exceeds remaining balance. Transaction cancelled.\n";
        return;
    }

    loan->remainingBalance -= repayment;
    saveLoanBook();

    cout << "Repayment successful. Updated remaining balance: " << loan->remainingBalance << "\n";
}

// Display all loans in the loan book with their details
void displayLoanBook() {
    if (loanBook.empty()) {
        cout << "Loan book is empty.\n";
        return;
    }
    cout << "Loan Book:\n";
    for (auto a = loanBook.cbegin(); a != loanBook.cend(); ++a) {
        cout << "Loan ID: " << a->loanID << "\n"
             << "Customer Name: " << a->customerName << "\n"
             << "Loan Amount: " << a->loanAmount << "\n"
             << "Interest Rate: " << a->interestRate << "%\n"
             << "Duration: " << a->duration << " months\n"
             << "Remaining Balance: " << a->remainingBalance << "\n"
             << "-------------------------\n";
    }
    sleep(5); // Pause for 5 seconds
}
