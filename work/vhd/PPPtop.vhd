--\begin{verbatim}
----------------------
-- PPPtop.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all; 
use work.nondeterminism.all;
use work.channel.all;

entity PPPtop is 		
end PPPtop;

architecture structure of PPPtop is 
    component winery
	port(wine_shipping : inout channel);
    end component;
    
    component consumer3 
	port(x,y,z : inout channel);
    end component;

    signal L,M,R : channel := init_channel;
begin
    LEFT  : winery    port map(wine_shipping => L);
    MIDDLE: winery    port map(wine_shipping => M);
    RIGHT : winery    port map(wine_shipping => R);
    PPP   : consumer3 port map(x => L, y => M, z => R);
end structure;
--\end{verbatim}
