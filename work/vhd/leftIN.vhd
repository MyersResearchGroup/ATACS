--\begin{verbatim}
----------------------
-- left.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity left is 		
    port(x : inout channel:=init_channel);
end left;

architecture behavior of left is 
    signal data : std_logic := '0';
begin
    left : process
    begin
	--if(probe(x)) then
	    send(x,data);
	--end if;
    end process left;
end behavior;
--\end{verbatim}
