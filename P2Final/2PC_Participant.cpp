/**
 * @file 2PC_Participant.cpp definition for Participant class
 * @author Nadezhda Chernova
 */

#include "2PC_Participant.h"
#include "Protocol.h"
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;

Participant::Participant(u_short serve_port, const string &accounts_filename,
                         const string &log_filename) : TCPServer(serve_port),
                                                       accounts_filename(
                                                               accounts_filename),
                                                       log_filename(
                                                               log_filename) {
    readAccounts(); // read an accounts text file on startup
}

Participant::~Participant() {
    log("Shutting down gracefully");
}

void Participant::stop() {
    log("Stopping server and rolling back changes");
    rollback();
    TCPServer::stopServer();
}

void Participant::readAccounts() {
    string account; // account number from file
    string balance; // balance from file
    double bal;     //  balance in another format for map

    // Open accounts file
    ifstream inputFile(accounts_filename);
    if (!inputFile.is_open()) {
        throw runtime_error{
                "Unable to open accounts file " + accounts_filename};
    }

    // Read each line from file
    while (getline(inputFile, balance, ' ')) {
        getline(inputFile, account);
        stringstream stream(balance); // convert balance to double (bal)
        if (!(stream >> bal)) {
            throw runtime_error("Invalid format of accounts file");
        }
        accounts[account] = bal; //  store account number and balance in map
    }

    // Close file
    inputFile.close();
}

void Participant::log(const string &message) {
    cout << message << endl;
    // Open in append mode and write to log_filename
    ofstream logFile(log_filename, std::ios::app);
    if (!logFile) {
        throw runtime_error("Unable to open log file");
    }
    logFile << message << endl;
    logFile.close();
}

void Participant::start_client(const string &their_host,
                               u_short their_port) {
    string message = "Accepted coordinator connection. State: INIT";
    log(message);
   }

bool Participant::process(const string &request) {
    string command, account;
    double amount;
    ostringstream note;
    stringstream stream(request);

    // Parse the command, account, and amount from request
    stream >> command >> account >> amount;

    Protocol protocol = toProtocol(command);
    switch (protocol) {

        case VOTE_REQUEST:
            return processVoteRequest(command, account, amount);

        case GLOBAL_COMMIT:
            processGlobalCommit(command);
            return false; // close communication

        case GLOBAL_ABORT:
            processGlobalAbort(command);
            return false; // close communication

        case UNKNOWN_PROTOCOL:
        default:
            log("Invalid command received: " + command);
            respond(toString(UNKNOWN_PROTOCOL));
            return false;
    }
}

string Participant::formatAmount(double amount) {
    ostringstream stream;
    stream << fixed << setprecision(2) << amount;
    return stream.str();
}

bool Participant::processVoteRequest(const string &command,
                                     const string &account,
                                     const double amount) {
    string formattedAmount = formatAmount(amount);

    // Withdraw
    // got VOTE-REQUEST and approve, place hold and reply VOTE-COMMIT
    if (amount < 0) {

        if (accounts.find(account) != accounts.end() &&
            accounts[account] >= -amount) {
            holding[account] = amount;
            log("Holding " + formattedAmount + " from account " +
                account);
            log("Got " + command + ", replying VOTE-COMMIT. State: READY");
            respond(toString(VOTE_COMMIT));
            return true;

            // got VOTE-REQUEST and don't approve, reply VOTE-ABORT
        } else {
            log("Got " + command + ", replying VOTE-ABORT. State: ABORT");
            if (accounts.find(account) != accounts.end()) {
                log("Releasing hold from account " + account);
            }
            respond(toString(VOTE_ABORT));
            return false; // close communication
        }

        // Deposit
        // got VOTE-REQUEST and approve, place hold and reply VOTE-COMMIT
    } else {

        if (accounts.find(account) != accounts.end()) {
            holding[account] = amount;
            log("Holding " + formattedAmount + " for account " + account);
            log("Got " + command + ", replying VOTE-COMMIT. State: READY");
            respond(toString(VOTE_COMMIT));
            return true;

            // got VOTE-REQUEST and don't approve, reply VOTE-ABORT without hold.
        } else {
            log("Got " + command + ", replying VOTE-ABORT. State: ABORT");
            respond(toString(VOTE_ABORT));
            return false;
        }
    }
}

void Participant::processGlobalCommit(const string &command) {
    log("Got " + command + ", replying ACK. State: COMMIT");
    string account;

    // Iterate through holding map to find account
    for (const auto &val: holding) {
        account = val.first;
    }

    // Check if the account exists in holding map
    auto it = holding.find(account);
    bool isFound = (it != holding.end());

    // Update balance
    if (isFound) {
        accounts[account] += it->second; // real withdraw or deposit
        log("Committing " + formatAmount(it->second) + " for account " +
            account);
    }
    updateAccountsFile();
    respond(toString(ACK));
}

void Participant::processGlobalAbort(const string &command) {
    log("Got " + command + ", replying ACK. State: ABORT");
    log("Releasing hold from account");
    rollback(); // clear holdings + reload account file
    respond("ACK");
}

void Participant::updateAccountsFile() {
    ofstream accountsFile(accounts_filename);
    if (!accountsFile) {
        throw runtime_error("Unable to open accounts file");
    }
    for (const auto &account: accounts) {
        accountsFile << account.second << " " << account.first << endl;
    }
    accountsFile.close();
}

void Participant::rollback() {
    holding.clear(); // clear all holding amounts
    readAccounts();  // reload account file
    log("Rollback complete");
}
