--
-- File : C:\vbproj\chap3\Default\genhdl\vhdl\wine_dualrail3.vhd
-- CDB  : C:\vbproj\chap3\default\default.cdb
-- By   : CDB2VHDL Netlister version 16.0.0.8
-- Time : Sat Jan 22 00:24:36 2000

-- Entity/architecture declarations 

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;

entity wine_dualrail3 is
end wine_dualrail3;

architecture wine_dualrail3 of wine_dualrail3 is
    -- Component declarations
    component shop_dualrail
        port(
            bottle1 : in std_logic;
            bottle0 : in std_logic;
            ack_patron : in std_logic;
            ack_wine : inout std_logic;
            shelf1 : inout std_logic;
            shelf0 : inout std_logic
        );
    end component;
    component patron_dualrail3
        port(
            shelf2_1 : in std_logic;
            shelf2_0 : in std_logic;
            shelf1_1 : in std_logic;
            shelf1_0 : in std_logic;
            shelf0_1 : in std_logic;
            shelf0_0 : in std_logic;
            ack_patron0 : out std_logic;
            ack_patron2 : out std_logic;
            ack_patron1 : out std_logic
        );
    end component;
    component winery_dualrail3
        port(
            ack_wine2 : in std_logic;
            ack_wine1 : in std_logic;
            ack_wine0 : in std_logic;
            bottle2_1 : inout std_logic;
            bottle1_1 : inout std_logic;
            bottle1_0 : inout std_logic;
            bottle2_0 : inout std_logic;
            bottle0_1 : inout std_logic;
            bottle0_0 : inout std_logic
        );
    end component;
    -- Signal declarations
    signal ack_patron0 : std_logic;
    signal ack_patron1 : std_logic;
    signal ack_patron2 : std_logic;
    signal ack_wine0 : std_logic;
    signal ack_wine1 : std_logic;
    signal ack_wine2 : std_logic;
    signal bottle0_0 : std_logic;
    signal bottle0_1 : std_logic;
    signal bottle1_0 : std_logic;
    signal bottle1_1 : std_logic;
    signal bottle2_0 : std_logic;
    signal bottle2_1 : std_logic;
    signal shelf0_0 : std_logic;
    signal shelf0_1 : std_logic;
    signal shelf1_0 : std_logic;
    signal shelf1_1 : std_logic;
    signal shelf2_0 : std_logic;
    signal shelf2_1 : std_logic;
begin
    -- Signal assignments
    -- Component instances
    shop_dualra1 : shop_dualrail
        port map(
            bottle1 => bottle2_1,
            bottle0 => bottle2_0,
            ack_patron => ack_patron2,
            ack_wine => ack_wine2,
            shelf1 => shelf2_1,
            shelf0 => shelf2_0
        );
    shop_dualra4 : shop_dualrail
        port map(
            bottle1 => bottle1_1,
            bottle0 => bottle1_0,
            ack_patron => ack_patron1,
            ack_wine => ack_wine1,
            shelf1 => shelf1_1,
            shelf0 => shelf1_0
        );
    shop_dualra5 : shop_dualrail
        port map(
            bottle1 => bottle0_1,
            bottle0 => bottle0_0,
            ack_patron => ack_patron0,
            ack_wine => ack_wine0,
            shelf1 => shelf0_1,
            shelf0 => shelf0_0
        );
    patron_dualrail1 : patron_dualrail3
        port map(
            shelf2_1 => shelf2_1,
            shelf2_0 => shelf2_0,
            shelf1_1 => shelf1_1,
            shelf1_0 => shelf1_0,
            shelf0_1 => shelf0_1,
            shelf0_0 => shelf0_0,
            ack_patron0 => ack_patron0,
            ack_patron2 => ack_patron2,
            ack_patron1 => ack_patron1
        );
    winery_dualrail2 : winery_dualrail3
        port map(
            ack_wine2 => ack_wine2,
            ack_wine1 => ack_wine1,
            ack_wine0 => ack_wine0,
            bottle2_1 => bottle2_1,
            bottle1_1 => bottle1_1,
            bottle1_0 => bottle1_0,
            bottle2_0 => bottle2_0,
            bottle0_1 => bottle0_1,
            bottle0_0 => bottle0_0
        );
end wine_dualrail3;
