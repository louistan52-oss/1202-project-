#include "Venue.h"

// Venue.cpp — Implementations for VenueOps namespace.
//
// Every function here is a thin, well-named wrapper around the existing
// free functions in QMS.cpp.  The originals are left completely untouched;
// we just forward through to them.  This means:
//
//   • No ODR (one-definition-rule) violations — the old free functions
//     still own their single definition in QMS.cpp.
//   • If you later want to inline the logic here and remove the free
//     functions from QMS.cpp, you only need to edit this one file.

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// ── Forward-declare the free functions that live in QMS.cpp ─────────────────
// (They are not prototyped in QMS.h except for save/load/leave/QMSMenu,
//  so we declare the missing ones here to keep the linker happy.)

// Already in QMS.h:  save_curr_timeslots, load_curr_timeslots,
//                    leave_venue, QMSMenu
// Declared only in QMS.cpp (internal linkage helpers we need to expose):
bool   check_booking  (string IC, map<char, QMS_Venue>& venues);
bool   enter_venue    (string IC, map<char, QMS_Venue>& venues,
                       char gantry_venue, string gantry_time);
char   venueSelection ();
string timeSelection  (const QMS_Venue& venue,
                       const map<int, string>& timeslots);


namespace VenueOps {

// ── Booking ──────────────────────────────────────────────────────────────────

bool check_booking(const std::string& IC,
                   std::map<char, QMS_Venue>& venues)
{
    // Delegates to the free function in QMS.cpp.
    // The free function handles the cancel prompt internally.
    return ::check_booking(IC, venues);
}

bool enter_venue(const std::string& IC,
                 std::map<char, QMS_Venue>& venues,
                 char gantry_venue,
                 const std::string& gantry_time)
{
    return ::enter_venue(IC, venues, gantry_venue, gantry_time);
}

bool leave_venue(const std::string& IC,
                 std::map<char, QMS_Venue>& venues)
{
    // leave_venue is already prototyped in QMS.h, so :: resolves cleanly.
    return ::leave_venue(IC, venues);
}

// ── Interactive selection helpers ─────────────────────────────────────────────

char venueSelection()
{
    return ::venueSelection();
}

std::string timeSelection(const QMS_Venue& venue,
                          const std::map<int, std::string>& timeslots)
{
    return ::timeSelection(venue, timeslots);
}

// ── Persistence ───────────────────────────────────────────────────────────────

bool save(const std::map<char, QMS_Venue>& venues)
{
    return save_curr_timeslots(venues);
}

bool load(std::map<char, QMS_Venue>& venues,
          std::map<int, std::string>& timeslots)
{
    return load_curr_timeslots(venues, timeslots);
}

// ── Remove Shelve related files ───────────────────────────────────────────────
int removeFiles(){
    bool status=1;
    const string filenames[6] = {"vA_shelves.txt","vB_shelves.txt","vC_shelves.txt","vA_stock.txt","vB_stock.txt","vC_stock.txt"};
    for (int i=0; i<6 ;i++){
        if (remove(filenames[i].c_str()) != 0) return 1;
        else continue;
    }
    return 0;
}

// ── Menu entry point ──────────────────────────────────────────────────────────

int menu(const std::string& IC,
         std::map<char, QMS_Venue>& venues,
         const std::map<int, std::string>& timeslots)
{
    // QMSMenu is prototyped in QMS.h; :: makes the call site unambiguous.
    return ::QMSMenu(IC, venues, timeslots);
}

} // namespace VenueOps
