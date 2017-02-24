
--------

entity pipe is
  port(c0,s1 : in std_logic;
       s0 : out std_logic := '1';
       c1 : out std_logic := '0';
       ds : out std_logic := '0'
       );
end pipe;

architecture hse of pipe is
begin
pipeP:process
begin
  guard(c0,'1');
  s0 <= '0' after delay(10,10);
  guard(s1,'1');
  c1 <= '1' after delay(20,20);
  ds <= '1' after delay(10,10);
  guard_and(c0,'0',s1,'0');
  s0 <= '1' after dealy(10,10);
  c1 <= '0' after delay(20,20);
  ds <= '0' after delay(10,10);
end process;
end hse;

-------------------------------------------------------------------------------

entity env is
  port(s0,c1 : in std_logic;
       c0 : out std_logic := '0';
       s1 : out std_logic := '1');
end env;

architecture hse of env is
begin
sender:process
begin
  assign(c0,'1',10,20);
  guard(s0,'0');
  assign(c0,'0',30,40);
  guard(s0,'1');
end process;
receiver:process
begin
  guard(c1,'1');
  s1 <='0' after delay(50,60);
  guard_and(c1,'0',s1,'0');
  assign(s1,'1',70,80);
end process;
end hse;   

-------------------------------------------------------------------------------

entity test is
  port(ds : out std_logic);
end test;

architecture test of test is
    -- Component declarations
    component pipe
      port(c0,s1 : in std_logic;
           s0 : out std_logic;
           c1 : out std_logic;
           ds : out std_logic);
    end component;
    component env
      port(s0,c1 : in std_logic;
           c0 : out std_logic;
           s1 : out std_logic);
    end component;
    -- Signal declarations
    signal c0 : std_logic := '0';      
    signal s1 : std_logic := '1';      
    signal c1 : std_logic := '0';      
    signal s0 : std_logic := '1';      
begin
    -- Signal assignments
    -- Component instances
    pipe1 : pipe
        port map(c0,s1,s0,c1,ds);
    env1 : env
        port map(s0,c1,c0,s1);
end test;

-------------------------------------------------------------------------------

