#!/usr/bin/env bash
# Starts slipstream and both replay clients in the order the server accepts them.
# Any extra arguments are passed to slipstream and override the defaults below,
# e.g. ./scripts/run.sh --symbol SYNTH4 --vwap-window-ms 5000
set -euo pipefail
cd "$(dirname "$0")/.."

SERVER=./build/src/server/slipstream
MD_PORT=14200
OE_PORT=14300

# Checks for a listening socket without connecting to it: a test connection would be accepted
# by slipstream as if it were the real client.
wait_for_port() {
    until ss -ltn "sport = :$1" | grep -q LISTEN; do sleep 0.1; done
}

trap 'kill $(jobs -p) 2>/dev/null' EXIT

# Clients start in the background once each port is listening. The OE port only starts
# listening after the MD client has connected. Client output: build/md_client.log, build/oe_client.log.
(
    wait_for_port "$MD_PORT"
    python3 clients/md_client.py --md-port "$MD_PORT" > build/md_client.log 2>&1 &
    wait_for_port "$OE_PORT"
    python3 clients/oe_client.py --oe-port "$OE_PORT" > build/oe_client.log 2>&1 &
    wait
) &

# The server runs in the foreground so HALT / OPEN / CLOSE can be typed into it.
"$SERVER" --symbol SYNTH1 --max-quantity 500 --participation-cap 0.15 \
    --vwap-window-ms 30000 --band-bps 25.5 \
    --md-port "$MD_PORT" --oe-port "$OE_PORT" "$@"
