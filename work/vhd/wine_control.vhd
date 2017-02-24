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
    component shopAP
        port(
            req_wine : inout std_logic;
            ack_patron : inout std_logic;
            req_patron : in std_logic;
            ack_wine : in std_logic
        );
    end component;
    component patronA
        port(
            req_patron : inout std_logic;
            ack_patron : in std_logic
        );
    end component;
    component wineryP
        port(
            ack_wine : inout std_logic;
            req_wine : in std_logic
        );
    end component;
    -- Signal declarations
    signal ack_patron : std_logic;
    signal ack_wine : std_logic;
    signal req_patron : std_logic;
    signal req_wine : std_logic;
begin
    -- Signal assignments
    -- Component instances
    shopAP_bundled2 : shopAP
        port map(
            req_wine => req_wine,
            ack_patron => ack_patron,
            req_patron => req_patron,
            ack_wine => ack_wine
        );
    patron_bundled4 : patronA
        port map(
            req_patron => req_patron,
            ack_patron => ack_patron
        );
    winery_bundled2 : wineryP
        port map(
            ack_wine => ack_wine,
            req_wine => req_wine
        );
end wine_bundled;
