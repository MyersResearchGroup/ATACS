--\begin{verbatim}
----------------------
-- X.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity X is 		
    port(xPut : out std_logic:='0';
	 xGet : in  std_logic:='0');
end X;

architecture behavior of X is 
begin
    X : process
    begin
	assign(xPut,'1',1,2);
	guard (xGet,'1');
	assign(xPut,'0',1,2);
	guard (xGet,'0');
    end process X;
end behavior;
--\end{verbatim}
