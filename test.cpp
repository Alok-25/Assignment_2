#include <iostream>
#include <map>
#include <vector>
#include <ctime>
#include <chrono>
#include <thread>
#include <functional>
#include "unistd.h"
using namespace std;
vector <long long> value_table(26);
int saving_account_number = 1000000;
int current_account_number = 2000000;
int loan_account_number = 3000000;


// Enum for Account Type
enum class AccountType {
    Savings,
    Current,
    Loan
};

// Enum for Loan Type
enum class LoanType {
    HomeLoan,
    CarLoan,
    PersonalLoan,
    BusinessLoan
};

// Class for User
class User {
public:
    string username;
    string mobile_number;
    string email;
    string address;
    long long customer_id;
    int age;
};

// Class for Account
class Account {
public:
    long long account_number;
    long long customer_id;
    double amount;
    string opening_date;
    int year;
    int month;
    int day;
    string card_number="You dont have an atm card";
    int card_cvv;
    string card_expiry_date="You dont have an atm card";
};


// Class for Savings Account
class SavingsAccount : public Account {
public:
    static const double interest_rate;
    static const int interest_rate_duration;
    
    static const double minimum_amount;
    static const int max_withdrawals_atm;
    static const int max_withdrawal_amount_atm;
    static const int charge_after_max_atm;
    static const int max_withdrawal_amount;
    static const double nrv_threshold;
    static const double nrv_charge;
    static const string card_number;
  
};

const double SavingsAccount::interest_rate = 0.06;
const int SavingsAccount::interest_rate_duration=30;

const double SavingsAccount::minimum_amount = 10000.0;
const int SavingsAccount::max_withdrawals_atm = 5;
const int SavingsAccount::max_withdrawal_amount_atm = 20000;
const int SavingsAccount::charge_after_max_atm = 500;
const int SavingsAccount::max_withdrawal_amount = 50000;
const double SavingsAccount::nrv_threshold = 100000.0;
const double SavingsAccount::nrv_charge = 1000.0;

// Class for Current Account
class CurrentAccount : public Account {
public:
    static const double minimum_amount;
    static const int min_transaction_per_month;
    static const double nrv_threshold;
    static const double nrv_charge;
    static const double transaction_charge_percentage;
    static const double max_transaction_charge;
    static const int minimum_age;
};

const double CurrentAccount::minimum_amount = 100000.0;
const int CurrentAccount::min_transaction_per_month = 3;
const int CurrentAccount::minimum_age=18;
const double CurrentAccount::nrv_threshold = 500000.0;
const double CurrentAccount::nrv_charge = 5000.0;
const double CurrentAccount::transaction_charge_percentage = 0.005;
const double CurrentAccount::max_transaction_charge = 500.0;

// Class for Loan Account
class LoanAccount {
public:
    long long account_number;
    long long customer_id;
    LoanType loan_type;
    double loan_amount;
    double interest_rate;
    int duration_years;
    static const double minimum_loan_amount;
    static const double maximum_loan_percentage;
    static const int minimum_age;
};

const double LoanAccount::minimum_loan_amount = 500000.0;
const double LoanAccount::maximum_loan_percentage = 0.4;
const int LoanAccount::minimum_age=25;


vector<int> day(){
    time_t now = time(0);
            tm* ltm = localtime(&now);
            vector<int> date;
            date.push_back(1900 + ltm->tm_year);
            date.push_back(1 + ltm->tm_mon);
            date.push_back(ltm->tm_mday);
    return date;
            
}





// Bank class
class Bank {
public:
    map<string, long long> customer_ids; // map for customer name to customer_id
    map<long long, User> users;          // map for customer_id to Users object
    map<long long, vector<Account>> accounts; // map for customer_id to accounts
    map <long long ,pair<pair<double,string>,pair<bool,bool>>> transactions;//map for account to their transactions
    //x.first.first=amount , x.first.second =day, s.second.first =isWithdrwal, s.second.second =isDirect

public:

    

    

    void create_value_table() {
        value_table[0] = 1;
        for (int i = 1; i < 26; i++) {
            value_table[i] = (value_table[i - 1] * 2) + i + 1;
        }
        
    }

    long long int generateCustomerID(string username) {
        create_value_table();
        long long int account_number = 0;
        for (int i = 0; i < username.length(); i++) {
            int temp = username[i] - 'A';
            if (temp <= 25) {
                account_number += value_table[temp];
            } else {
                temp = username[i] - 'a';
                account_number += value_table[temp];
            }
        }
        
        return account_number;
    }


    // Function to create a new user
    void createUser(const string& username, const string& mobile_number, const string& email,
        const string& address, int age) {
        long long customer_id = generateCustomerID(username);
        User new_user;
        new_user.username = username;
        new_user.mobile_number = mobile_number;
        new_user.email = email;
        new_user.address = address;
        new_user.customer_id = customer_id;
        new_user.age = age;
        
        customer_ids[username] = customer_id;
        users[customer_id] = new_user;
    }
    void interest(long long customer_id){

        vector<Account>& customer_accounts = accounts[customer_id];
        for(auto x : customer_accounts){
            performTransaction(users[customer_id].username,x.account_number, x.amount*SavingsAccount::interest_rate/12,true,true);
            cout<<"Interest has been credited. Your account balance is :" <<x.amount<<endl;

        }
        
    }
    void nrv(long long customer_id){
        vector<Account>& customer_accounts = accounts[customer_id];
        for(auto x:customer_accounts){
            if(x.account_number/1000000 ==1){
                if(x.amount < SavingsAccount::nrv_threshold)
                performTransaction(users[customer_id].username, x.account_number, SavingsAccount::nrv_charge ,true,true);
                
                }

            else if (x.account_number/1000000 ==2){
                if(x.amount < CurrentAccount::nrv_threshold)
                performTransaction(users[customer_id].username, x.account_number, CurrentAccount::nrv_charge ,true,true);
                
                }
            
        }
    }

    // Function to open an account
    void openAccount() {
        Account new_account;
        
        string username="";
        AccountType account_type;
        double amount=0;
        cout<<"Enter your username: "<<endl;
        cin>>username; cout<<endl;
        cout<<"Press 1 to create a savings account,Press 2 to create a current account,Press 3 to create a loan account "<<endl;
        int t;
        cin>>t;
        if(t==1) account_type=AccountType::Savings;
        else if(t==2) account_type=AccountType::Current;
        else account_type=AccountType::Loan;

        if (account_type==AccountType::Savings){
            cout<<"Please enter the amount you want to credit in your savings account"<<endl;
            cin>>amount; cout<<endl;
            
            while(amount < SavingsAccount::minimum_amount){
                cout<<"Enter amount greater than "<<SavingsAccount::minimum_amount<<endl;
                cin>>amount;
                
            }
            string card_number;
            for(int i=0;i<16;i++){
                char temp= 48 + (rand() % 9);
                card_number=card_number+temp;
            }
            new_account.card_number=card_number;
            vector<int> date =day();
            new_account.card_expiry_date=to_string(date[0]) + "/" + to_string(date[1]) +
                "/" + to_string(date[2]+3);//expires in 3 years after getting atm card
            new_account.card_cvv= 100 +(rand()%900);

            

            
        }
        if (account_type==AccountType::Current){
            cout<<"Please enter the amount you want to credit in your current account"<<endl;
            cin>>amount; cout<<endl;
            while(amount < CurrentAccount::minimum_amount){
                cout<<"Enter amount greater than "<<CurrentAccount::minimum_amount<<endl;
                cin>>amount;
                
            }
        }
        else if(t==3){
            
            if(customer_ids.find(username) != customer_ids.end()){
                long long customer_id=customer_ids[username];
            if(users[customer_id].age<LoanAccount::minimum_age){
                cout<<"You should be "<< LoanAccount::minimum_age<<" or above in age to open a loan account"<<endl;
                username="";
            } 
            else {
                cout<<"Please enter the amount of loan you want to take"<<endl;
                cin>>amount; cout<<endl;
                
                while(amount < LoanAccount::minimum_loan_amount){
                    cout<<"Enter amount greater than "<<LoanAccount::minimum_loan_amount<<endl;
                    cin>>amount;
                    
                }
            }
        }
        else {
            cout<<"You need to have a savings or current account before opening a new loan account."<<endl;
            username="";
            
        }
        }
        if(username!=""){
            long long customer_id = customer_ids[username];
            vector<int> date=day();
            new_account.day=date[0];
            new_account.month=date[1];
            new_account.year=date[2];
            new_account.opening_date=to_string(date[0]) + "/" + to_string(date[1]) +
                "/" + to_string(date[2]);


            
            new_account.customer_id = customer_id;
            new_account.amount = amount;
            if(account_type == AccountType::Savings ) new_account.account_number=saving_account_number++;
            else if (account_type == AccountType::Current) new_account.account_number=current_account_number++;
            else new_account.account_number=loan_account_number++;
            

            // Get current date
            time_t now = time(0);
            tm* ltm = localtime(&now);
            new_account.year = 1900 + ltm->tm_year;
            new_account.month = 1 + ltm->tm_mon;
            new_account.day = ltm->tm_mday;
            new_account.opening_date = to_string(new_account.day) + "/" + to_string(new_account.month) +
                "/" + to_string(new_account.year);

            vector<Account>& customer_accounts = accounts[customer_id];
            accounts[customer_id].push_back(new_account);

            cout << "Account opened successfully!" << endl;
        }
    }
    

    // Function to get account details for a user
    void getAccountDetails(const string& username) {
        long long customer_id = customer_ids[username];
        const vector<Account>& customer_accounts = accounts[customer_id];

        cout << "Account Details for User: " << username << endl;
        cout<<customer_accounts.size()<< endl;
        for (auto account : customer_accounts) {
            
            cout << "Account Number: " << account.account_number << endl;
            cout << "Amount: " << account.amount << endl;
            cout << "Opening Date: " << account.opening_date << endl;
            cout << "Customer ID: " << account.customer_id << endl;
            cout << "--------------------------" << endl;
        }
    }

    // Function to perform a transaction (withdrawal or deposit) for a user
    void performTransaction(const string& username, int account_number, double amount, bool isWithdrawal, bool isDirect) {
        long long customer_id = customer_ids[username];
        vector<Account>& customer_accounts = accounts[customer_id];
        transactions[account_number].first.first = amount;
        vector<int> date= day();
        transactions[account_number].first.second = to_string(date[0]) + "/" + to_string(date[1]) +
                "/" + to_string(date[2]);
        transactions[account_number].second.first=isWithdrawal;
        transactions[account_number].second.first=isDirect;

        
        for (Account& account : customer_accounts) {
            
            if (account.account_number == account_number) {
                if (isWithdrawal) {
                    if (account.amount >= amount) {
                        account.amount -= amount;
                        cout << "Withdrawal of " << amount << " successful!" << endl;
                    } else {
                        cout << "Insufficient funds for withdrawal!" << endl;
                    }
                } else {
                    account.amount += amount;
                    cout << "Deposit of " << amount << " successful!" << endl;
                }
                return;
            }
        }

        
    }

    void jobScheduler()
    {
    
    std::time_t now = std::time(nullptr);
    std::tm* current = std::localtime(&now);

    
    if (current->tm_mday == 19 || (current->tm_mday == 30 && (current->tm_mon == 0 || current->tm_mon == 2 || current->tm_mon == 4 ||
        current->tm_mon == 6 || current->tm_mon == 7 || current->tm_mon == 9 || current->tm_mon == 11)) ||
        (current->tm_mday == 29 && current->tm_mon == 1 && (current->tm_year % 4 == 0 && (current->tm_year % 100 != 0 || current->tm_year % 400 == 0))) ||
        (current->tm_mday == 28 && current->tm_mon == 1 && !(current->tm_year % 4 == 0 && (current->tm_year % 100 != 0 || current->tm_year % 400 == 0))))
    {
        // Execute the end-of-month job
        for (auto x : accounts){
            interest(x.first);

            nrv(x.first);
        }
        
    }
    
}
};

void existing_users(Bank bank){
    cout<<"Please enter your username"<<endl;
        string username;
        cin>>username;
        if(bank.customer_ids.find(username) != bank.customer_ids.end()){
            cout<<"Welcome "<<username<<endl;
            int act_type;
            cout<<"Press 1 if you want to make an account"<<endl;
            cout<<"Press 2 if you want to see account details"<<endl;
            cout<<"Press 3 if you want to perform a transaction"<<endl;
            cin>> act_type;
            if(act_type ==1){
                bank.openAccount();

            }
            else if (act_type ==2){
                bank.getAccountDetails(username);
            }
            else if (act_type == 3){
                double amount;
                long long account_number;
                bool isWithdraw,isDirect;
                cout<<"Enter account number"<<endl;
                cin>>account_number;
                cout<<"Enter amount"<<endl;
                cin>>amount;
                cout<<"Press 1 if the transaction is debiting else press 2"<<endl;
                int deb;cin>>deb;
                if(deb==1) isWithdraw=true;
                else if(deb==2) isWithdraw=false;

                cout<<"Press 1 if you want to preform the transaction using ATM Card,else press 2 for direct transaction"<<endl;
                int atm; cin>>atm;
                if(atm==1) isDirect=false;
                else if (atm ==2) isDirect = true;

                bank.performTransaction(username,account_number,amount,isWithdraw,isDirect);
                
            }
            existing_users(bank);
        
        }
}

int main() {

    
    Bank bank;
    int user_type;
    cout<<"Press 1 if you have an existing account"<<endl;
    cout<<"Press 2 if you want to create a new account"<<endl;
    cin>>user_type;
    if(user_type == 1 ){
        existing_users(bank);
        
        }
    
    if(user_type == 2){
        //Create a user
        string username,mobile_no,email,address;
        int age;
        cout<<"Enter username"<<endl; cin>>username;
        cout<<"Enter mobile_no."<<endl;cin>>mobile_no;
        cout<<"Enter email"<<endl;cin>>email;
        cout<<"Enter address"<<endl;cin>>address;
        cout<<"Enter age"<<endl;cin>>age;
        
        bank.createUser("Alok", "1234567890", "jsdasd@example.com", "123 Main St", 30);
        cout<<"User created"<<endl;

        existing_users(bank);
    }
    
    // running for interest updates
    bank.jobScheduler();
    std::this_thread::sleep_for(std::chrono::hours(24));
    return 0;
}
