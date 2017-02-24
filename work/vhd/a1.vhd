library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;

entity a1 is
  port(a1 : in std_logic;
       o : inout std_logic :='0');
end a1;


architecture behavior of a1 is 
begin
  o <= (a1) after delay(2, 3);


end behavior;

