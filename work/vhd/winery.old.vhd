library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity winery is
  port( ack_wine : in std_logic;
         bottle1 : inout std_logic := '0';
         bottle0 : inout std_logic := '0');
end winery;

architecture hse of winery is
begin 
  communication:process
    variable rv : integer := 0;
  begin
	rv := selection( 2 );
    case rv is
	  when 1 => assign( bottle1, '1', 5, 15 );
	  when others => assign( bottle0, '1', 5, 15 );
	end case;
    guard( ack_wine, '1' );
	if ( bottle1 = '1' ) then
	  assign( bottle1, '0', 5, 10 );
	else 
	  assign( bottle0, '0', 5, 10 );
	end if;
    guard( ack_wine, '0' );
  end process;  
end hse;


