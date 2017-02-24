--\begin{algorithm}
--\small
library ieee; --tex comment
use ieee.std_logic_1164.all; --tex comment
use work.nondeterminism.all; --tex comment
use work.handshake.all; --tex comment

entity winery_dualrail is --tex comment
  port(ack_wine:in std_logic; --tex comment
       bottle1:buffer std_logic := '0'; --tex comment
       bottle0:buffer std_logic := '0'); --tex comment
end winery_dualrail; --tex comment

architecture hse of winery_dualrail is --tex comment
begin --tex comment
winery_dual_rail:process
  variable z:integer;
begin
  z:=selection(2);
  case z is
    when 1 =>
      assign(bottle1,'1',1,3);
    when others =>
      assign(bottle0,'1',1,3);
  end case;
  guard(ack_wine,'1'); 
  vassign(bottle1,'0',1,3,bottle0,'0',1,3);
  guard(ack_wine,'0'); 
end process;
end hse; --tex comment
--\end{algorithm}

library ieee; --tex comment 
use ieee.std_logic_1164.all; --tex comment 
use work.nondeterminism.all; --tex comment 
use work.handshake.all; --tex comment 

entity shop_dualrail is --tex comment 
  port(bottle1 : in std_logic; --tex comment 
       bottle0 : in std_logic; --tex comment 
       ack_wine : buffer std_logic:='0'; --tex comment 
       ack_patron : in std_logic; --tex comment 
       shelf1 : buffer std_logic:='0'; --tex comment 
       shelf0 : buffer std_logic:='0'); --tex comment 
end shop_dualrail; --tex comment 

architecture hse of shop_dualrail is --tex comment 
begin --tex comment 
--\begin{algorithm}
--\small
shopPA_dual_rail:process
begin
  guard(ack_patron,'0');
  guard_or(bottle0,'1',bottle1,'1');
  if bottle0 = '1' then assign(shelf0,'1',1,3);
  elsif bottle1 = '1' then assign(shelf1,'1',1,3);
  end if;
  assign(ack_wine,'1',1,3); 
  guard(ack_patron,'1');
  vassign(shelf0,'0',1,3,shelf1,'0',1,3);
  guard_and(bottle0,'0',bottle1,'0');
  assign(ack_wine,'0',1,3);
end process;
--\end{algorithm}
end hse; --tex comment 

library ieee; --tex comment 
use ieee.std_logic_1164.all; --tex comment 
use work.nondeterminism.all; --tex comment 
use work.handshake.all; --tex comment 

entity patron_dualrail is --tex comment 
  port(ack_patron : buffer std_logic:='0'; --tex comment 
       shelf1 : in std_logic; --tex comment 
       shelf0 : in std_logic); --tex comment 
end patron_dualrail; --tex comment 

architecture hse of patron_dualrail is --tex comment 
begin --tex comment 
--\begin{algorithm}
--\small
patronP_dualrail:process
begin
  guard_or(shelf1,'1',shelf0,'1');
  assign(ack_patron,'1',1,3);
  guard_and(shelf1,'0',shelf0,'0');
  assign(ack_patron,'0',1,3);
end process;
--\end{algorithm}
end hse; --tex comment 

--
-- File : C:\vbproj\chap3\Default\genhdl\vhdl\wine_dualrail.vhd
-- CDB  : C:\vbproj\chap3\default\default.cdb
-- By   : CDB2VHDL Netlister version 16.0.0.8
-- Time : Fri Jan 21 23:24:15 2000

-- Entity/architecture declarations 

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity wine_dualrail is
end wine_dualrail;

architecture wine_dualrail of wine_dualrail is
    -- Component declarations
    component winery_dualrail
        port(
            ack_wine : in std_logic;
            bottle1 : buffer std_logic;
            bottle0 : buffer std_logic
        );
    end component;
    component shop_dualrail
        port(
            bottle1 : in std_logic;
            bottle0 : in std_logic;
            ack_patron : in std_logic;
            ack_wine : buffer std_logic;
            shelf1 : buffer std_logic;
            shelf0 : buffer std_logic
        );
    end component;
    component patron_dualrail
        port(
            shelf1 : in std_logic;
            shelf0 : in std_logic;
            ack_patron : buffer std_logic
        );
    end component;
    -- Signal declarations
    signal ack_patron : std_logic; --@ in
    signal ack_wine : std_logic;
    signal bottle0 : std_logic; --@ in
    signal bottle1 : std_logic; --@ in
    signal shelf0 : std_logic;
    signal shelf1 : std_logic;
begin
    -- Signal assignments
    -- Component instances
    winery_dualrail1 : winery_dualrail
        port map(
            ack_wine => ack_wine,
            bottle1 => bottle1,
            bottle0 => bottle0
        );
    shop_dualrail1 : shop_dualrail
        port map(
            bottle1 => bottle1,
            bottle0 => bottle0,
            ack_patron => ack_patron,
            ack_wine => ack_wine,
            shelf1 => shelf1,
            shelf0 => shelf0
        );
    patron_dualrail2 : patron_dualrail
        port map(
            shelf1 => shelf1,
            shelf0 => shelf0,
            ack_patron => ack_patron
        );
end wine_dualrail;
