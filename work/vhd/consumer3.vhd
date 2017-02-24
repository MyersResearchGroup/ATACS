--\begin{verbatim}
----------------------
-- consumer3.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity consumer3 is 		
    port(x,y,z : inout channel:=init_channel);
end consumer3;

architecture behavior of consumer3 is 
    signal data : std_logic := '0';
begin
    consumer3 : process
    begin
	receive(x,data);
	receive(y,data);
	receive(z,data);
    end process consumer3;
end behavior;
--\end{verbatim}
