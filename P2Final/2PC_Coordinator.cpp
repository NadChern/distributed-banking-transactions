/**
 * @file 2PC_Coordinator.cpp definition for Coordinator class
 * @author Nadezhda Chernova
 */
#include <vector>
#include <iomanip>
#include <string>
#include <stdexcept>
#include <tuple>
#include <fstream>
#include <sstream>
#include <iostream>
#include "TCPClient.h"
#include "Protocol.h"
#include "2PC_Coordinator.h"


using namespace std;

Coordinator::Coordinator(const string &logFilename) {
    // Open the log file in append mode
    ofstream logFile(logFilename, ios::app);
    if (!logFile) {
        throw runtime_error("Cannot open log file: " + logFilename);
    }

    // Ensure file is writable
    logFile << "\nLog file opened successfully" << endl;
    if (!logFile.good()) {
        throw runtime_error("Cannot write to log file: " + logFilename);
    }
    logFile.close();

    // Assign logFilename after validation
    this->logFilename = logFilename;
}

Coordinator::~Coordinator() {
    log("Shutting down gracefully");
}

void Coordinator::callParticipants(const string &accountFrom,
                                   const string &accountTo,
                                   double amount,
                                   const vector<pair<string, u_short>> &banks) {
    for (const auto &bank: banks) {
        addParticipant(bank.first, bank.second);
    }
    bool twoPC = sendVoteRequest(amount, accountFrom, accountTo);
    if (twoPC) {
        sendGlobalCommit();
    } else {
        sendGlobalAbort();
    }
}

void Coordinator::addParticipant(const string &host, u_short port) {
    participants.emplace_back(TCPClient(host, port), host, port, INIT);
    log("Connected to participant " + host + ":" + to_string(port));
}

bool Coordinator::sendVoteRequest(double amount, const string &accountFrom,
                                  const string &accountTo) {
    string response; // hold response from participants
    vector<string> messages = {
            string(toString(VOTE_REQUEST)) + " " + accountFrom + " " +
            formatAmount(-amount),
            string(toString(VOTE_REQUEST)) + " " + accountTo + " " +
            formatAmount(amount)};

    using size_type = vector<tuple<TCPClient, string,
    unsigned short, ParticipantState>>::size_type;
    // Send request to participants
    for (size_type i = 0; i < participants.size(); i++) {
        log("Sending message '" + messages[i] + "' to " +
            get<1>(participants[i]) + ":" +
            to_string(get<2>(participants[i])));
        get<0>(participants[i]).send_request(messages[i]);
    }

    // Get response and process it
    for (auto &bank: participants) {
        response = get<0>(bank).get_response();
        if (!processResponse(response)) {
            get<3>(bank) = ABORT; // update state to ABORT
        } else {
            get<3>(bank) = COMMIT; // update state to COMMIT
        }
    }

    // Return true/false based on updated state
    for (auto &bank: participants) {
        if (get<3>(bank) == ABORT) {
            return false;
        }
    }
    return true;
}

void Coordinator::log(const string &message) {
    cout << message << endl;

    // Add log messages to the end of file without overwriting existing
    ofstream logFile(logFilename, ios::app);
    if (!logFile) {
        throw runtime_error("Unable to open log file");
    }
    logFile << message << endl;
    logFile.close();
}

string Coordinator::formatAmount(double amount) {
    ostringstream stream;
    stream << fixed << setprecision(2) << amount;
    return stream.str();
}

bool Coordinator::processResponse(const string &response) {
    Protocol protocol = toProtocol(response);
    switch (protocol) {
        case VOTE_COMMIT:
            return true;
        case VOTE_ABORT:
            return false;
        default:
            log("Invalid response received: " + response);
            return false;
    }
}

void Coordinator::sendGlobalCommit() {
   string response; // hold response from participants
   bool isCommitted = true; // checks for ACK messages

    // Send request
    for (auto &bank: participants) {
        log("Sending message '" + string(toString(GLOBAL_COMMIT)) + "' to " +
            get<1>(bank) + ":" + to_string(get<2>(bank)));
        get<0>(bank).send_request(toString(GLOBAL_COMMIT));
    }

    // Get response
    for (auto &bank: participants) {
        response = get<0>(bank).get_response();
        if (response != toString(ACK)) {
            cerr << "Failed to receive " + string(toString(ACK)) +
                    " from "
                 << get<1>(bank) + ":" +
                    to_string(get<2>(bank))
                 << endl;
           isCommitted = false;
        } else {
            log("'" + response + "' received from " + get<1>(bank) + ":" +
                to_string(get<2>(bank)));
        }
    }

    // Consider committed or aborted
    if(isCommitted) {
        log("Transaction committed");
    } else {
        log("Transaction aborted");
    }
}

void Coordinator::sendGlobalAbort() {
    string response; // hold response from participants

    // Send global-abort to participants except those who already sent abort
    for (auto &bank: participants) {
        if (get<3>(bank) != ABORT) {
            log("Sending message '" + string(toString(GLOBAL_ABORT)) + "' to " +
                get<1>(bank) + ":" + to_string(get<2>(bank)));
            get<0>(bank).send_request(toString(GLOBAL_ABORT));

            // Get response
            response = get<0>(bank).get_response();
            if (response != toString(ACK)) {
                cerr << "Failed to receive " + string(toString(ACK)) +
                        "from "
                     << get<1>(bank) + ":" +
                        to_string(get<2>(bank))
                     << endl;
            } else {
                log("'" + response + "' received from " + get<1>(bank) + ":" +
                    to_string(get<2>(bank)));
            }
        }
    }
    log("Transaction aborted");
}
