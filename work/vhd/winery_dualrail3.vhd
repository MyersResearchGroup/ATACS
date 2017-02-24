--\begin{verbatim}
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use work.nondeterminism.all;
use work.handshake.all;

entity winery_dualrail3 is
  port(ack_wine2 : in std_logic;
       bottle2_1 : inout std_logic := '0';
       bottle2_0 : inout std_logic := '0';
       ack_wine1 : in std_logic;
       bottle1_1 : inout std_logic := '0';
       bottle1_0 : inout std_logic := '0';
       ack_wine0 : in std_logic;
       bottle0_1 : inout std_logic := '0';
       bottle0_0 : inout std_logic := '0');
end winery_dualrail3;

architecture hse of winery_dualrail3 is
begin
winery_dual_rail:process
    variable z : integer;
begin
     z:=selection(8);
     case z is
     when 1 =>
       assign(bottle2_0,'1',2,5,bottle1_0,'1',2,5,
              bottle0_0,'1',2,5);
     when 2 =>
       assign(bottle2_0,'1',2,5,bottle1_0,'1',2,5,
              bottle0_1,'1',2,5);
     when 3 =>
       assign(bottle2_0,'1',2,5,bottle1_1,'1',2,5,
              bottle0_0,'1',2,5);
     when 4 =>
       assign(bottle2_0,'1',2,5,bottle1_1,'1',2,5,
              bottle0_1,'1',2,5);
     when 5 =>
       assign(bottle2_1,'1',2,5,bottle1_0,'1',2,5,
              bottle0_0,'1',2,5);
     when 6 =>
       assign(bottle2_1,'1',2,5,bottle1_0,'1',2,5,
              bottle0_1,'1',2,5);
     when 7 =>
       assign(bottle2_1,'1',2,5,bottle1_1,'1',2,5,
              bottle0_0,'1',2,5);
     when others =>
       assign(bottle2_1,'1',2,5,bottle1_1,'1',2,5,
              bottle0_1,'1',2,5);
     end case;
     guard_and(ack_wine2,'1',ack_wine1,'1',ack_wine0,'1'); 
     vassign(bottle2_0,'0',2,5,bottle1_0,'0',2,5,
             bottle0_0,'0',2,5,bottle2_1,'0',2,5,
             bottle1_1,'0',2,5,bottle0_1,'0',2,5);
     guard_and(ack_wine2,'0',ack_wine1,'0',ack_wine0,'0'); 
end process;
end hse;
--\end{verbatim}

