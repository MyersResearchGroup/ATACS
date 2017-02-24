---------------------- 
-- merge3.vhd 
---------------------- 
library ieee; 
use ieee.std_logic_1164.all;
use work.handshake.all;
use work.channel.all; 

entity merge3 is
    port(W,X,Y,Z : inout channel:=init_channel);
end merge3; 

architecture behavior of merge3 is  
    signal u : std_logic_vector( 2 downto 0 ); 
begin 
    merge3 : process
    begin
        guard_or(probe(W),'1',probe(X),'1', probe(Y),'1');
        if (probe(X)) then 
            receive(X,u);
        elsif (probe(Y)) then
            receive(Y,u);
	else
	    receive(W,u);
        end if;
        send(Z,u);
        wait for 2 ns;
    end process merge3;
end behavior; 
