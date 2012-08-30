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

-- Shift register with load
-- Intendet for mux replacement

Package pkg_shift_reg_ld Is
   Component shift_reg_ld Is
      Generic(
         word_width  : positive;
         stages_num  : positive
             );
      Port(
         clk         : In  std_logic;
         rst         : In  std_logic;

         ld          : In  std_logic;  -- load data
         oe          : In  std_logic;  -- shift data out
      
         datain      : In  std_logic_vector(word_width * stages_num - 1 downto 0);
         dataout     : Out std_logic_vector(word_width -1 downto 0)
          );
   End Component;
End package;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

Entity shift_reg_ld Is
   Generic(
      word_width  : positive := 8;
      stages_num  : positive := 2
          );
   Port(
      clk         : In  std_logic;
      rst         : In  std_logic;

      ld          : In  std_logic;  -- load data
      oe          : In  std_logic;  -- shift data out
      
      datain      : In  std_logic_vector(word_width * stages_num - 1 downto 0);
      dataout     : Out std_logic_vector(word_width -1 downto 0)
       );
End Entity;

Architecture arch Of shift_reg_ld Is
   Subtype data_word_t Is std_logic_vector(word_width -1 downto 0);
   Type reg_stages_t   Is array (stages_num - 1 downto 0) of data_word_t;

   signal reg_stages : reg_stages_t := (others => (others => '0'));
Begin

   Process(clk, rst, ld)
      variable i : integer;
   Begin
      If rst = '1' Then
         reg_stages <= (others => (others => '0'));
      ElsIf clk'Event And clk = '1' Then
         If ld = '1' Then
            For i in 0 to stages_num - 1 Loop
               reg_stages(i) <= datain(word_width * (i + 1) - 1 downto word_width * i);
            End Loop;
         ElsIf oe = '1' Then 
            reg_stages(stages_num - 1) <= (others => '0');
            For i in 0 to stages_num - 2 Loop
               reg_stages(i) <= reg_stages(i + 1) ;
            End Loop;
         Else
            reg_stages <= reg_stages;
         End If;
      End If;
   End Process;

   dataout <= reg_stages(0);

End Arch;
