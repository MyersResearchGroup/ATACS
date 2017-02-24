--\begin{verbatim}
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity winery_dualrail is
  port(ack_wine : in std_logic;
       bottle1 : inout std_logic := '0';
       bottle0 : inout std_logic := '0');
end winery_dualrail;

architecture hse of winery_dualrail is
begin
winery_dual_rail:process
    variable z : integer;
begin
     z:=selection(2);
     if (z=1) then
       assign(bottle1,'1',2,3);
     else
       assign(bottle0,'1',2,3);
     end if;
--     case z is
--	 when 1 =>
--	     assign(bottle1,'1',2,5);
--	 when others =>
--             assign(bottle0,'1',2,5);
--     end case;
     guard(ack_wine,'1');                    -- wine delivered
     vassign(bottle1,'0',2,3,bottle0,'0',2,3);
     guard(ack_wine,'0');                    -- ack_wine resets
end process;
end hse;
--\end{verbatim}

