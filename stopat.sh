#!/bin/bash
# Hard deadline for a worker run: sleeps until $1 (date -d format) and then stops everything.
# The operator's authorisation window is a promise the fleet cannot keep by being remembered.
KIT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && { pwd -W 2>/dev/null || pwd; })"
SP="$(python "$KIT/kitpaths.py" state)"
DEADLINE="${1:?usage: stopat.sh '2026-09-07 19:30:00'}"
TARGET=$(date -d "$DEADLINE" +%s) || exit 2
echo "stopat: will stop the fleet at $DEADLINE ($TARGET)"
while [ "$(date +%s)" -lt "$TARGET" ]; do
  sleep 60
done
touch "$SP/FLEET_STOPPED" "$SP/STOP_PULL" "$SP/USAGE_LIMIT_STOP"
bash "$KIT/fullstop.sh" --hard
echo "stopat: deadline $DEADLINE reached -- fleet stopped"
