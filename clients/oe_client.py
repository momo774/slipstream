"""Bidirectional OE client: replays Trade rows and reacts to what the server sends back."""

import argparse
import itertools
import socket
import struct
import threading
import time

from csv_replay import load_rows, replay, ts_ns
from wire import INBOUND, pack_exec_report, pack_trade, split_frames, to_fixed

NEW_ORDER = 10
SESSION_CONTROL = 4
SESSION_STATES = {0: "OPEN", 1: "HALT", 2: "CLOSE"}
EXEC_ACK = 0
EXEC_FILL = 1


class StopReplay(Exception):
    """Raised from the replay callback to end the replay early (server sent CLOSE)."""


# Runs on a background thread for the whole session: reads everything the server sends and
# reacts to it, while the main thread is busy (mostly sleeping) in the replay.
def receive_loop(sock, send_lock, closing, counts):
    buf = bytearray()  # Bytes received but not yet parsed; a frame can arrive split across recv()s.
    while chunk := sock.recv(4096):  # recv() returns b"" once the server closes, ending the loop.
        buf += chunk
        for msg_type, body in split_frames(buf):  # Every complete frame now in buf, oldest first.
            name, fmt = INBOUND.get(msg_type, (f"Unknown({msg_type})", None))
            fields = struct.unpack(fmt, body) if fmt else body.hex()
            print(name, fields, flush=True)

            if msg_type == NEW_ORDER:
                # NewOrder fields, in wire order: client_order_id, symbol, status, ts_ns, trade_id,
                # side, qty, limit_px.
                order_id, status, qty, limit_px = fields[0], fields[2], fields[6], fields[7]
                counts[status] = counts.get(status, 0) + 1
                if status == b"A":
                    # Act as the exchange: acknowledge, then fill in full at the limit price.
                    now = time.time_ns()
                    try:
                        with send_lock:
                            sock.sendall(pack_exec_report(order_id, now, EXEC_ACK, 0, 0)
                                         + pack_exec_report(order_id, now, EXEC_FILL, qty, limit_px))
                    except OSError:
                        pass  # Our write side is already shut down after the replay finished.
            elif msg_type == SESSION_CONTROL:
                state = SESSION_STATES.get(fields[1], f"Unknown({fields[1]})")
                print(f"session state -> {state}", flush=True)
                if fields[1] == 2:
                    closing.set()  # Operator typed CLOSE on the server: tell the sender to stop.
    closing.set()
    print("server closed the connection", flush=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", default="data/Quotes_and_Trades.csv")
    parser.add_argument("--oe-host", default="127.0.0.1")
    parser.add_argument("--oe-port", type=int, default=14300)
    args = parser.parse_args()

    rows = load_rows(args.csv)
    trade_ids = itertools.count(1)  # The CSV has no trade ids, so number trades 1, 2, 3, ...
    closing = threading.Event()  # Set by the receiver; checked by the sender before every row.
    send_lock = threading.Lock()  # Both threads send on the same socket.
    counts = {}  # NewOrder status (b"A" / b"R") -> how many the server sent.
    with socket.create_connection((args.oe_host, args.oe_port)) as sock:
        # daemon=True: if the main thread exits, this thread doesn't keep the process alive.
        receiver = threading.Thread(target=receive_loop, args=(sock, send_lock, closing, counts), daemon=True)
        receiver.start()

        # Trades keep flowing while the server is halted: HALT stops orders, not the market data
        # the server needs to keep its VWAP current.
        # Called by replay() once per CSV row, at that row's real-time offset from the first row.
        def send_trade(row):
            if closing.is_set():
                raise StopReplay
            if row["Type"] != "T":
                return  # Quotes belong to the MD client.
            with send_lock:
                sock.sendall(pack_trade(
                    row["Symbol"], ts_ns(row), int(row["Qty"]),
                    to_fixed(row["Price"]), b"?", next(trade_ids),
                ))

        # Ends normally when the CSV runs out, or early via StopReplay (CLOSE) or a broken
        # connection (the server shut down because the MD client finished first).
        try:
            replay(rows, send_trade)
            sock.shutdown(socket.SHUT_WR)  # tell the server we're done, keep reading its last replies
        except (StopReplay, BrokenPipeError, ConnectionResetError):
            pass
        receiver.join()  # Wait for the server's remaining replies before printing the summary.

    print(f"orders accepted: {counts.get(b'A', 0)}, rejected: {counts.get(b'R', 0)}", flush=True)


if __name__ == "__main__":
    main()
