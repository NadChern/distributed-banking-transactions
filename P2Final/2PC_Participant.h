/**
 * @file 2PC_Participant.h declaration for Participant class
 * @author Nadezhda Chernova
 */

#pragma once

#include <fstream>
#include <sstream>
#include "TCPServer.h"
#include <unordered_map>
using namespace std;

/**
 * @class
 * Participant class is a derived class of TCPServer class, it inherits all the
 * functionalities of TCPServer and overrides the start_client and process
 * methods.
 * The class includes its own public and private helper methods for handling
 * bank transactions, representing a running 2-phase commit participant (bank).
 * It listens for calls from a coordinator and responds according to the FSM
 * protocol. The class includes methods for checking accounts,  withdrawing or
 * depositing money, aborting transactions if no account exists or insufficient
 * funds, and mechanisms to recover from crashes or connection failures.
 */
class Participant : public TCPServer {
public:
    /**
     * Constructs Participant object and initializes the TCP server
     * @param serve_port port number on which server listens
     * @param accounts_filename filename where account info is stored
     * @param log_filename filename where transaction logs are stored
     */
    explicit Participant(u_short serve_port,
                         const string &accounts_filename,
                         const string &log_filename);

    /**
     * Destructor
     */
    ~Participant() override;

    /**
     * Logs message (step, transaction made) to log file.
     * Each action taken in the FSM is appended to a log file.
     *
     * @param message message to be logged
     * @throws runtime_error If file cannot be opened
     */
    void log(const string &message);

    /**
     * Stops server and rolls back changes
     */
    void stop();

    /**
     * Rolls back any uncommitted changes to the accounts.
     * Used to undo any changes that have not been committed. It clears
     * holding amounts, which are temporary changes that have not been
     * finalized, and reloads account information from accounts file.
     * This ensures that account balances are restored to their original
     * state before changes.
     * Log message indicating the completion of the rollback is recorded.
     */
    void rollback();

protected:
    /**
     * Logs message indicating acceptance of the connection.
     * This method is part of the participant's setup to handle communication
     * with the coordinator.
     * @param their_host coordinator's host address
     * @param their_port coordinator's port number
     */
    void
    start_client(const string &their_host, u_short their_port) override;

    /**
     * Processes a received request from the coordinator.
     * Process method handles various types of requests from the coordinator
     * as part of the 2-phase commit protocol. It parses the received request,
     * determines the type of protocol command, and calls the appropriate method
     * to handle the command. Protocol enum used.
     *
     * Supported Commands:
     * - VOTE-REQUEST: Calls processVoteRequest.
     * - GLOBAL-COMMIT: Calls processGlobalCommit.
     * - GLOBAL-ABORT: Calls processGlobalAbort.
     * - UNKNOWN_PROTOCOL: Logs and responds with invalid command message.
     * @param request command from coordinator
     * @return true if the server should continue processing requests,
     * false to stop.
     */
    bool process(const string &request) override;

private:
    string accounts_filename; // filename for stored account info
    string log_filename; // filename for stored transaction logs
    unordered_map<string, double> accounts; // map of accounts to balances
    unordered_map<string, double> holding;  // map of accounts to holding amount

    /**
     * Opens the accounts file, reads each line to extract account  numbers and
     * balances, and stores them in the accounts map.
     * Each line in the file is expected to contain a balance and an account
     * number separated by a space.
     *
     * @throws runtime_error If file cannot be opened/if file format is invalid.
     */
    void readAccounts();

    /**
     * Updates accounts file with current account information
     * @throws runtime_error If file cannot be opened
     */
    void updateAccountsFile();

    /**
     * Formats an amount as a string with two decimal places
     * @param amount to format
     * @return formatted amount as string
     */
    string formatAmount(double amount);

    /**
     * Processes VOTE-REQUEST command
     * @param command received from coordinator
     * @param account account involved in request
     * @param amount to deposit or withdraw (depends on the sign)
     * @return true if request was approved, false otherwise
     */
    bool processVoteRequest(const string &command,
                            const string &account,
                            double amount);

    /**
     * Processes GLOBAL-ABORT command.
     * This method is invoked if the participant did not already abort
     * transaction in a previous phase. Upon receiving this command,
     * participant acknowledges the abort ("ACK" message) and releases any
     * holds on account balances by rolling back the transaction.
     * @param command received from coordinator
     */
    void processGlobalAbort(const string &command);

    /**
     * Processes GLOBAL-COMMIT command.
     * Updates account balance in accounts map by applying held amount,
     * updates accounts file, responds with an "ACK" message to acknowledge
     * commit.
     * @param command
     */
    void processGlobalCommit(const string &command);
};


