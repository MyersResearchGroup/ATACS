--\begin{verbatim}
----------------------
-- chan.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity chan is 		
end chan;

architecture structure of chan is 
    component Xchan
	port(x : inout channel);
    end component;

    component Ychan 
	port(y : inout channel);
    end component;

    signal L : channel := init_channel;
begin
    myX : Xchan port map(x => L);
    myY : Ychan port map(y => L);
end structure;
--\end{verbatim}
