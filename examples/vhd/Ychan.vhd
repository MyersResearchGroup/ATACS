--\begin{verbatim}
----------------------
-- Ychan.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity Ychan is
  port(y : inout channel := init_channel(receiver => timing(1, 2)));
end Ychan;

architecture behavior of Ychan is
  signal data : std_logic := '0';
begin
  Ychan : process
  begin
    receive(y, data);
  end process Ychan;
end behavior;
--\end{verbatim}
