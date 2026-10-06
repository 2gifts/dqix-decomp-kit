#!/usr/bin/env python
# Parse a Claude CLI usage-limit reset line and print SECONDS to sleep until reset (+buffer), so the
# automation can auto-resume spawning workers. Prints -1 if unparseable or too far out (weekly limit
# days away) -> caller should clean-stop instead of sleeping for days.
# Machine TZ is America/New_York (EDT -0400) == the reset TZ, so local datetime is correct; zoneinfo
# isn't installed on this Windows python. If the machine TZ ever changes, revisit.
# Formats handled:
#   "... resets 7:30am (America/New_York)"        session/daily (time only)
#   "... resets 1am (America/New_York)"           session/daily
#   "... resets Jul 20, 8pm (America/New_York)"   weekly (date + time)
import sys, re, os
from datetime import datetime, timedelta
line = sys.argv[1] if len(sys.argv) > 1 else ""
BUFFER = 120                                          # resume 2 min AFTER the stated reset (clock skew)
MAXWAIT = int(os.environ.get("RESET_MAXWAIT", 8*3600))  # >8h (weekly limit / full-day exhaustion) -> -1, clean-stop
MON = {'jan':1,'feb':2,'mar':3,'apr':4,'may':5,'jun':6,'jul':7,'aug':8,'sep':9,'oct':10,'nov':11,'dec':12}
m = re.search(r'resets\s+(?:([A-Za-z]{3})\s+(\d{1,2})\s*,\s*)?(\d{1,2})(?::(\d{2}))?\s*(am|pm)', line, re.I)
if not m:
    print(-1); sys.exit()
mon, day, hh, mm, ap = m.groups()
hh = int(hh); mm = int(mm or 0)
if ap.lower() == 'pm' and hh != 12: hh += 12
if ap.lower() == 'am' and hh == 12: hh = 0
now = datetime.now()
try:
    if mon:  # weekly: explicit date
        tgt = now.replace(month=MON.get(mon.lower(), now.month), day=int(day),
                          hour=hh, minute=mm, second=0, microsecond=0)
        if tgt < now - timedelta(hours=1):      # date already well past -> next year (rollover)
            tgt = tgt.replace(year=now.year + 1)
    else:    # session/daily: time only -> next occurrence
        tgt = now.replace(hour=hh, minute=mm, second=0, microsecond=0)
        if tgt <= now:
            tgt += timedelta(days=1)
except ValueError:
    print(-1); sys.exit()
secs = int((tgt - now).total_seconds()) + BUFFER
if secs < BUFFER: secs = BUFFER                 # reset already passed -> resume ~now
print(-1 if secs > MAXWAIT else secs)
