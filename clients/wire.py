"""Packs and unpacks MD/1 and OE/1 frames."""

import struct

QUOTE = 1
TRADE = 2
EXEC_REPORT = 11

# Server -> client messages: msg_type -> (name, body format)
# struct format letters: "<" = little-endian with no padding, Q/q = 8-byte unsigned/signed,
# I = 4-byte unsigned, H = 2-byte unsigned, B = 1-byte unsigned, c = one char, 12s = 12 bytes.
# Each format matches the C++ packed struct byte for byte (struct.calcsize(fmt) == sizeof).
INBOUND = {
    3: ("Heartbeat", "<Q"),
    4: ("SessionControl", "<QB"),
    10: ("NewOrder", "<Q12scQqcIq"),
}


# Decimal price -> fixed-point x10,000. round(), not int(): 87.37 * 10000 is 873699.999...
def to_fixed(price):
    return round(float(price) * 10_000)


# 4-byte header (body_len, msg_type, version=1) followed by the body.
def frame(msg_type, body):
    return struct.pack("<HBB", len(body), msg_type, 1) + body


def pack_quote(symbol, ts_ns, bid_px, bid_qty, ask_px, ask_qty):
    body = struct.pack("<12sQIqIq", symbol.encode(), ts_ns, bid_qty, bid_px, ask_qty, ask_px)
    return frame(QUOTE, body)


def pack_trade(symbol, ts_ns, qty, px, aggressor, trade_id):
    body = struct.pack("<12sQIqcq", symbol.encode(), ts_ns, qty, px, aggressor, trade_id)
    return frame(TRADE, body)


# status: 0=ACK, 1=FILL, 2=PARTIAL, 3=REJECT. filled_qty is cumulative for the order.
def pack_exec_report(client_order_id, ts_ns, status, filled_qty, avg_px, reason_code=0):
    body = struct.pack("<QQBIqB", client_order_id, ts_ns, status, filled_qty, avg_px, reason_code)
    return frame(EXEC_REPORT, body)


def split_frames(buf):
    """Pops every complete frame off the front of buf (a bytearray); yields (msg_type, body)."""
    while len(buf) >= 4:
        body_len, msg_type, _ = struct.unpack_from("<HBB", buf)
        if len(buf) < 4 + body_len:
            return
        body = bytes(buf[4:4 + body_len])
        del buf[:4 + body_len]
        yield msg_type, body
