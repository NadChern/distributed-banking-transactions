/**
 * @file 2PC_Participant.h declaration and definition for Protocol enum
 * @author Nadezhda Chernova
 */

#pragma once
#include <string>

using namespace std;

/**
 * @enum Protocol
 * Defines different protocol messages used in the 2-phase commit protocol.
 * These messages are used to coordinate the actions between participants
 * and coordinator based on the received commands (responses).
 */
enum Protocol {
    VOTE_REQUEST,
    VOTE_COMMIT,
    VOTE_ABORT,
    GLOBAL_COMMIT,
    GLOBAL_ABORT,
    ACK,
    UNKNOWN_PROTOCOL
};

/**
 * Converts string message to its corresponding Protocol enum value
 * @param message protocol message as a string
 * @return Protocol enum value
 */
Protocol toProtocol(const string &message) {
    if (message == "VOTE-REQUEST")
        return VOTE_REQUEST;
    if (message == "VOTE-COMMIT")
        return VOTE_COMMIT;
    if (message == "VOTE-ABORT")
        return VOTE_ABORT;
    if (message == "GLOBAL-COMMIT")
        return GLOBAL_COMMIT;
    if (message == "GLOBAL-ABORT")
        return GLOBAL_ABORT;
    if (message == "ACK")
        return ACK;
    return UNKNOWN_PROTOCOL;
}

/**
 * Converts Protocol enum value to string
 * @param protocol Protocol enum value
 * @return protocol message as a string
 */
const char *toString(Protocol protocol) {
    switch (protocol) {
        case VOTE_REQUEST:
            return "VOTE-REQUEST";
        case VOTE_COMMIT:
            return "VOTE-COMMIT";
        case VOTE_ABORT:
            return "VOTE-ABORT";
        case GLOBAL_COMMIT:
            return "GLOBAL-COMMIT";
        case GLOBAL_ABORT:
            return "GLOBAL-ABORT";
        case ACK:
            return "ACK";
        default:
            return "UNKNOWN-PROTOCOL"; // handle unexpected messages
    }
}


