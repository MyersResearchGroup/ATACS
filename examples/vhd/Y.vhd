--\begin{verbatim}
----------------------
-- Y.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity Y is 		
    port(yPut : in  std_logic:='0';
	 yGet : out std_logic:='0');
end Y;

architecture behavior of Y is 
begin
    Y : process
    begin
	guard (yPut,'1');
	assign(yGet,'1',1,2);
	guard (yPut,'0');
	assign(yGet,'0',1,2);
    end process Y;
end behavior;
--\end{verbatim}
