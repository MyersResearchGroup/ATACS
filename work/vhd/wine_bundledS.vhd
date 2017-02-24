library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;

entity wine_bundled is
  port(ack_patron : inout std_logic;
         ack_wine : inout std_logic;
       req_patron : inout std_logic;
         req_wine : inout std_logic
       );
end wine_bundled;


architecture structure of wine_bundled is

  -- internal signals declarations
  signal ack_patron: std_logic;
  signal ack_wine: std_logic;
  signal req_patron: std_logic;
  signal req_wine: std_logic;


  -- output complements
  signal ack_patron_bar: std_logic;
  signal ack_wine_bar: std_logic;
  signal req_patron_bar: std_logic;
  signal req_wine_bar: std_logic;



  component a1
  port(a1 : in std_logic;
       o : inout std_logic);
  end component;

  component n2p2
  port(a1n : in std_logic;
       b1n : in std_logic;
       a1p : in std_logic;
       b1p : in std_logic;
       o : inout std_logic;
       obar : inout std_logic);
  end component;

begin
  i_0 : a1
    port map(a1=>ack_wine_bar, 
             o=>req_wine);

  i_1 : a1
    port map(a1=>req_patron, 
             o=>ack_patron);

  i_2 : n2p2
    port map(a1n=>ack_patron_bar, b1n=>ack_wine, 
             a1p=>ack_patron_bar, b1p=>ack_wine, 
             o=>req_patron, obar=>req_patron_bar);

  i_3 : n2p2
    port map(a1n=>req_wine, b1n=>req_patron_bar, 
             a1p=>req_wine, b1p=>req_patron_bar, 
             o=>ack_wine, obar=>ack_wine_bar);

end structure;

