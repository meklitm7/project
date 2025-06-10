#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib> // for system("clear")
#include <unistd.h> // for sleep()
#include <ctime>

using namespace std;

// Trim function to remove leading and trailing spaces from input strings
string trim(const string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Structure to represent a bank account with account number, customer name, balance, interest rate, and frozen status
struct Account {
    int accountNumber;          // Unique account number assigned by user
    string customerName;        // Name of the account holder
    double balance;             // Current balance in the account
    double interestRate;        // Annual interest rate in percentage
    bool isFrozen;              // Account frozen status: true if frozen, false if active
};

// Structure to represent a transaction with ID, date/time, type, amount, and balance after transaction
struct Transaction {
    int transactionID;
    int accountNumber;
    string dateTime;
    string type;               // deposit, withdrawal, transfer_in, transfer_out
    double amount;
    double balanceAfter;
};

// Structure to represent a loan with loan ID, customer name, loan amount, interest rate, duration, and remaining balance
struct Loan {
    int loanID;                 // Unique loan identifier generated automatically
    string customerName;        // Name of the loan customer
    double loanAmount;          // Original loan amount
    double interestRate;        // Interest rate for the loan in percentage
    int duration;               // Duration of the loan in months
    double remainingBalance;    // Remaining balance to be repaid
};

// Global vectors to store all accounts, loans, and transactions in memory
vector<Account> accounts;
vector<Loan> loanBook;
vector<Transaction> transactions;

// File names used to persist account, loan, and transaction data between program runs
const string accountsFile = "accounts.txt";
const string loanBookFile = "loanbook.txt";
const string transactionsFile = "transactions.txt";

// Function declarations for account management operations
void loadAccounts();
void saveAccounts();
bool accountNumberExists(int);
int findAccountIndexByNumber(int);
int findAccountIndexByName(const string&);
void createAccount();
void depositFunds();
void withdrawFunds();
void transferFunds();
void viewCurrentBalance();
void calculateAndAddInterest();
void closeAccount();
void listAllAccounts();
void deleteAllAccounts();
void searchAccount();
void freezeAccount();
void unfreezeAccount();
void viewTransactionHistory();
void loadTransactions();
void saveTransactions();
int generateTransactionID();
string getCurrentDateTime();

// Function declarations for loan management operations
void loadLoanBook();
void saveLoanBook();
int generateUniqueLoanID();
Loan* findLoanByID(int);
void createLoanAgreement();
void makeMonthlyRepayment();
void displayLoanBook();

int main() {
    loadAccounts();
    loadLoanBook();
    loadTransactions();

    int choice;
    do {
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
             << "14. Search for Account\n"
             << "15. Freeze Account\n"
             << "16. Unfreeze Account\n"
             << "17. View Transaction History\n"
             << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 0:
                saveAccounts();
                saveLoanBook();
                saveTransactions();
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
            case 14:
                searchAccount();
                break;
            case 15:
                freezeAccount();
                break;
            case 16:
                unfreezeAccount();
                break;
            case 17:
                viewTransactionHistory();
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 0);

    return 0;
}

void loadAccounts() {
    accounts.clear();
    ifstream inFile(accountsFile);
    if (!inFile) return;
    string line;
    while (getline(inFile, line)) {
        istringstream iss(line);
        Account acc;
        string name;
        int frozenInt;
        if (iss >> acc.accountNumber) {
            iss.ignore();
            getline(iss, name, '|');
            acc.customerName = trim(name);
            iss >> acc.balance >> acc.interestRate >> frozenInt;
            acc.isFrozen = (frozenInt == 1);
            accounts.push_back(acc);
        }
    }
    inFile.close();
}

void saveAccounts() {
    ofstream outFile(accountsFile);
    for (const auto& acc : accounts) {
        outFile << acc.accountNumber << " " << acc.customerName << "|" << " "
                << acc.balance << " " << acc.interestRate << " " << (acc.isFrozen ? 1 : 0) << "\n";
    }
    outFile.close();
}

bool accountNumberExists(int accountNumber) {
    for (const auto& acc : accounts) {
        if (acc.accountNumber == accountNumber) return true;
    }
    return false;
}

int findAccountIndexByNumber(int accountNumber) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].accountNumber == accountNumber) return i;
    }
    return -1;
}

int findAccountIndexByName(const string& name) {
    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].customerName == name) return i;
    }
    return -1;
}

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

    cout << "Enter annual interest rate (percent): ";
    cin >> newAcc.interestRate;
    cin.ignore();

    newAcc.isFrozen = false;

    accounts.push_back(newAcc);
    saveAccounts();

    cout << "Account created successfully.\n"
         << "Account Number: " << newAcc.accountNumber << "\n"
         << "Customer Name: " << newAcc.customerName << "\n"
         << "Balance: " << newAcc.balance << "\n"
         << "Interest Rate: " << newAcc.interestRate << "%\n";
}

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

    if (accounts[idx].isFrozen) {
        cout << "Account is frozen. Cannot perform deposit.\n";
        return;
    }

    cout << "Enter deposit amount: ";
    double amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid deposit amount.\n";
        return;
    }

    accounts[idx].balance += amount;

    // Log transaction
    Transaction t;
    t.transactionID = generateTransactionID();
    t.accountNumber = accNum;
    t.dateTime = getCurrentDateTime();
    t.type = "deposit";
    t.amount = amount;
    t.balanceAfter = accounts[idx].balance;
    transactions.push_back(t);

    saveAccounts();
    saveTransactions();

    cout << "Deposit successful. New balance: " << accounts[idx].balance << "\n";
}

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

    if (accounts[idx].isFrozen) {
        cout << "Account is frozen. Cannot perform withdrawal.\n";
        return;
    }

    cout << "Enter withdrawal amount: ";
    double amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid withdrawal amount.\n";
        return;
    }

    if (amount > accounts[idx].balance) {
        cout << "Insufficient funds.\n";
        return;
    }

    accounts[idx].balance -= amount;

    // Log transaction
    Transaction t;
    t.transactionID = generateTransactionID();
    t.accountNumber = accNum;
    t.dateTime = getCurrentDateTime();
    t.type = "withdrawal";
    t.amount = amount;
    t.balanceAfter = accounts[idx].balance;
    transactions.push_back(t);

    saveAccounts();
    saveTransactions();

    cout << "Withdrawal successful. New balance: " << accounts[idx].balance << "\n";
}

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

    if (accounts[srcIdx].isFrozen) {
        cout << "Source account is frozen. Cannot perform transfer.\n";
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

    if (accounts[destIdx].isFrozen) {
        cout << "Destination account is frozen. Cannot receive transfer.\n";
        return;
    }

    cout << "Enter transfer amount: ";
    double amount;
    cin >> amount;
    cin.ignore();

    if (amount <= 0) {
        cout << "Invalid transfer amount.\n";
        return;
    }

    if (amount > accounts[srcIdx].balance) {
        cout << "Insufficient funds in source account.\n";
        return;
    }

    accounts[srcIdx].balance -= amount;
    accounts[destIdx].balance += amount;

    // Log transactions for both accounts
    Transaction tOut;
    tOut.transactionID = generateTransactionID();
    tOut.accountNumber = srcAccNum;
    tOut.dateTime = getCurrentDateTime();
    tOut.type = "transfer_out";
    tOut.amount = amount;
    tOut.balanceAfter = accounts[srcIdx].balance;
    transactions.push_back(tOut);

    Transaction tIn;
    tIn.transactionID = generateTransactionID();
    tIn.accountNumber = destAccNum;
    tIn.dateTime = getCurrentDateTime();
    tIn.type = "transfer_in";
    tIn.amount = amount;
    tIn.balanceAfter = accounts[destIdx].balance;
    transactions.push_back(tIn);

    saveAccounts();
    saveTransactions();

    cout << "Transfer successful.\n"
         << "Source account new balance: " << accounts[srcIdx].balance << "\n"
         << "Destination account new balance: " << accounts[destIdx].balance << "\n";
}

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

    double interest = accounts[idx].balance * (accounts[idx].interestRate / 100.0);
    accounts[idx].balance += interest;
    saveAccounts();

    cout << "Interest added. New balance: " << accounts[idx].balance << "\n";
}

void closeAccount() {
    cout << "Enter account number to close: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    for (size_t i = 0; i < accounts.size(); ++i) {
        if (accounts[i].accountNumber == accNum) {
            accounts.erase(accounts.begin() + i);
            saveAccounts();
            cout << "Account closed successfully.\n";
            return;
        }
    }
    cout << "Account not found.\n";
}

void listAllAccounts() {
    if (accounts.empty()) {
        cout << "No accounts found.\n";
        return;
    }

    system("clear");

    cout << "Accounts List:\n";
    for (const auto& a : accounts) {
        cout << "Account Number: " << a.accountNumber << "\n"
             << "Customer Name: " << a.customerName << "\n"
             << "Balance: " << a.balance << "\n"
             << "Interest Rate: " << a.interestRate << "%\n"
             << "Status: " << (a.isFrozen ? "Frozen" : "Active") << "\n"
             << "-------------------------\n";
    }
    sleep(5);
}

void deleteAllAccounts() {
    accounts.clear();
    saveAccounts();
    cout << "All accounts deleted.\n";
}

void searchAccount() {
    cout << "Search by:\n1. Account Number\n2. Account Holder's Name\nEnter choice: ";
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        cout << "Enter account number: ";
        int accNum;
        cin >> accNum;
        cin.ignore();

        int idx = findAccountIndexByNumber(accNum);
        if (idx == -1) {
            cout << "Account not found.\n";
            return;
        }

        const Account& acc = accounts[idx];
        cout << "Account found:\n"
             << "Account Number: " << acc.accountNumber << "\n"
             << "Customer Name: " << acc.customerName << "\n"
             << "Balance: " << acc.balance << "\n"
             << "Interest Rate: " << acc.interestRate << "%\n"
             << "Status: " << (acc.isFrozen ? "Frozen" : "Active") << "\n";
    } else if (choice == 2) {
        cout << "Enter account holder's name: ";
        string name;
        getline(cin, name);
        name = trim(name);

        int idx = findAccountIndexByName(name);
        if (idx == -1) {
            cout << "Account not found.\n";
            return;
        }

        const Account& acc = accounts[idx];
        cout << "Account found:\n"
             << "Account Number: " << acc.accountNumber << "\n"
             << "Customer Name: " << acc.customerName << "\n"
             << "Balance: " << acc.balance << "\n"
             << "Interest Rate: " << acc.interestRate << "%\n"
             << "Status: " << (acc.isFrozen ? "Frozen" : "Active") << "\n";
    } else {
        cout << "Invalid choice.\n";
    }
}

void freezeAccount() {
    cout << "Enter account number to freeze: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    if (accounts[idx].isFrozen) {
        cout << "Account is already frozen.\n";
        return;
    }

    accounts[idx].isFrozen = true;
    saveAccounts();
    cout << "Account frozen successfully.\n";
}

void unfreezeAccount() {
    cout << "Enter account number to unfreeze: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    if (!accounts[idx].isFrozen) {
        cout << "Account is not frozen.\n";
        return;
    }

    accounts[idx].isFrozen = false;
    saveAccounts();
    cout << "Account unfrozen successfully.\n";
}

void viewTransactionHistory() {
    cout << "Enter account number to view transaction history: ";
    int accNum;
    cin >> accNum;
    cin.ignore();

    int idx = findAccountIndexByNumber(accNum);
    if (idx == -1) {
        cout << "Account not found.\n";
        return;
    }

    cout << "Transaction History for Account Number: " << accNum << "\n";
    cout << "ID\tDate & Time\t\tType\t\tAmount\tBalance After\n";
    cout << "-----------------------------------------------------------------\n";

    bool found = false;
    for (const auto& t : transactions) {
        if (t.accountNumber == accNum) {
            cout << t.transactionID << "\t" << t.dateTime << "\t" << t.type << "\t\t"
                 << t.amount << "\t" << t.balanceAfter << "\n";
            found = true;
        }
    }

    if (!found) {
        cout << "No transactions found for this account.\n";
    }
}

void loadTransactions() {
    transactions.clear();
    ifstream inFile(transactionsFile);
    if (!inFile) return;
    string line;
    while (getline(inFile, line)) {
        istringstream iss(line);
        Transaction t;
        if (iss >> t.transactionID >> t.accountNumber) {
            iss.ignore();
            getline(iss, t.dateTime, '|');
            iss >> t.type >> t.amount >> t.balanceAfter;
            transactions.push_back(t);
        }
    }
    inFile.close();
}

void saveTransactions() {
    ofstream outFile(transactionsFile);
    for (const auto& t : transactions) {
        outFile << t.transactionID << " " << t.accountNumber << " " << t.dateTime << "| "
                << t.type << " " << t.amount << " " << t.balanceAfter << "\n";
    }
    outFile.close();
}

int generateTransactionID() {
    int maxID = 0;
    for (const auto& t : transactions) {
        if (t.transactionID > maxID) maxID = t.transactionID;
    }
    return maxID + 1;
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
             ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    return string(buffer);
}

// Loan-related functions remain unchanged (omitted here for brevity)
void loadLoanBook() {
    loanBook.clear();
    ifstream inFile(loanBookFile);
    if (!inFile) return;
    string line;
    while (getline(inFile, line)) {
        istringstream iss(line);
        Loan loan;
        string name;
        if (iss >> loan.loanID) {
            iss.ignore();
            getline(iss, name, '|');
            loan.customerName = trim(name);
            iss >> loan.loanAmount >> loan.interestRate >> loan.duration >> loan.remainingBalance;
            loanBook.push_back(loan);
        }
    }
    inFile.close();
}

void saveLoanBook() {
    ofstream outFile(loanBookFile);
    for (const auto& loan : loanBook) {
        outFile << loan.loanID << " " << loan.customerName << "|" << " "
                << loan.loanAmount << " " << loan.interestRate << " "
                << loan.duration << " " << loan.remainingBalance << "\n";
    }
    outFile.close();
}

int generateUniqueLoanID() {
    int maxID = 0;
    for (const auto& loan : loanBook) {
        if (loan.loanID > maxID) maxID = loan.loanID;
    }
    return maxID + 1;
}

Loan* findLoanByID(int id) {
    for (auto& loan : loanBook) {
        if (loan.loanID == id) return &loan;
    }
    return nullptr;
}

void createLoanAgreement() {
    Loan newLoan;
    newLoan.loanID = generateUniqueLoanID();

    cout << "Enter customer name: ";
    string name;
    getline(cin, name);
    newLoan.customerName = trim(name);

    cout << "Enter loan amount: ";
    cin >> newLoan.loanAmount;

    cout << "Enter interest rate (percent): ";
    cin >> newLoan.interestRate;

    cout << "Enter duration (months): ";
    cin >> newLoan.duration;

    newLoan.remainingBalance = newLoan.loanAmount;

    loanBook.push_back(newLoan);
    saveLoanBook();

    system("clear");

    cout << "Loan agreement created successfully.\n"
         << "Loan ID: " << newLoan.loanID << "\n"
         << "Customer Name: " << newLoan.customerName << "\n"
         << "Loan Amount: " << newLoan.loanAmount << "\n"
         << "Interest Rate: " << newLoan.interestRate << "%\n"
         << "Duration: " << newLoan.duration << " months\n"
         << "Remaining Balance: " << newLoan.remainingBalance << "\n"
         << "-------------------------\n";

    sleep(5);
}

void makeMonthlyRepayment() {
    cout << "Enter loan ID for repayment: ";
    int id;
    cin >> id;
    cin.ignore();

    Loan* loan = findLoanByID(id);
    if (!loan) {
        cout << "Loan ID not found.\n";
        return;
    }

    cout << "Current remaining balance: " << loan->remainingBalance << "\n";
    cout << "Enter repayment amount: ";
    double repayment;
    cin >> repayment;

    if (repayment > loan->remainingBalance) {
        cout << "Repayment amount exceeds remaining balance. Transaction cancelled.\n";
        return;
    }

    loan->remainingBalance -= repayment;
    saveLoanBook();

    cout << "Repayment successful. Updated remaining balance: " << loan->remainingBalance << "\n";
}

void displayLoanBook() {
    if (loanBook.empty()) {
        cout << "Loan book is empty.\n";
        return;
    }

    cout << "Loan Book:\n";
    for (const auto& loan : loanBook) {
        cout << "Loan ID: " << loan.loanID << "\n"
             << "Customer Name: " << loan.customerName << "\n"
             << "Loan Amount: " << loan.loanAmount << "\n"
             << "Interest Rate: " << loan.interestRate << "%\n"
             << "Duration: " << loan.duration << " months\n"
             << "Remaining Balance: " << loan.remainingBalance << "\n"
             << "-------------------------\n";
    }
    system("clear");
    sleep(5);
}
