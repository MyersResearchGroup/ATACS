--
-- File : C:\vbproj\chap3\Default\genhdl\vhdl\wine_bundled2.vhd
-- CDB  : C:\vbproj\chap3\Default\Default.cdb
-- By   : CDB2VHDL Netlister version 15.0.0.9
-- Time : Mon Jan 24 11:25:42 2000

-- Entity/architecture declarations 

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity wine_bundled2 is
end wine_bundled2;

architecture wine_bundled2 of wine_bundled2 is
    -- Component declarations
    component shopPA_bundled
        port(
            ack_patron : in std_logic;
            --bottle : in std_logic_vector(2 downto 0);
            req_wine : in std_logic;
            --shelf : inout std_logic_vector(2 downto 0);
            req_patron : inout std_logic;
            ack_wine : inout std_logic
        );
    end component;
    component wineryA2
        port(
            ack_wine2 : in std_logic;
            ack_wine1 : in std_logic;
            --bottle1 : inout std_logic_vector(2 downto 0);
            req_wine1 : inout std_logic;
            --bottle2 : inout std_logic_vector(2 downto 0);
            req_wine2 : inout std_logic
        );
    end component;
    component patronP2
        port(
            --shelf2 : in std_logic_vector(2 downto 0);
            req_patron2 : in std_logic;
            --shelf1 : in std_logic_vector(2 downto 0);
            req_patron1 : in std_logic;
            ack_patron2 : inout std_logic;
            ack_patron1 : inout std_logic
        );
    end component;
    -- Signal declarations
    --signal bottle1 : std_logic_vector(2 downto 0);
    --signal bottle2 : std_logic_vector(2 downto 0);
    --signal shelf1 : std_logic_vector(2 downto 0);
    --signal shelf2 : std_logic_vector(2 downto 0);
    signal XSIG010001 : std_logic;
    signal XSIG010003 : std_logic;
    signal XSIG010004 : std_logic;
    signal XSIG010005 : std_logic;
    signal XSIG010007 : std_logic;
    signal XSIG010008 : std_logic;
    signal XSIG010009 : std_logic;
    signal XSIG010010 : std_logic;
begin
    -- Signal assignments
    -- Component instances
    shopPA_bundled1 : shopPA_bundled
        port map(
            ack_patron => XSIG010008,
            --bottle => bottle1,
            req_wine => XSIG010001,
            --shelf => shelf1,
            req_patron => XSIG010007,
            ack_wine => XSIG010003
        );
    winery_bundled1 : wineryA2
        port map(
            ack_wine2 => XSIG010005,
            ack_wine1 => XSIG010003,
            --bottle1 => bottle1,
            req_wine1 => XSIG010001,
            --bottle2 => bottle2,
            req_wine2 => XSIG010004
        );
    shopPA_bundled2 : shopPA_bundled
        port map(
            ack_patron => XSIG010010,
            --bottle => bottle2,
            req_wine => XSIG010004,
            --shelf => shelf2,
            req_patron => XSIG010009,
            ack_wine => XSIG010005
        );
    patron_bundled1 : patronP2
        port map(
            --shelf2 => shelf2,
            req_patron2 => XSIG010009,
            --shelf1 => shelf1,
            req_patron1 => XSIG010007,
            ack_patron2 => XSIG010010,
            ack_patron1 => XSIG010008
        );
end wine_bundled2;
