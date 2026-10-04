"""Bidirectional OE client: replays Trade rows and prints whatever the server sends back."""

import argparse
import itertools
import socket
import struct
import threading

from csv_replay import load_rows, replay, ts_ns
from wire import INBOUND, pack_trade, split_frames, to_fixed


def receive_loop(sock):
    buf = bytearray()
    while chunk := sock.recv(4096):
        buf += chunk
        for msg_type, body in split_frames(buf):
            name, fmt = INBOUND.get(msg_type, (f"Unknown({msg_type})", None))
            print(name, struct.unpack(fmt, body) if fmt else body.hex(), flush=True)
    print("server closed the connection", flush=True)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", default="data/Quotes_and_Trades.csv")
    parser.add_argument("--oe-host", default="127.0.0.1")
    parser.add_argument("--oe-port", type=int, default=14300)
    args = parser.parse_args()

    rows = load_rows(args.csv)
    trade_ids = itertools.count(1)
    with socket.create_connection((args.oe_host, args.oe_port)) as sock:
        receiver = threading.Thread(target=receive_loop, args=(sock,), daemon=True)
        receiver.start()

        def send_trade(row):
            if row["Type"] != "T":
                return
            sock.sendall(pack_trade(
                row["Symbol"], ts_ns(row), int(row["Qty"]),
                to_fixed(row["Price"]), b"?", next(trade_ids),
            ))

        replay(rows, send_trade)
        sock.shutdown(socket.SHUT_WR)  # tell the server we're done, keep reading its last replies
        receiver.join()


if __name__ == "__main__":
    main()
