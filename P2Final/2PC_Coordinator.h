/**
 * @file 2PC_Coordinator.h declaration for Coordinator class
 * @author Nadezhda Chernova
 */

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include "TCPClient.h"

using namespace std;

/**
 * @class Coordinator class
 * The Coordinator class is responsible for managing 2-phase commit protocol,
 * which involves coordinating transactions between two participant banks.
 * It sends transaction requests, collects votes from participants, and
 * decides whether to commit or abort the transaction based on the received
 * votes.
 * Class uses TCPClient for each connection it makes to the participants.
 */
class Coordinator {
public:

/**
 * @enum ParticipantState defines the different states of a participants in
 * the 2-phase commit protocol.  These states are used for tracking participants
 * states and sending proper messages to the participants.
 */
    enum ParticipantState {
        INIT,
        ABORT,
        COMMIT
    };

    /**
     * Constructs Coordinator object with specified log file.
     * @param logFilename filename where logs will be stored
     * @throws runtime_error if log file cannot be opened, file is not writable
     */
    explicit Coordinator(const string &logFilename);

    /**
     * Destructor
     */
    ~Coordinator();

    /**
     * Initiates connection with participants using TCPClient, sends them
     * vote requests and then decides whether to commit or abort transaction
     * based on their responses.
     * @param accountFrom account from which the amount is to be transferred
     * @param accountTo account to which the amount is to be transferred
     * @param amount amount to be transferred
     * @param participants vector of pairs with host and port of
     * each participant
     */
    void callParticipants(const string &accountFrom, const string &accountTo,
                          double amount,
                          const vector<pair<string, u_short>> &participants);

    /**
     * Adds a participant to the coordinator's list
     * @param host host address of the participant
     * @param port port number of the participant
     */
    void addParticipant(const string &host, u_short port);

    /**
     * Logs message (step, transaction made) to log file.
     * Each action taken in the FSM is appended to a log file.
     *
     * @param message message to be logged
     * @throws runtime_error If file cannot be opened
     */
    void log(const string &message);

private:
    string logFilename; // filename where logs will be stored

    // Tuple Access: used get<0>, get<1>, get<2>, and get<3> to access
    // TCPClient, host, port, and state respectively in the tuple.
    vector<tuple<TCPClient, string, u_short, ParticipantState>> participants;

    /**
     * Sends vote request to participants and processes their responses.
     * @param amount amount to be transferred
     * @param accountFrom account from which the amount is to be transferred
     * @param accountTo account to which the amount is to be transferred
     * @return returns true if all participants agree to commit the transaction,
     * false otherwise
     */
    bool sendVoteRequest(double amount, const string &accountFrom,
                         const string &accountTo);

    /**
     * Processes response received from a participant.
     * @param response from participant
     * @return Returns true if response is VOTE_COMMIT, false if response is
     * VOTE_ABORT or an invalid response.
     */
    bool processResponse(const string &response);

    /**
     * Sends a GLOBAL-COMMIT message to participants and processes their
     * acknowledgements.
     * Transaction is committed if coordinator received ACK for all
     * participants, otherwise transaction is considered aborted.
     */
    void sendGlobalCommit();

    /**
     * Sends a GLOBAL-ABORT message to all participants that have not
     * already sent an abort response.
     */
    void sendGlobalAbort();

     /**
     * Formats an amount as a string with two decimal places
     * @param amount to format
     * @return formatted amount as string
     */
    string formatAmount(double amount);
};


