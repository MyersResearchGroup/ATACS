--\begin{verbatim}
----------------------
-- conveyor.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity conveyor is
end conveyor;

architecture structure of conveyor is
  component producer
    port(x : inout channel;
         y : inout channel);
  end component;

  component consumer
    port(x : inout channel;
         y : inout channel);
  end component;

  signal x : channel := init_channel;
  signal y : channel := init_channel;
begin
  source : producer port map(x => x, y => y);
  sink : consumer port map(x => x, y => y);
end structure;
--\end{verbatim}
