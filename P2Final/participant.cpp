/**
 * @file participant.cpp - driver for Participant program
 * @author Nadezhda Chernova
 */


#include <iostream>
#include <sstream>
#include <csignal>
#include <stdexcept>
#include <memory>
#include "2PC_Participant.h"

using namespace std;

unique_ptr<Participant> participant_ptr; // unique pointer to Participant obj

/**
 * Validates and parses command-line arguments.
 * @param argc number of command-line arguments
 * @param argv array of command-line arguments
 * @param serve_port ref to var to store parsed serve port
 * @param accounts_filename ref on accounts filename var
 * @param log_filename ref on log filename var
 * @throws runtime_error if validation fails
 */
void validateArguments(int argc, char *argv[], int &serve_port,
                       string &accounts_filename, string &log_filename);

/**
 * Signal handler for Ctrl-C (SIGINT).
 * It logs the receipt of the signal, rolls back any changes, stops the server
 * and ensures proper cleanup before exiting.
 *
 * @param signum signal number received
 */
void signalHandler(int signum);

/**
 * Handles server errors (crashes) by logging, rolling back any changes
 * to accounts and stopping the server.
 * @param errorMessage error message to log
 */
void handleServerError(const string &errorMessage);

/**
 * Main function initializes Participant program, validates command-line
 * arguments, creates a Participant object, and starts the TCP server to handle
 * transactions. It also registers a signal handler to gracefully handle
 * Ctrl-C (SIGINT) signals and use handleServerError() to handle exceptions
 * and crashes that can occur during initialization or execution.
 *
 * @param argc number of command-line arguments
 * @param argv array of command-line arguments
 * @return EXIT_SUCCESS on successful execution, EXIT_FAILURE on error
 */
int main(int argc, char *argv[]) {
    try {
        // Declare variables for validation
        int serve_port;
        string accounts_filename, log_filename;

        // Validate and parse command-line arguments
        validateArguments(argc, argv, serve_port, accounts_filename,
                          log_filename);

        // Create a Participant object and start the server
        participant_ptr = make_unique<Participant>(serve_port, argv[2],
                                                   argv[3]);

        // Register signal handler for Ctrl-C
        signal(SIGINT, signalHandler);

        ostringstream note;
        note << "\nTransaction service on port " << serve_port
             << " (Ctrl-C to stop)";
        participant_ptr->log(note.str());
        participant_ptr->serve();
    }
    catch (const exception &e) {
        handleServerError("Error. " + string(e.what()));
        return EXIT_FAILURE;
    }
    catch (...) {
        handleServerError("Unknown error occurred");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void signalHandler(int signum) {
    handleServerError("Received Ctrl-C");
    exit(signum);
}

void handleServerError(const string &errorMessage) {
    cerr << errorMessage << endl;
    if (participant_ptr) {
        participant_ptr->log(errorMessage);
        participant_ptr->stop();
    }
}

void validateArguments(int argc, char *argv[], int &serve_port,
                       string &accounts_filename, string &log_filename) {
    // Check if the correct number of arguments is provided
    if (argc < 4)
        throw runtime_error("Usage: participant serve_port "
                            "accounts_filename log_filename");

    accounts_filename = argv[2];
    log_filename = argv[3];

    // Extract and validate serve port
    try {
        serve_port = stoi(argv[1]);
        if (serve_port < 1 || serve_port >= 1 << 16) {
            throw runtime_error("Invalid port: " + string(argv[4]));
        }
    }
    catch (const invalid_argument &) {
        throw runtime_error("Invalid port format: " + string(argv[4]));
    }

    // Check if the log file has .txt extension
    if (log_filename.substr(log_filename.find_last_of('.') + 1) != "txt") {
        throw runtime_error(
                "Log file must have a .txt extension: " + log_filename);
    }

    // Check if the accounts file has .txt extension
    if (accounts_filename.substr(accounts_filename.find_last_of('.') + 1) !=
        "txt") {
        throw runtime_error(
                "Accounts file must have a .txt extension: " +
                accounts_filename);
    }
}