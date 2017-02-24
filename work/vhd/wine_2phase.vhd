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
    component shopPA_2phase
        port(
            req_wine : in std_logic;
            --bottle : in std_logic_vector(2 downto 0);
            ack_patron : in std_logic;
            req_patron : inout std_logic;
            ack_wine : inout std_logic
            --shelf : inout std_logic_vector(2 downto 0)
        );
    end component;
    component patron_bundled
        port(
            req_patron : in std_logic;
            --shelf : in std_logic_vector(2 downto 0);
            ack_patron : inout std_logic
        );
    end component;
    component winery_bundled
        port(
            ack_wine : in std_logic;
            req_wine : inout std_logic
            --bottle : inout std_logic_vector(2 downto 0)
        );
    end component;
    -- Signal declarations
    signal ack_patron : std_logic;
    signal ack_wine : std_logic;
    --signal bottle : std_logic_vector(2 downto 0);
    signal req_patron : std_logic;
    signal req_wine : std_logic;
    --signal shelf : std_logic_vector(2 downto 0);
begin
    -- Signal assignments
    -- Component instances
    shopPA_2phase2 : shopPA_2phase
        port map(
            req_wine => req_wine,
            --bottle => bottle,
            ack_patron => ack_patron,
            req_patron => req_patron,
            ack_wine => ack_wine
            --shelf => shelf
        );
    patron_bundled4 : patron_bundled
        port map(
            req_patron => req_patron,
            --shelf => shelf,
            ack_patron => ack_patron
        );
    winery_bundled2 : winery_bundled
        port map(
            ack_wine => ack_wine,
            req_wine => req_wine
            --bottle => bottle
        );
end wine_bundled;
