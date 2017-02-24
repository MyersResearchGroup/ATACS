library ieee;
use ieee.std_logic_1164.all;
use work.nondeterminism.all;

entity mpegdecoder_ctrl_HSE_BEST is
  port( calculate_please_ack : inout std_logic;
        calculate_please_req : in std_logic;
                      cb_ack : in std_logic;
                      cb_req : inout std_logic;
                      cr_ack : in std_logic;
                      cr_req : inout std_logic;
                  dpdata_ack : in std_logic;
                  dpdata_req : inout std_logic;
        increment_please_ack : inout std_logic;
        increment_please_req : in std_logic;
                     lum_ack : in std_logic;
                     lum_req : inout std_logic;
       mem_access_please_ack : inout std_logic;
       mem_access_please_req : in std_logic;
               mem_rw_please : inout std_logic;
           splice_please_ack : inout std_logic;
           splice_please_req : in std_logic
       );
end mpegdecoder_ctrl_HSE_BEST;


architecture structure of mpegdecoder_ctrl_HSE_BEST is

  -- input complements
  signal dpdata_ack_bar: std_logic;
  signal increment_please_ack_bar: std_logic;
  signal mem_access_please_ack_bar: std_logic;
  signal mem_access_please_req_bar: std_logic;
  signal mem_rw_please_bar: std_logic;
  signal splice_please_req_bar: std_logic;



  component a1
  port(a1 : in std_logic;
       o : inout std_logic);
  end component;

  component a2
  port(a1 : in std_logic;
       b1 : in std_logic;
       o : inout std_logic);
  end component;

  component a2a2
  port(a1 : in std_logic;
       b1 : in std_logic;
       a2 : in std_logic;
       b2 : in std_logic;
       o : inout std_logic);
  end component;

  component a3
  port(a1 : in std_logic;
       b1 : in std_logic;
       c1 : in std_logic;
       o : inout std_logic);
  end component;

  component a3a2
  port(a1 : in std_logic;
       b1 : in std_logic;
       c1 : in std_logic;
       a2 : in std_logic;
       b2 : in std_logic;
       o : inout std_logic);
  end component;

  component a4
  port(a1 : in std_logic;
       b1 : in std_logic;
       c1 : in std_logic;
       d1 : in std_logic;
       o : inout std_logic);
  end component;

begin
  i_0 : a1
    port map(a1=>cr_ack, 
             o=>cr_req);

  i_1 : a4
    port map(a1=>dpdata_ack_bar, b1=>mem_access_please_req, c1=>mem_access_please_ack, d1=>mem_rw_please_bar, 
             o=>increment_please_ack);

  i_2 : a3a2
    port map(a1=>cb_ack, b1=>increment_please_req, c1=>mem_access_please_ack_bar, 
             a2=>cb_ack, b2=>increment_please_ack_bar, 
             o=>cb_req);

  i_3 : a2
    port map(a1=>calculate_please_req, b1=>mem_access_please_ack_bar, 
             o=>splice_please_ack);

  i_4 : a1
    port map(a1=>lum_req, 
             o=>calculate_please_ack);

  i_5 : a3
    port map(a1=>dpdata_ack, b1=>splice_please_req_bar, c1=>mem_rw_please_bar, 
             o=>dpdata_req);

  i_6 : a2a2
    port map(a1=>mem_access_please_req_bar, b1=>dpdata_req, 
             a2=>splice_please_req, b2=>splice_please_ack, 
             o=>mem_access_please_ack);

  i_7 : a2
    port map(a1=>lum_ack, b1=>mem_rw_please, 
             o=>lum_req);

  i_8 : a1
    port map(a1=>lum_ack, 
             o=>mem_rw_please);

  -- complements of inputs
  dpdata_ack_bar <= not dpdata_ack after delay(0,10);
  increment_please_ack_bar <= not increment_please_ack after delay(0,10);
  mem_access_please_ack_bar <= not mem_access_please_ack after delay(0,10);
  mem_access_please_req_bar <= not mem_access_please_req after delay(0,10);
  mem_rw_please_bar <= not mem_rw_please after delay(0,10);
  splice_please_req_bar <= not splice_please_req after delay(0,10);

end structure;





