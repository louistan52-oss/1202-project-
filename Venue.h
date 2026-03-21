#ifndef VENUE_H
#define VENUE_H

// Venue.h — Unified venue-operation interface.
//
// PURPOSE:
//   QMS.h owns Queue/QMS_Venue (timeslot + booking data).
//   Robot_Transport_System.h owns Venue (shelves + robots).
//   Neither header should need to know about the other.
//
//   This file provides a single include point for any translation
//   unit that needs to work with venues at the user-facing level
//   (booking, entry, exit, persistence, menu).  The implementations
//   live in Venue.cpp.
//
// USAGE (e.g. Users.cpp):
//   #include "Venue.h"          // replaces individual QMS.h / RTS.h includes
//   VenueOps::check_booking(...);
//   VenueOps::QMSMenu(...);
//
// NO CHANGES are required to QMS.h, QMS.cpp, or Robot_Transport_System.h.

#include "QMS.h"          // Queue, QMS_Venue, save/load_curr_timeslots
#include <map>
#include <string>

// ── Bring only what downstream callers need into scope ──────────────────────
// (QMS.h already pulls in <iostream>, <set>, etc.)

namespace VenueOps {

    // ── Booking ──────────────────────────────────────────────────────────────

    // Scans all venues for an existing booking under IC.
    // If found: prints details and prompts the user to cancel (Y/N).
    // Returns true if a booking WAS found (whether or not it was cancelled),
    // false if IC has no booking at all.
    bool check_booking(const std::string& IC,
                       std::map<char, QMS_Venue>& venues);

    // Checks whether IC holds a reservation for exactly gantry_venue +
    // gantry_time.  Prints entry confirmation on success.
    // Returns true if entry is permitted.
    bool enter_venue(const std::string& IC,
                     std::map<char, QMS_Venue>& venues,
                     char gantry_venue,
                     const std::string& gantry_time);

    // Removes IC from whichever queue it currently occupies.
    // Returns true if IC was found and removed, false otherwise.
    bool leave_venue(const std::string& IC,
                     std::map<char, QMS_Venue>& venues);

    // ── Interactive selection helpers ─────────────────────────────────────────

    // Prompts the user to pick A, B, or C.  Loops until valid input.
    char venueSelection();

    // Displays non-full timeslots for 'venue' and prompts the user to pick one.
    // Accepts either the list index ("1") or the raw time string ("0900").
    // 'timeslots' is the global index→time-string map built during load.
    std::string timeSelection(const QMS_Venue& venue,
                              const std::map<int, std::string>& timeslots);

    // ── Persistence ───────────────────────────────────────────────────────────

    // Writes the current in-memory venue/queue state to timeslots.txt.
    // Thin wrapper around save_curr_timeslots() from QMS.h.
    bool save(const std::map<char, QMS_Venue>& venues);

    // Reads timeslots.txt and populates both 'venues' and 'timeslots'.
    // Thin wrapper around load_curr_timeslots() from QMS.h.
    bool load(std::map<char, QMS_Venue>& venues,
              std::map<int, std::string>& timeslots);

    // Remove files at end of program
    int removeFiles();
    // ── Menu entry point ──────────────────────────────────────────────────────

    // Full interactive QMS loop for a logged-in visitor (identified by IC).
    // Returns the last menu choice made (mirrors QMSMenu return value).
    int menu(const std::string& IC,
             std::map<char, QMS_Venue>& venues,
             const std::map<int, std::string>& timeslots);

} // namespace VenueOps

#endif // VENUE_H