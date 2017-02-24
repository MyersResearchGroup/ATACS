library ieee;   --tex comment 
use ieee.std_logic_1164.all;   --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.handshake.all;   --tex comment 

entity shopPA_bundled is   --tex comment 
  port(req_wine:in std_logic;   --tex comment 
       ack_wine:buffer std_logic:='0';   --tex comment 
       bottle:in std_logic_vector(2 downto 0) := "000";  --tex comment 
       req_patron:buffer std_logic:='0';   --tex comment 
       ack_patron:in std_logic;   --tex comment 
       shelf:buffer std_logic_vector(2 downto 0) := "000");  --tex comment 
end shopPA_bundled;   --tex comment 

architecture hse of shopPA_bundled is   --tex comment 
begin   --tex comment 
--\begin{algorithm}
--\small
Shop_PA_lazy_active:process
begin
  guard(req_wine,'1');        -- winery calls
  --@synthesis_off
  shelf <= bottle after delay(2,4);
  wait for delay(5,10);
  --@synthesis_on
  assign(ack_wine,'1',1,3);   -- shop receives wine
  guard(ack_patron,'0');      -- ack_patron resets
  assign(req_patron,'1',1,3); -- call patron
  guard(req_wine,'0');        -- req_wine resets
  assign(ack_wine,'0',1,3);   -- reset ack_wine
  guard(ack_patron,'1');      -- patron buys wine
  assign(req_patron,'0',1,3); -- reset req_patron
end process;
--\end{algorithm}
end hse;   --tex comment 

library ieee;  --tex comment 
use ieee.std_logic_1164.all;  --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.handshake.all;  --tex comment 

entity patron_bundled is  --tex comment 
  port(req_patron:in std_logic;  --tex comment 
       ack_patron:buffer std_logic:='0';  --tex comment 
       shelf:in std_logic_vector(2 downto 0));  --tex comment 
end patron_bundled;  --tex comment 

architecture four_phase of patron_bundled is  --tex comment 
  signal bag:std_logic_vector(2 downto 0);  --tex comment 
begin  --tex comment 
--\begin{algorithm}
--\small
patronP_bundled_4phase:process
begin
  guard(req_patron,'1');       -- shop calls
  --@synthesis_off
  bag <= shelf after delay(2,4);
  wait for delay(5,10);
  --@synthesis_on
  assign(ack_patron,'1',1,3);  -- patron buys wine
  guard(req_patron,'0');       -- req_patron resets
  assign(ack_patron,'0',1,3);  -- reset ack_patron
end process;
--\end{algorithm}
end four_phase;  --tex comment 

library ieee;  --tex comment 
use ieee.std_logic_1164.all;  --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.handshake.all;  --tex comment 

entity winery_bundled is  --tex comment 
  port(req_wine:buffer std_logic := '0';  --tex comment 
       ack_wine:in std_logic;  --tex comment 
       bottle:buffer std_logic_vector(2 downto 0) := "000");  --tex comment 
end winery_bundled;  --tex comment 

architecture four_phase of winery_bundled is  --tex comment 
begin  --tex comment 
--\begin{algorithm}
--\small
winery_bundled_4phase:process
begin
  --@synthesis_off
  bottle <= selection(8,3);
  wait for delay(5,10);
  --@synthesis_on
  assign(req_wine,'1',1,3);   -- call shop
  guard(ack_wine,'1');        -- wine delivered
  assign(req_wine,'0',1,3);   -- reset req_wine
  guard(ack_wine,'0');        -- ack_wine resets
end process;
--\end{algorithm}
end four_phase;  --tex comment 

--
-- File : C:\vbproj\chap3\Default\genhdl\vhdl\wine_bundled.vhd
-- CDB  : C:\vbproj\chap3\default\default.cdb
-- By   : CDB2VHDL Netlister version 16.0.0.8
-- Time : Sat Jan 22 11:01:45 2000

-- Entity/architecture declarations 

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity wine_bundled is
end wine_bundled;

architecture wine_bundled of wine_bundled is
    -- Component declarations
    component shopPA_bundled
        port(
            req_wine : in std_logic;
            bottle : in std_logic_vector(2 downto 0);
            ack_patron : in std_logic;
            req_patron : buffer std_logic;
            ack_wine : buffer std_logic;
            shelf : buffer std_logic_vector(2 downto 0)
        );
    end component;
    component patron_bundled
        port(
            req_patron : in std_logic;
            shelf : in std_logic_vector(2 downto 0);
            ack_patron : buffer std_logic
        );
    end component;
    component winery_bundled
        port(
            ack_wine : in std_logic;
            req_wine : buffer std_logic;
            bottle : buffer std_logic_vector(2 downto 0)
        );
    end component;
    -- Signal declarations
    signal ack_patron : std_logic; --@ in
    signal ack_wine : std_logic;
    signal bottle : std_logic_vector(2 downto 0);
    signal req_patron : std_logic;
    signal req_wine : std_logic; --@ in
    signal shelf : std_logic_vector(2 downto 0);
begin
    -- Signal assignments
    -- Component instances
    shopPA_bundled2 : shopPA_bundled
        port map(
            req_wine => req_wine,
            bottle => bottle,
            ack_patron => ack_patron,
            req_patron => req_patron,
            ack_wine => ack_wine,
            shelf => shelf
        );
    patron_bundled4 : patron_bundled
        port map(
            req_patron => req_patron,
            shelf => shelf,
            ack_patron => ack_patron
        );
    winery_bundled2 : winery_bundled
        port map(
            ack_wine => ack_wine,
            req_wine => req_wine,
            bottle => bottle
        );
end wine_bundled;
