library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity patron is
  port( ack_patron : out std_logic := '0';
            shelf1 : in std_logic;
            shelf0 : in std_logic );
end patron;

architecture hse of patron is
  signal brown_bag : std_logic;
begin
  commnication : process
  begin
    guard_or( shelf1, '1', shelf0, '1' );
    if ( shelf0 = '1' ) then
      assign( brown_bag, '0', 15, 20 );
    else
	  assign( brown_bag, '1', 15, 20 );
    end if;
    assign( ack_patron,'1', 15, 20 );                
    guard_and( shelf1, '0', shelf0, '0' );
    assign( ack_patron,'0', 15, 20 );             
  end process;
end hse;

