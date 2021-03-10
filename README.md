# cameleon-chess

cameleon-chess is the **chess engine** which will be powering up BAUB-Chess in the near future. The goal is to make it a fully functionnal, decent, and UCI compatible engine, optimized enough to run correctly on a Raspberry Pi.
It uses a combination of a 0x88 board representation, speed up by keeping a piece-list to avoid scanning it in its entirety.

## TODO

[x] Board Representation
[x] Evaluation function
[x] Search function
[ ] UCI compatibility
[ ] Deep optimization
[ ] Opening book

## How do I use cameleon ?

The engine features a functionnal CLI, while it patiently waits for UCI implementation.
For now, only simple CPU vs Human play is possible in the console, the only documentation is the move notation, which goes as follows :

<FROM><TO>-<FLAG>
  
With FROM and TO being the starting and landing squares respectively, and FLAG indicating the move type :
QUIET               -> 00
DOUBLE PAWN PUSH    -> 01
KINGSIDE CASTLING   -> 02
QUEENSIDE CASTLING  -> 03
CAPTURE             -> 04
EN PASSANT          -> 05
KNIGHT PROMOTION    -> 08
BISHOP PROMOTION    -> 09
ROOK PROMOTION      -> 10
QUEEN PROMOTION     -> 11
KNIGHT PROMOCAPTURE -> 12
BISHOP PROMOCAPTURE -> 13
ROOK PROMOCAPTURE   -> 14
QUEEN PROMOCAPTURE  -> 15
