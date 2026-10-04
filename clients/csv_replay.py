import csv
import time


def load_rows(csv_path):
    with open(csv_path, newline="") as f:
        return list(csv.DictReader(line for line in f if not line.startswith("#")))


def ts_ns(row):
    hms, ms = row["Timestamp"].split(".")
    h, m, s = (int(x) for x in hms.split(":"))
    return ((h * 3600 + m * 60 + s) * 1000 + int(ms)) * 1_000_000


def replay(rows, on_row):
    """Calls on_row(row) at each row's time relative to the first row in the file."""
    t0 = ts_ns(rows[0])
    start = time.monotonic_ns()
    for row in rows:
        delay = (ts_ns(row) - t0) - (time.monotonic_ns() - start)
        if delay > 0:
            time.sleep(delay / 1e9)
        on_row(row)
