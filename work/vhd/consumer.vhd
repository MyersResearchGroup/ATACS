--\begin{verbatim}
----------------------
-- consumer.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity consumer is
  port(x, y : inout channel := init_channel(receiver => timing(1, 2)));
end consumer;

architecture behavior of consumer is
  signal data : std_logic := '0';
begin
  consumer : process
  begin
    receive(x, data);
    receive(y, data);
  end process consumer;
end behavior;
--\end{verbatim}
