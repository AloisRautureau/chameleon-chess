# cameleon-chess

cameleon-chess is the **chess engine** which will be powering up BAUB-Chess in the near future. The goal is to make it a fully functionnal, decent, and UCI compatible engine, optimized enough to run correctly on a Raspberry Pi.<br/>
It uses a combination of a 0x88 board representation, speed up by keeping a piece-list to avoid scanning it in its entirety.

## TODO

[x] Board Representation<br/>
[x] Evaluation function<br/>
[x] Search function<br/>
[ ] UCI compatibility<br/>
[ ] Deep optimization<br/>
[ ] Opening book<br/>

## How do I use cameleon ?

The engine features a functionnal CLI, while it patiently waits for UCI implementation.<br/>
For now, only simple CPU vs Human play is possible in the console, the only documentation is the move notation, which goes as follows :<br/>
<br/>
FROMTO-FLAG<br/>
  <br/>
With FROM and TO being the starting and landing squares respectively, and FLAG indicating the move type :<br/>
QUIET               -> 00<br/>
DOUBLE PAWN PUSH    -> 01<br/>
KINGSIDE CASTLING   -> 02<br/>
QUEENSIDE CASTLING  -> 03<br/>
CAPTURE             -> 04<br/>
EN PASSANT          -> 05<br/>
KNIGHT PROMOTION    -> 08<br/>
BISHOP PROMOTION    -> 09<br/>
ROOK PROMOTION      -> 10<br/>
QUEEN PROMOTION     -> 11<br/>
KNIGHT PROMOCAPTURE -> 12<br/>
BISHOP PROMOCAPTURE -> 13<br/>
ROOK PROMOCAPTURE   -> 14<br/>
QUEEN PROMOCAPTURE  -> 15<br/>
