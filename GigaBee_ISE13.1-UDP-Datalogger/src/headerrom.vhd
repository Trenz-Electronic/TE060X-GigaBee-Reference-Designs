-------------------------------------------------------------------------------
-- Project: Data Logger with Ethernet UDP output
-- Author(s): DFC Design, s.r.o. (www.dfcdesign.cz)
-- E-Mail:  info@dfcdesign.cz
-- Created: Sept 2011   
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--                                                                           --
-- Copyright (C) 2011 Authors                                                --
--                                                                           --
-- This source file may be used and distributed without                      --
-- restriction provided that this copyright statement is not                 --
-- removed from the file and that any derivative work contains               --
-- the original copyright notice and the associated disclaimer.              --
--                                                                           --
-- This source file is free software; you can redistribute it                --
-- and-or modify it under the terms of the GNU Lesser General                --
-- Public License as published by the Free Software Foundation;              --
-- either version 3.0 of the License, or (at your option) any                --
-- later version.                                                            --
--                                                                           --
-- This source is distributed in the hope that it will be                    --
-- useful, but WITHOUT ANY WARRANTY; without even the implied                --
-- warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                   --
-- PURPOSE.  See the GNU Lesser General Public License for more              --
-- details.                                                                  --
--                                                                           --
-- You should have received a copy of the GNU Lesser General                 --
-- Public License along with this source; If not, see                        --
-- <http://www.gnu.org/licenses/>                                            --
--                                                                           --
------------------------------------------------------------------------------- 


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity headerrom is

	generic 
	(
		DATA_WIDTH : natural := 8;
		ADDR_WIDTH : natural := 8
	);

	port 
	(
		clk		: in std_logic;
		addr	: in natural range 0 to 2**ADDR_WIDTH - 1;
		q		: out std_logic_vector((DATA_WIDTH -1) downto 0)
	);

end entity;

architecture rtl of headerrom is

	-- Build a 2-D array type for the RoM
	subtype word_t is std_logic_vector((DATA_WIDTH-1) downto 0);
	type memory_t is array(0 to 2**ADDR_WIDTH-1) of word_t;

 

	-- Declare the ROM signal and specify a default value.	
	signal rom : memory_t :=
	(
	-- Preamble + sof delimiter 8 B
   x"55", x"55", x"55", x"55", x"55", x"55", x"55", x"d5",
   
   -- Eth Header 14 B
   x"ff", x"ff", x"ff", x"ff", x"ff", x"ff", -- Destination MAC
   x"66", x"77", x"44", x"22", x"33", x"11", -- Src MAC
   x"08", x"00",  -- Ethertype

   -- IP Header 20 B (if changed, IP chkSum must be recalculated)
   x"45", x"00", x"05", x"2c", 
   x"61", x"d2", x"00", x"00", 
   x"80", x"11", x"13", x"46", 
   x"c0", x"a8", x"00", x"01", -- SRC IP address
   x"ff", x"ff", x"ff", x"ff", -- DST IP address

   -- UDP Header 8 B
   x"20", x"08", -- DST Port
   x"20", x"08", -- SRC Port
   x"05", x"18", -- Lenght
   x"00", x"00", -- Checksum 
   
   -- Our Header 16 B
   x"ee", x"ee", -- ID protocol
   x"ee", x"ee", -- Length
   x"ee", x"ee", x"ee", x"ee", -- ID Packet
   x"00", x"ee", -- ID Board
   x"ee", x"ee", -- Flags
   x"00", x"00", x"00", x"00",

   -- padding 78 B
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", 
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", 
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", 
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc",
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", 
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", 

  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", x"cc", 
  x"cc", x"cc", x"cc", x"cc", x"cc", x"cc"

	);

begin

	process(clk)
	begin
	if(rising_edge(clk)) then
      q <= rom(addr);
	end if;
	end process;

end rtl;
