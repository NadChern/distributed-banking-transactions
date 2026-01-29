//
// Created by Nadezhda Chernova on 5/22/24.
//

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "2PC_Coordinator.h"

using namespace std;

void validateArguments(int argc, char *argv[], string &logFilename,
                       double &amount, string &hostFrom, int &portFrom,
                       string &accountFrom, string &hostTo, int &portTo,
                       string &accountTo);

int main(int argc, char *argv[])
{
  try
  {
    string logFilename, hostFrom, accountFrom, hostTo, accountTo;
    double amount;
    int portFrom, portTo;

    // Validate and parse command line arguments
    validateArguments(argc, argv, logFilename, amount, hostFrom, portFrom,
                      accountFrom, hostTo, portTo, accountTo);

    // Create vector of participants
    vector<pair<string, u_short>> participants = {
        {hostFrom, static_cast<u_short>(portFrom)},
        {hostTo, static_cast<u_short>(portTo)}};

    // Initialize coordinator
    Coordinator coordinator(logFilename);

    // Log transaction details
    ostringstream note;
    note << "Transaction: $" << amount << "\n\tFrom: " << hostFrom << ":"
         << portFrom << " account #" << accountFrom
         << "\n\tTo:   " << hostTo << ":" << portTo << " account #"
         << accountTo;
    coordinator.log(note.str());

    // Call participants
    coordinator.callParticipants(accountFrom, accountTo, amount,
                                 participants);
  }
  catch (const exception &e)
  {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }
  catch (...)
  {
    cerr << "Unknown error." << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void validateArguments(int argc, char *argv[],
                       string &logFilename, double &amount,
                       string &hostFrom, int &portFrom,
                       string &accountFrom, string &hostTo,
                       int &portTo, string &accountTo)
{
  if (argc < 9)
  {
    throw runtime_error(
        "Usage: coordinator log_filename amount hostFrom portFrom "
        "accountFrom hostTo portTo accountTo");
  }

  logFilename = argv[1];
  hostFrom = argv[3];
  accountFrom = argv[5];
  hostTo = argv[6];
  accountTo = argv[8];

  // Check if the log file has .txt extension
  if (logFilename.substr(logFilename.find_last_of('.') + 1) != "txt")
  {
    throw runtime_error(
        "Log file must have a .txt extension: " + logFilename);
  }

  // Check for valid amount, ensure it's not zero or negative
  try
  {
    amount = stod(argv[2]);
    if (amount <= 0)
    {
      throw runtime_error("Amount must be greater than zero:" +
                          string(argv[2]));
    }
  }
  catch (const invalid_argument &)
  {
    throw runtime_error("Invalid amount format: " + string(argv[2]));
  }

  // Check for valid ports
  try
  {
    portFrom = stoi(argv[4]);
    if (portFrom < 1 || portFrom >= (1 << 16))
    {
      throw runtime_error("Invalid port: " + string(argv[4]));
    }
  }
  catch (const invalid_argument &)
  {
    throw runtime_error("Invalid port format: " + string(argv[4]));
  }

  try
  {
    portTo = stoi(argv[7]);
    if (portTo < 1 || portTo >= (1 << 16))
    {
      throw runtime_error("Invalid port: " + string(argv[7]));
    }
  }
  catch (const invalid_argument &)
  {
    throw runtime_error("Invalid port format: " + string(argv[7]));
  }
}