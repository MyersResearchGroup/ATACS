---------------------- 
-- merge.vhd 
---------------------- 
library ieee; 
use ieee.std_logic_1164.all; 
use ieee.std_logic_arith.all; 
use ieee.std_logic_unsigned.all;  
use work.nondeterminism.all; 
use work.channel.all; 

entity merge is 		 
    port(X : inout channel:=init_channel; 
	 Y : inout channel:=init_channel; 
	 Z : inout channel:=init_channel);	  
end merge; 
 
architecture behavior of merge is  
    signal u : std_logic_vector( 2 downto 0 ); 
begin 
--\begin{verbatim}
    merge : process
    begin
	send   (Z,u);
	guard_or(probe(X),'1',probe(Y),'1');
	if (probe(X)) then 
	    receive(X,u);
	else
	    receive(Y,u);
	end if;
	wait for 2 ns;
    end process merge;
--\end{verbatim}
end behavior; 
