library ieee;  --tex comment 
use ieee.std_logic_1164.all;  --tex comment 
use work.nondeterminism.all;  --tex comment 
use work.handshake.all;  --tex comment 

entity patron_bundled is  --tex comment 
  port(req_patron : in std_logic;  --tex comment 
       ack_patron : inout std_logic := '0';  --tex comment 
       shelf : in std_logic_vector(2 downto 0)  --tex comment 
       );  --tex comment 
end patron_bundled;  --tex comment 

architecture four_phase of patron_bundled is  --tex comment 
  signal brown_bag : std_logic_vector(2 downto 0);  --tex comment 
begin  --tex comment 
--\begin{verbatim}
  patronP_bundled_4phase : process
  begin
    guard(req_patron, '1');  -- shop calls
    --@synthesis_off
    brown_bag <= shelf after delay(1, 2);
    wait for 5 ns;
    --@synthesis_on
    assign(ack_patron, '1', 2, 3);  -- patron buys wine
    guard(req_patron, '0');  -- req_patron resets
    assign(ack_patron, '0', 2, 3);  -- reset ack_patron
  end process;
--\end{verbatim}
end four_phase;  --tex comment 
