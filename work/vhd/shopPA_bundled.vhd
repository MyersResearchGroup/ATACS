library ieee;  --tex comment 
use ieee.std_logic_1164.all;  --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.handshake.all;  --tex comment 

entity shopPA_bundled is  --tex comment 
  port(req_wine : in std_logic;  --tex comment 
       ack_wine : inout std_logic := '0';  --tex comment 
       bottle : in std_logic_vector(2 downto 0);  --tex comment 
       req_patron : inout std_logic := '0';  --tex comment 
       ack_patron : in std_logic;  --tex comment 
       shelf : inout std_logic_vector(2 downto 0)
       );  --tex comment 
end shopPA_bundled;  --tex comment 

architecture four_phase of shopPA_bundled is  --tex comment
  signal x:std_logic:='1';
begin  --tex comment 
--\begin{verbatim}
  shop_bundled_4phase : process
  begin
    guard(req_wine, '1');  -- winery calls
    --@synthesis_off
    shelf <= bottle after delay(1, 2);
    wait for 5 ns;
    --@synthesis_on
    assign(ack_wine, '1', 1, 2);
    assign(x,'0',1,2);
    guard(ack_patron, '0');  -- ack_patron resets
    assign(req_patron, '1', 1, 2);  -- call patron
    guard(req_wine, '0');  -- req_wine resets
    assign(ack_wine, '0', 1, 2);  -- reset ack_wine
    assign(x,'1',1,2);
    guard(ack_patron, '1');  -- patron buys wine
    assign(req_patron, '0', 1, 2);  -- reset req_patron
  end process;
--\end{verbatim}
end four_phase;  --tex comment 
