library ieee; 
use ieee.std_logic_1164.all;
use work.channel.all; 

entity merge is 		 
    port(X,Y,Z : inout channel:=init_channel); 
end merge; 

architecture behavior of merge is  
    signal u : std_logic_vector( 2 downto 0 ); 
begin 
    merge : process
    begin
	if ( not (probe(X) or probe(Y) ) ) then
	    wait until (probe(X) or probe(Y));
	end if;
	if (probe(X)) then 
	    receive(X,u);
	else
	    receive(Y,u);
	end if;
	send   (Z,u);
	wait for 2 ns;
    end process merge;
end behavior; 
