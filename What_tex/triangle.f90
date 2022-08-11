!* +-----------------------------------------------------------------------
!* |                         T R I A N G L E
!* | Main program that calls WHAT_TEXTURE, a function that classifies soil
!* | in the USDA textural triangle using sand and clay %
!* +-----------------------------------------------------------------------
!* | Created by: aris gerakis, apr. 98 with help from brian baer
!* | Modified by: aris gerakis, july 99: now all borderline cases are valid
!* | Modified by: aris gerakis, apr 2001: add a sample ID to input file 
!* | and make it comma-delimited.
!* +-----------------------------------------------------------------------
!* | COMMENTS
!* | o Supply a data file with three comma-delimited columns, in free format:  
!* |   1st column sample ID, 2nd column sand %, 3rd column clay %, no header.  
!* |   The output is a file with the classes.
!* +-----------------------------------------------------------------------
!* | You may use, distribute and modify this code provided you maintain
!* ! this header and give appropriate credit.
!* +-----------------------------------------------------------------------

program triangle

implicit none

integer, parameter:: max_line_len = 250

integer    :: finish, flstat, flstat1, flstat2, line_num, start
real       :: sand, clay
character  :: infile*12, line*max_line_len, outfile*12, sampleID*max_line_len, &
              sand_str*max_line_len, clay_str*max_line_len, texture*42, what_texture*42
logical    :: eof


write (unit = 6, fmt = 3000)
3000 format (/, 10x, 'Classify soils in the USDA triangle using sand and clay %.', /, &
                10x, 'By aris gerakis, april 2001,  with help from  brian  baer.', /, &
                10x, 'Supply a text (ASCII) comma-delimited file with three col-', /, &
                10x, 'umns:  1st column sample ID, 2nd column sand %, 3rd column', /, &
                10x, 'clay %. Lines with # or ! at the beginning will be treated', /, & 
				10x, 'as comment lines.  Output is comma-delimited file that can', /, &
                10x, 'be readily imported  to a spreadsheet.  Send feedback  to:', /, &
				10x, 'gerakis@msu.edu.', /)

! Open input-output files:

write (unit = 6, fmt = '(10x, $a)') 'Enter input filename up to 12 characters: '
read (unit = 5, fmt = '(a)') infile
open (unit = 10, file = infile, status = 'old')
open (unit = 20, file = 'soilclas.csv', status = 'unknown')

eof = .false.
line_num = 0

do while(.not. eof)

   sampleID = ' '
   sand_str = ' '
   clay_str = ' '
   start = 0
   finish = 0
   sand = 0.0
   clay = 0.0

   ! Read and parse input:

   read (unit = 10, fmt = '(a<max_line_len>)', iostat = flstat) line

   line_num = line_num + 1
      
   if (flstat .eq. 0) then

	  if (line(1:1) .eq. '#' .or. line(1:1) .eq. '!') then 
         ! Process comments:
         write (unit = 20, fmt = '(a)') '"'//trim(line)//'"'
      else 
         ! Process data:
         start = verify (trim(line), ' ')
         finish = start + scan (trim(line(start:)), ',') - 2
         sampleID = line(start:finish)

         start = finish + 2
         finish = start + scan (trim(line(start:)), ',')  - 2
         sand_str = line(start:finish)
         read (unit = sand_str, fmt = '(f<finish-start+1>.0)', iostat = flstat1) sand

         if (flstat1 .eq. 0) then
            start = finish + 2
            finish = start + verify (trim(line(start:)), ',', back = .true.) - 1
            clay_str = line(start:finish)
            read (unit = clay_str, fmt = '(f<finish-start+1>.0)', iostat = flstat2) clay

            if (flstat2 .eq. 0) then
               ! Call function that estimates texture:
               texture = what_texture (sand, clay)
               ! Write output:
               write (unit = 20, fmt = 2000) trim(sampleID), sand, clay, texture(2:len_trim(texture))
               2000  format ('"', a, '"', ',', 2(1x, f5.1, ','), 1x, a)
            else
               write (unit = 20, fmt = 1000) line_num
               write (unit = 6, fmt = 1000) line_num
            endif
         else
            write (unit = 20, fmt = 1000) line_num
            write (unit = 6, fmt = 1000) line_num
         endif
	  endif
   elseif (flstat .eq. -1) then
      eof = .true.
   else
      write (unit = 20, fmt = 1000) line_num
      write (unit = 6, fmt = 1000) line_num
      1000  format (1x, 'ERROR in line ', i, ' - please check your inputs') 
   endif

enddo

write (unit = 6, fmt = '(10x, a)') 'Your output file is soilclas.csv'

close (unit = 10)
close (unit = 20)

end program triangle
