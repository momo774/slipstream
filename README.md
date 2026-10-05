# slipstream [Work in progress]

A small C++20 execution server. It listens to a binary market-data feed, keeps a live top-of-book
and a rolling VWAP for one symbol, and decides which trades are worth taking.

Two Python clients replay a CSV of historical quotes and trades into it over TCP.

## Status

- [x] Binary wire protocol: encoder, streaming decoder
- [x] Python replay clients (market data + order entry)
- [x] Blocking TCP transport with sockets
- [x] L1 book + sliding-window VWAP
- [x] Market-data server with symbol filtering
- [ ] Decision engine + order-entry server
- [ ] Latency metrics, benchmark

## Build & test

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Run

```bash
# terminal 1: start the server first
./build/src/server/slipstream --symbol SYNTH1

# terminal 2: replay market data into it
python3 clients/md_client.py
```

The server logs quotes it filters out, and prints the final best bid/ask when the client disconnects.

## Tested on

| | |
|---|---|
| OS | Ubuntu 24.04 (WSL2 on Windows) |
| CPU | Intel Core i5-8350U @ 1.70 GHz, 4 cores / 8 threads |
| Memory | 3 GB allocated to WSL |
| Toolchain | GCC 13.3, CMake 3.28, Python 3.12 |

Any recent GCC or Clang with C++20 support should work.

## License

MIT
