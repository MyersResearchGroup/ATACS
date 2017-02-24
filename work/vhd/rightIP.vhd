--\begin{verbatim}
----------------------
-- right.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity right is 		
    port(y : inout  channel:=init_channel);
end right;

architecture behavior of right is 
    signal data : std_logic := '0';
begin
    right : process
    begin
	if(probe(y)) then
	    receive(y,data);
	end if;
    end process right;
end behavior;
--\end{verbatim}
