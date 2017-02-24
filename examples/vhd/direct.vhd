--\begin{verbatim}
----------------------
-- direct.vhd
----------------------
library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;
use work.handshake.all;

entity direct is 		
end direct;

architecture structure of direct is 
    component X
	port(xPut : out std_logic;
	     xGet : in  std_logic);
    end component;

    component Y 
	port(yPut : in  std_logic;
	     yGet : out std_logic);
    end component;

    signal zPut : std_logic := 0;
    signal zGet : std_logic := 0;
begin
    myX : X port map(xPut => zPut, xGet => zGet);
    myY : Y port map(yPut => zPut, yGet => zGet);
end structure;
--\end{verbatim}
