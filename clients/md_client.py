"""Unidirectional MD client: replays Quote rows from the CSV to the server over MD/1."""

import argparse
import socket

from csv_replay import load_rows, replay, ts_ns
from wire import pack_quote, to_fixed


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--csv", default="data/Quotes_and_Trades.csv")
    parser.add_argument("--md-host", default="127.0.0.1")
    parser.add_argument("--md-port", type=int, default=14200)
    args = parser.parse_args()

    rows = load_rows(args.csv)
    with socket.create_connection((args.md_host, args.md_port)) as sock:

        def send_quote(row):
            if row["Type"] != "Q":
                return
            sock.sendall(pack_quote(
                row["Symbol"], ts_ns(row),
                to_fixed(row["BidPrice"]), int(row["BidQty"]),
                to_fixed(row["AskPrice"]), int(row["AskQty"]),
            ))

        replay(rows, send_quote)


if __name__ == "__main__":
    main()
