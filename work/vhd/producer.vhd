--\begin{verbatim}
----------------------
-- producer.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity producer is
  port(x, y : inout channel := init_channel(sender => timing(2, 3)));
end producer;

architecture behavior of producer is
  signal data : std_logic := '0';
begin
  producer : process
  begin
    send(x, data);
    send(y, data);
  end process producer;
end behavior;
--\end{verbatim}
