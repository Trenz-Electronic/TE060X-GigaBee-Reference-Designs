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

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;

---- Uncomment the following library declaration if instantiating
---- any Xilinx primitives in this code.
library UNISIM;
use UNISIM.VComponents.all;

Entity ethoutput Is
   Generic(
      bit_num  : positive := 1
          );
   Port(
      datain_h		: IN STD_LOGIC_VECTOR (bit_num - 1 DOWNTO 0);
		datain_l		: IN STD_LOGIC_VECTOR (bit_num - 1 DOWNTO 0);
		outclock		: IN STD_LOGIC ;
		dataout		: OUT STD_LOGIC_VECTOR (bit_num - 1 DOWNTO 0)
       );

End ethoutput;

Architecture Behavioral Of ethoutput Is
	signal noutclock : std_logic;
Begin

	noutclock <= not outclock;

   g1 : For i In 0 To bit_num - 1 Generate
		output_ff : FDCE
      generic map (
         INIT => '0')
      port map (
         Q => dataout(i),      -- Data output
         C => outclock,      -- Clock input
         CE => '1',    -- Clock enable input
         CLR => '0',  -- Asynchronous clear input
         D => datain_h(i)       -- Data input      
         );    
   End Generate;
  
   -- End of ODDR_inst instantiation
   

End Behavioral;

