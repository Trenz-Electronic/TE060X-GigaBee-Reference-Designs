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

Package pkg_rst_gen is
   Component rst_gen is
   Generic(
      rst_cycles : positive; 
      rst1_cycles : positive
          );
   Port(
      clk      : In  std_logic;
      ext_rst  : In  std_logic;
      rst      : Out std_logic;
      rst1     : Out std_logic
       );
   End Component;
End Package;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

Entity rst_gen is
   Generic(
      rst_cycles  : positive := 128;
      rst1_cycles : positive := 128
          );
   Port(
      clk      : In  std_logic;
      ext_rst  : In  std_logic;
      rst      : Out std_logic;
      rst1     : Out std_logic
       );
End Entity;

Architecture arch of rst_gen is
   Signal rst_cnt : integer range 0 to rst_cycles - 1 := 0;
   Signal done    : std_logic := '0';
Begin
   Process(clk)
   Begin
      If clk'Event and clk = '1' Then
         If ext_rst = '1' Then
            rst_cnt <= 0;
            done    <= '0';
         ElsIf rst_cnt < rst_cycles - 1 Then
            rst_cnt <= rst_cnt + 1;
         Else
            done <= '1';
         End If;
      End If;
   End Process;
      
   rst1 <= '1' When rst_cnt < rst1_cycles - 1 Else '0';
   rst  <= not done;

End arch;
