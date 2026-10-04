"""Packs and unpacks MD/1 and OE/1 frames."""

import struct

QUOTE = 1
TRADE = 2

# Server -> client messages: msg_type -> (name, body format)
INBOUND = {
    3: ("Heartbeat", "<Q"),
    4: ("SessionControl", "<QB"),
    10: ("NewOrder", "<Q12scQqcIq"),
}


def to_fixed(price):
    return round(float(price) * 10_000)


def frame(msg_type, body):
    return struct.pack("<HBB", len(body), msg_type, 1) + body


def pack_quote(symbol, ts_ns, bid_px, bid_qty, ask_px, ask_qty):
    body = struct.pack("<12sQIqIq", symbol.encode(), ts_ns, bid_qty, bid_px, ask_qty, ask_px)
    return frame(QUOTE, body)


def pack_trade(symbol, ts_ns, qty, px, aggressor, trade_id):
    body = struct.pack("<12sQIqcq", symbol.encode(), ts_ns, qty, px, aggressor, trade_id)
    return frame(TRADE, body)


def split_frames(buf):
    """Pops every complete frame off the front of buf (a bytearray); yields (msg_type, body)."""
    while len(buf) >= 4:
        body_len, msg_type, _ = struct.unpack_from("<HBB", buf)
        if len(buf) < 4 + body_len:
            return
        body = bytes(buf[4:4 + body_len])
        del buf[:4 + body_len]
        yield msg_type, body
