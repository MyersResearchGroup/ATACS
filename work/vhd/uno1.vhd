-- Area: 44   Typ Delay: 5   Min Delay: 4   Max Delay: 6

-- dfg uno {
--   input a 	32
--   input b 	32
--   output zz  32
--   node *	multA	32
--   datain a -> multA
--   datain b -> multA
--   dataout multA -> zz
-- }
--
--  allocation uno {  /* ASYNC */
--   multA	     1   *
--  }


library IEEE;
use IEEE.std_logic_1164.all;
entity uno is
port(a: in std_logic_vector(31 downto 0);
     b: in std_logic_vector(31 downto 0);
     zz: out std_logic_vector(31 downto 0);
     uno_req: in std_logic;
uno_ack: out std_logic);
end uno;

architecture structural of uno is
component Mult
  port(a  : in std_logic_vector(31 downto 0);
       b  : in std_logic_vector(31 downto 0);
       res: out std_logic_vector(31 downto 0);
       req: in  std_logic;
       ack: out std_logic);
end component;

component latch
  port(d  : in std_logic_vector(31 downto 0);
       q  : out std_logic_vector(31 downto 0);
       req: in std_logic;
       ack: out std_logic);
end component;

component uno_ctrl
  port(signal Mult_1_req 	: inout std_logic;
       signal Mult_1_ack 	: in  std_logic;
       signal zz_req 	: out std_logic;
       signal zz_ack 	: in  std_logic;
       signal a_req 	: out std_logic;
       signal a_ack 	: in  std_logic;
       signal b_req 	: out std_logic;
       signal b_ack 	: in  std_logic;
       signal uno_req 	: in std_logic;
       signal uno_ack 	: out std_logic);
end component;

signal Mult_1_res 	: std_logic_vector(31 downto 0);
signal Mult_1_req, Mult_1_ack 	: std_logic;
signal zz_req, zz_ack 	: std_logic;
signal a_req, a_ack 	: std_logic;
signal b_req, b_ack 	: std_logic;
signal a_isig  	: std_logic_vector(31 downto 0);
signal b_isig  	: std_logic_vector(31 downto 0);

begin

l_zz: latch port map(Mult_1_res,zz,zz_req,zz_ack);

l_a: latch port map(a,a_isig,a_req,a_ack);
l_b: latch port map(b,b_isig,b_req,b_ack);

Mult_1: Mult port map(a_isig,b_isig,Mult_1_res,Mult_1_req,Mult_1_ack);

CTRL: uno_ctrl port map(Mult_1_req,Mult_1_ack,
                        zz_req,zz_ack,
                        a_req,a_ack,
                        b_req,b_ack,
                        uno_req,uno_ack);

end structural;


configuration cfg_uno of uno is
  for structural

    -- Latches for data edges

    -- Latches for outputs
    for l_zz: latch use entity WORK.latch(behavioral); end for;

    -- Latches for inputs
    for l_a: latch use entity WORK.latch(behavioral); end for;
    for l_b: latch use entity WORK.latch(behavioral); end for;

    -- System Control
    for CTRL: uno_ctrl use entity WORK.uno_ctrl(behavioral); end for;

    -- Resources used
    for Mult_1: Mult use entity WORK.Mult(behavioral); end for;

    -- Muxes used
  end for;
end cfg_uno;
